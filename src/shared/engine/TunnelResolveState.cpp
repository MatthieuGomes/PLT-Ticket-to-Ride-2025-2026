#include "TunnelResolveState.h"

#include <json/json.h>

#include <cctype>
#include <sstream>

#include "ConfirmationState.h"
#include "Engine.h"
#include "EngineCommand.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "cardsState/CardsState.h"
#include "cardsState/SharedDeck.h"
#include "cardsState/WagonCard.h"
#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "mapState/Tunnel.h"
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
      result.nextPhase = engine ? engine->phase : Phase::SETUP;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

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

    cardsState::ColorCard roadColorToCard(mapState::RoadColor roadColor)
    {
      switch (roadColor)
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

    mapState::RoadColor cardToRoadColor(cardsState::ColorCard color)
    {
      switch (color)
      {
        case cardsState::ColorCard::RED: return mapState::RoadColor::RED;
        case cardsState::ColorCard::BLUE: return mapState::RoadColor::BLUE;
        case cardsState::ColorCard::GREEN: return mapState::RoadColor::GREEN;
        case cardsState::ColorCard::BLACK: return mapState::RoadColor::BLACK;
        case cardsState::ColorCard::YELLOW: return mapState::RoadColor::YELLOW;
        case cardsState::ColorCard::ORANGE: return mapState::RoadColor::ORANGE;
        case cardsState::ColorCard::PINK: return mapState::RoadColor::PINK;
        case cardsState::ColorCard::WHITE: return mapState::RoadColor::WHITE;
        case cardsState::ColorCard::UNKNOWN:
        case cardsState::ColorCard::LOCOMOTIVE:
        default:
          return mapState::RoadColor::UNKNOWN;
      }
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

    std::string colorToString(cardsState::ColorCard color)
    {
      switch (color)
      {
        case cardsState::ColorCard::RED: return "RED";
        case cardsState::ColorCard::BLUE: return "BLUE";
        case cardsState::ColorCard::GREEN: return "GREEN";
        case cardsState::ColorCard::BLACK: return "BLACK";
        case cardsState::ColorCard::YELLOW: return "YELLOW";
        case cardsState::ColorCard::ORANGE: return "ORANGE";
        case cardsState::ColorCard::PINK: return "PINK";
        case cardsState::ColorCard::WHITE: return "WHITE";
        case cardsState::ColorCard::LOCOMOTIVE: return "LOCOMOTIVE";
        case cardsState::ColorCard::UNKNOWN:
        default:
          return "UNKNOWN";
      }
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
  }

  void TunnelResolveState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::TUNNEL_RESOLVE;
  }

  EngineResult TunnelResolveState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Tunnel resolve: engine not initialized");
    }
    if (command.type != EngineCommandType::CMD_TUNNEL_COLOR)
    {
      return buildError(engine, "Tunnel resolve: command not allowed");
    }

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      return buildError(engine, "Tunnel resolve: missing state");
    }
    std::shared_ptr<mapState::MapState> mapState(state, &state->map);
    std::shared_ptr<cardsState::CardsState> cardsState(state, &state->cards);
    if (!mapState || !cardsState)
    {
      return buildError(engine, "Tunnel resolve: missing map/cards");
    }

    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    int playerIndex = engine->context.currentPlayer;
    if (players.empty() || playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
      return buildError(engine, "Tunnel resolve: invalid player");
    }
    std::shared_ptr<playersState::Player> player = players[playerIndex];
    if (!player)
    {
      return buildError(engine, "Tunnel resolve: missing player");
    }

    std::shared_ptr<mapState::Road> road = engine->context.pendingTunnel.route;
    if (!road)
    {
      return buildError(engine, "Tunnel resolve: no pending tunnel");
    }
    if (!std::dynamic_pointer_cast<mapState::Tunnel>(road))
    {
      return buildError(engine, "Tunnel resolve: pending road is not a tunnel");
    }

    if (!playersState::PlayersState::isRoadClaimable(mapState, road, player))
    {
      return buildError(engine, "Tunnel resolve: road not claimable");
    }
    if (!player->isRoadBuildable(mapState, road))
    {
      return buildError(engine, "Tunnel resolve: insufficient resources");
    }

    Json::Value root;
    std::string error;
    if (!parsePayload(command.payload, root, error))
    {
      return buildError(engine, error);
    }

    cardsState::ColorCard selected = cardsState::ColorCard::UNKNOWN;
    if (root.isMember("color") && root["color"].isString())
    {
      selected = parseColorToken(root["color"].asString());
    }
    if (selected == cardsState::ColorCard::UNKNOWN || selected == cardsState::ColorCard::LOCOMOTIVE)
    {
      return buildError(engine, "Tunnel resolve: invalid color selection");
    }
    engine->context.pendingTunnel.color = cardToRoadColor(selected);

    std::shared_ptr<cardsState::SharedDeck<cardsState::WagonCard>> deck = cardsState->gameWagonCards;
    if (!deck || !deck->faceDownCards || !deck->trash)
    {
      return buildError(engine, "Tunnel resolve: wagon deck not ready");
    }

    if (deck->faceDownCards->cards.size() < 3)
    {
      return buildError(engine, "Tunnel resolve: not enough cards in deck");
    }

    EngineResult result;
    result.ok = true;
    result.error = "";
    result.nextPhase = Phase::TUNNEL_RESOLVE;
    addInfo(result, "Using color " + colorToString(selected) + " for claim.");

    int baseLength = engine->context.pendingTunnel.baseLength;
    int extraRequired = 0;
    engine->context.pendingTunnel.revealed.clear();

    std::shared_ptr<cardsState::PlayerCards> hand = player->getHand();
    if (!hand)
    {
      return buildError(engine, "Tunnel resolve: missing hand");
    }
    int available = cardsState->countWagonCards(hand, selected, true);
    if (available < baseLength)
    {
      std::shared_ptr<GameState> nextState(new ConfirmationState());
      engine->stateMachine->transitionTo(engine, nextState);
      result.nextPhase = Phase::CONFIRMATION;
      addInfo(result, "Tunnel claim failed. Number of cards (" + std::to_string(available)
          + ") inferior to " + std::to_string(baseLength) + ".");
      return result;
    }

    for (int i = 0; i < 3; ++i)
    {
      std::shared_ptr<cardsState::Card> removed = deck->faceDownCards->takeLastCard();
      std::shared_ptr<cardsState::WagonCard> drawn = std::dynamic_pointer_cast<cardsState::WagonCard>(removed);
      if (!drawn)
      {
        return buildError(engine, "Tunnel resolve: invalid card");
      }
      engine->context.pendingTunnel.revealed.push_back(drawn);
      deck->trash->addCard(drawn);

      cardsState::ColorCard drawnColor = drawn->getColor();
      std::string drawMessage = "Drew card " + colorToString(drawnColor) + ".";
      if (drawnColor == selected || drawnColor == cardsState::ColorCard::LOCOMOTIVE)
      {
        ++extraRequired;
        int totalNeeded = baseLength + extraRequired;
        drawMessage += " Tunnel cost increased to " + std::to_string(totalNeeded) + ".";
        addInfo(result, drawMessage);
        if (totalNeeded > available)
        {
          break;
        }
      }
      else
      {
        addInfo(result, drawMessage);
      }
    }

    engine->context.pendingTunnel.extraRequired = extraRequired;
    int totalNeeded = baseLength + extraRequired;
    if (available < totalNeeded)
    {
      std::shared_ptr<GameState> nextState(new ConfirmationState());
      engine->stateMachine->transitionTo(engine, nextState);
      result.nextPhase = Phase::CONFIRMATION;
      addInfo(result, "Tunnel claim failed. Number of cards (" + std::to_string(available)
          + ") inferior to " + std::to_string(totalNeeded) + ".");
      return result;
    }

    if (!cardsState->discardWagonCards(hand, selected, totalNeeded, true))
    {
      return buildError(engine, "Tunnel resolve: failed to discard cards");
    }
    player->removeTrain(baseLength);
    road->setOwner(player);

    std::shared_ptr<GameState> nextState(new ConfirmationState());
    engine->stateMachine->transitionTo(engine, nextState);
    result.nextPhase = Phase::CONFIRMATION;
    addInfo(result, "Tunnel claimed.");
    return result;
  }

  std::vector<EngineCommandType> TunnelResolveState::getAllowedCommands()
  {
    std::vector<EngineCommandType> allowed;
    allowed.push_back(EngineCommandType::CMD_TUNNEL_COLOR);
    allowed.push_back(EngineCommandType::CMD_EXIT);
    return allowed;
  }
}
