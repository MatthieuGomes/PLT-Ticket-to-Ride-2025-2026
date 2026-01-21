#include "TunnelResolveState.h"

#include <json/json.h>

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
    (void)command;
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Tunnel resolve: engine not initialized");
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

    std::shared_ptr<cardsState::SharedDeck<cardsState::WagonCard>> deck = cardsState->gameWagonCards;
    if (!deck || !deck->faceDownCards || !deck->trash)
    {
      return buildError(engine, "Tunnel resolve: wagon deck not ready");
    }

    if (deck->faceDownCards->cards.size() < 3)
    {
      return buildError(engine, "Tunnel resolve: not enough cards in deck");
    }

    cardsState::ColorCard color = roadColorToCard(engine->context.pendingTunnel.color);
    if (color == cardsState::ColorCard::UNKNOWN)
    {
      return buildError(engine, "Tunnel resolve: missing color selection");
    }

    int extraRequired = 0;
    engine->context.pendingTunnel.revealed.clear();
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

      if (drawn->getColor() == color || drawn->getColor() == cardsState::ColorCard::LOCOMOTIVE)
      {
        ++extraRequired;
      }
    }

    engine->context.pendingTunnel.extraRequired = extraRequired;

    int length = engine->context.pendingTunnel.baseLength;
    int totalNeeded = length + extraRequired;
    std::shared_ptr<cardsState::PlayerCards> hand = player->getHand();
    if (!hand)
    {
      return buildError(engine, "Tunnel resolve: missing hand");
    }

    if (cardsState->countWagonCards(hand, color, true) < totalNeeded)
    {
      std::shared_ptr<GameState> nextState(new ConfirmationState());
      engine->stateMachine->transitionTo(engine, nextState);
      return buildInfo(Phase::CONFIRMATION, "Tunnel failed: not enough cards");
    }

    if (!cardsState->discardWagonCards(hand, color, totalNeeded, true))
    {
      return buildError(engine, "Tunnel resolve: failed to discard cards");
    }
    player->removeTrain(length);
    road->setOwner(player);

    std::shared_ptr<GameState> nextState(new ConfirmationState());
    engine->stateMachine->transitionTo(engine, nextState);
    return buildInfo(Phase::CONFIRMATION, "Tunnel claimed");
  }
}
