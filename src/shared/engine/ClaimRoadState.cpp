#include "ClaimRoadState.h"

#include <json/json.h>

#include <cctype>

#include <sstream>

#include "ConfirmationState.h"
#include "Engine.h"
#include "EngineCommand.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "TunnelResolveState.h"
#include "cardsState/CardsState.h"
#include "cardsState/ColorCard.h"
#include "mapState/Ferry.h"
#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "mapState/RoadColor.h"
#include "mapState/Tunnel.h"
#include "playersState/Player.h"
#include "playersState/PlayersState.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::SETUP;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }

    EngineResult buildInfo(Phase nextPhase, const std::string& message)
    {
      EngineResult result;
      result.ok = true;
      result.error = "";
      result.nextPhase = nextPhase;

      EngineEvent event;
      event.type = EngineEventType::INFO;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
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

    int readRoadId(const Json::Value& root)
    {
      if (root.isMember("roadId"))
      {
        return root["roadId"].asInt();
      }
      if (root.isMember("id"))
      {
        return root["id"].asInt();
      }
      if (root.isMember("road_id"))
      {
        return root["road_id"].asInt();
      }
      return -1;
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

    cardsState::ColorCard roadColorToCard(mapState::RoadColor color)
    {
      switch (color)
      {
        case mapState::RoadColor::RED: return cardsState::ColorCard::RED;
        case mapState::RoadColor::BLUE: return cardsState::ColorCard::BLUE;
        case mapState::RoadColor::GREEN: return cardsState::ColorCard::GREEN;
        case mapState::RoadColor::BLACK: return cardsState::ColorCard::BLACK;
        case mapState::RoadColor::YELLOW: return cardsState::ColorCard::YELLOW;
        case mapState::RoadColor::ORANGE: return cardsState::ColorCard::ORANGE;
        case mapState::RoadColor::PINK: return cardsState::ColorCard::PINK;
        case mapState::RoadColor::WHITE: return cardsState::ColorCard::WHITE;
        case mapState::RoadColor::NONE:
        case mapState::RoadColor::UNKNOWN:
        default:
          return cardsState::ColorCard::UNKNOWN;
      }
    }

    cardsState::ColorCard pickBestColor(cardsState::CardsState& cardsState,
                                        const std::shared_ptr<cardsState::PlayerCards>& hand)
    {
      cardsState::ColorCard colors[] = {
        cardsState::ColorCard::RED,
        cardsState::ColorCard::BLUE,
        cardsState::ColorCard::GREEN,
        cardsState::ColorCard::BLACK,
        cardsState::ColorCard::YELLOW,
        cardsState::ColorCard::ORANGE,
        cardsState::ColorCard::PINK,
        cardsState::ColorCard::WHITE
      };
      int bestCount = 0;
      cardsState::ColorCard best = cardsState::ColorCard::UNKNOWN;
      for (std::size_t i = 0; i < sizeof(colors) / sizeof(colors[0]); ++i)
      {
        int count = cardsState.countWagonCards(hand, colors[i], false);
        if (count > bestCount)
        {
          bestCount = count;
          best = colors[i];
        }
      }
      return best;
    }
  }

  void ClaimRoadState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::CLAIM_ROAD;
  }

  EngineResult ClaimRoadState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Claim road: engine not initialized");
    }
    if (command.type != EngineCommandType::CMD_TAKE_ROAD)
    {
      return buildError(engine, "Claim road: command not allowed");
    }

    Json::Value root;
    std::string error;
    if (!parsePayload(command.payload, root, error))
    {
      return buildError(engine, error);
    }

    int roadId = readRoadId(root);
    if (roadId < 0)
    {
      return buildError(engine, "Claim road: missing road id");
    }

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      return buildError(engine, "Claim road: missing state");
    }
    std::shared_ptr<mapState::MapState> mapState(state, &state->map);
    if (!mapState)
    {
      return buildError(engine, "Claim road: missing map");
    }
    std::shared_ptr<cardsState::CardsState> cardsState(state, &state->cards);
    if (!cardsState)
    {
      return buildError(engine, "Claim road: missing cards state");
    }

    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    int playerIndex = engine->context.currentPlayer;
    if (players.empty() || playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
      return buildError(engine, "Claim road: invalid player");
    }
    std::shared_ptr<playersState::Player> player = players[playerIndex];
    if (!player)
    {
      return buildError(engine, "Claim road: missing player");
    }

    std::shared_ptr<mapState::Road> road = mapState->getRoadByID(roadId);
    if (!road)
    {
      return buildError(engine, "Claim road: road not found");
    }

    if (!playersState::PlayersState::isRoadClaimable(mapState, road, player))
    {
      return buildError(engine, "Claim road: road not claimable");
    }

    if (!player->isRoadBuildable(mapState, road))
    {
      return buildError(engine, "Claim road: insufficient resources");
    }

    std::shared_ptr<cardsState::PlayerCards> hand = player->getHand();
    if (!hand)
    {
      return buildError(engine, "Claim road: missing player hand");
    }

    if (std::dynamic_pointer_cast<mapState::Tunnel>(road))
    {
      engine->context.pendingTunnel.route = road;
      engine->context.pendingTunnel.baseLength = road->getLength();
      engine->context.pendingTunnel.extraRequired = 0;
      engine->context.pendingTunnel.revealed.clear();

      cardsState::ColorCard selected = cardsState::ColorCard::UNKNOWN;
      if (root.isMember("color") && root["color"].isString())
      {
        selected = parseColorToken(root["color"].asString());
      }
      if (selected == cardsState::ColorCard::UNKNOWN)
      {
        selected = roadColorToCard(road->getColor());
      }
      if (selected == cardsState::ColorCard::UNKNOWN)
      {
        selected = pickBestColor(*cardsState, hand);
      }
      if (selected == cardsState::ColorCard::UNKNOWN || selected == cardsState::ColorCard::LOCOMOTIVE)
      {
        return buildError(engine, "Claim road: missing tunnel color selection");
      }
      engine->context.pendingTunnel.color = static_cast<mapState::RoadColor>(selected);

      std::shared_ptr<GameState> nextState(new TunnelResolveState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }

    cardsState::ColorCard roadColor = roadColorToCard(road->getColor());
    if (roadColor == cardsState::ColorCard::UNKNOWN)
    {
      if (root.isMember("color") && root["color"].isString())
      {
        roadColor = parseColorToken(root["color"].asString());
      }
    }
    if (roadColor == cardsState::ColorCard::UNKNOWN)
    {
      roadColor = pickBestColor(*cardsState, hand);
    }
    if (roadColor == cardsState::ColorCard::UNKNOWN)
    {
      return buildError(engine, "Claim road: missing color selection");
    }

    int length = road->getLength();
    if (!cardsState->discardWagonCards(hand, roadColor, length, true))
    {
      return buildError(engine, "Claim road: unable to discard cards");
    }
    player->removeTrain(length);
    road->setOwner(player);

    std::shared_ptr<GameState> nextState(new ConfirmationState());
    engine->stateMachine->transitionTo(engine, nextState);
    return buildInfo(Phase::CONFIRMATION, "Road claimed");
  }
}
