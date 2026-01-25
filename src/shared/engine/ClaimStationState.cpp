#include "ClaimStationState.h"

#include <json/json.h>

#include <cctype>
#include <sstream>

#include "ConfirmationState.h"
#include "PlayerTurnState.h"
#include "Engine.h"
#include "EngineCommand.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "cardsState/CardsState.h"
#include "cardsState/ColorCard.h"
#include "mapState/MapState.h"
#include "mapState/Station.h"
#include "playersState/Player.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::CLAIM_STATION;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }

    EngineResult buildErrorAndReturn(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result = buildError(engine, message);
      if (engine && engine->stateMachine)
      {
        std::shared_ptr<GameState> nextState(new PlayerTurnState());
        engine->stateMachine->transitionTo(engine, nextState);
        result.nextPhase = Phase::PLAYER_TURN;
      }
      return result;
    }

    void addInfo(EngineResult& result, const std::string& message)
    {
      EngineEvent event;
      event.type = EngineEventType::INFO;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);
    }

    bool parsePayload(const std::string& payload, Json::Value& root, std::string& error)
    {
      error.clear();
      if (payload.empty())
      {
        error = "Missing payload";
        return false;
      }
      Json::CharReaderBuilder builder;
      builder["collectComments"] = false;
      std::string errs;
      std::istringstream stream(payload);
      if (!Json::parseFromStream(builder, stream, &root, &errs))
      {
        error = "Invalid payload: " + errs;
        return false;
      }
      if (!root.isObject())
      {
        error = "Payload must be an object";
        return false;
      }
      return true;
    }

    cardsState::ColorCard parseColorToken(const std::string& token)
    {
      std::string upper;
      for (std::size_t i = 0; i < token.size(); ++i)
      {
        upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(token[i]))));
      }
      if (upper == "RED") return cardsState::ColorCard::RED;
      if (upper == "BLUE") return cardsState::ColorCard::BLUE;
      if (upper == "GREEN") return cardsState::ColorCard::GREEN;
      if (upper == "BLACK") return cardsState::ColorCard::BLACK;
      if (upper == "YELLOW") return cardsState::ColorCard::YELLOW;
      if (upper == "ORANGE") return cardsState::ColorCard::ORANGE;
      if (upper == "PINK") return cardsState::ColorCard::PINK;
      if (upper == "WHITE") return cardsState::ColorCard::WHITE;
      if (upper == "LOCOMOTIVE") return cardsState::ColorCard::LOCOMOTIVE;
      return cardsState::ColorCard::UNKNOWN;
    }

    int stationCost(int nbStations)
    {
      // station cost grows as stations get used (3->1, 2->2, 1->3)
      if (nbStations >= 3)
      {
        return 1;
      }
      if (nbStations == 2)
      {
        return 2;
      }
      if (nbStations == 1)
      {
        return 3;
      }
      return 0;
    }
  }

  void ClaimStationState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (engine)
    {
      engine->phase = Phase::CLAIM_STATION;
    }
  }

  EngineResult ClaimStationState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Claim station: engine not initialized");
    }
    if (command.type != EngineCommandType::CMD_CLAIM_STATION)
    {
      return buildError(engine, "Claim station: command not allowed");
    }

    Json::Value root;
    std::string error;
    if (!parsePayload(command.payload, root, error))
    {
      return buildErrorAndReturn(engine, error);
    }

    if (!root.isMember("station") || !root["station"].isString())
    {
      return buildErrorAndReturn(engine, "Claim station: missing station name");
    }
    std::string stationName = root["station"].asString();

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      return buildErrorAndReturn(engine, "Claim station: missing state");
    }
    std::shared_ptr<mapState::MapState> mapState(state, &state->map);
    std::shared_ptr<cardsState::CardsState> cardsState(state, &state->cards);
    if (!mapState || !cardsState)
    {
      return buildErrorAndReturn(engine, "Claim station: missing map/cards");
    }

    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    int playerIndex = engine->context.currentPlayer;
    if (players.empty() || playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
      return buildErrorAndReturn(engine, "Claim station: invalid player");
    }
    std::shared_ptr<playersState::Player> player = players[playerIndex];
    if (!player)
    {
      return buildErrorAndReturn(engine, "Claim station: missing player");
    }
    std::shared_ptr<cardsState::PlayerCards> hand = player->getHand();
    if (!hand)
    {
      return buildErrorAndReturn(engine, "Claim station: missing hand");
    }

    std::shared_ptr<mapState::Station> station = mapState->getStationByName(stationName);
    if (!station)
    {
      return buildErrorAndReturn(engine, "Claim station: station not found");
    }
    if (station->getOwner())
    {
      return buildErrorAndReturn(engine, "Claim station: station already owned");
    }

    int remainingStations = player->getNbStations();
    if (remainingStations <= 0)
    {
      return buildErrorAndReturn(engine, "Claim station: no stations remaining");
    }

    int cost = stationCost(remainingStations);
    cardsState::ColorCard chosen = cardsState::ColorCard::UNKNOWN;
    if (root.isMember("color") && root["color"].isString())
    {
      chosen = parseColorToken(root["color"].asString());
    }
    if (chosen == cardsState::ColorCard::UNKNOWN)
    {
      return buildErrorAndReturn(engine, "Claim station: missing color selection");
    }

    if (cardsState->countWagonCards(hand, chosen, true) < cost)
    {
      return buildErrorAndReturn(engine, "Claim station: insufficient cards");
    }
    if (!cardsState->discardWagonCards(hand, chosen, cost, true))
    {
      return buildErrorAndReturn(engine, "Claim station: failed to discard cards");
    }

    station->setOwner(player);
    player->setNbStations(remainingStations - 1);

    EngineResult result;
    result.ok = true;
    result.error = "";
    result.nextPhase = Phase::CONFIRMATION;
    addInfo(result, "Station claimed.");

    std::shared_ptr<GameState> nextState(new ConfirmationState());
    engine->stateMachine->transitionTo(engine, nextState);
    return result;
  }

  std::vector<EngineCommandType> ClaimStationState::getAllowedCommands()
  {
    std::vector<EngineCommandType> allowed;
    allowed.push_back(EngineCommandType::CMD_CLAIM_STATION);
    allowed.push_back(EngineCommandType::CMD_EXIT);
    return allowed;
  }
}
