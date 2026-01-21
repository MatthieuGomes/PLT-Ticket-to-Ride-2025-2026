#include "FaceDownState.h"

#include "EngineCommand.h"
#include "ConfirmationState.h"
#include "DrawWagonCardState.h"
#include "Engine.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "cardsState/CardsState.h"
#include "cardsState/SharedDeck.h"
#include "cardsState/WagonCard.h"
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

    std::string colorLabel(cardsState::ColorCard color)
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
  }

  void FaceDownState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::DRAW_WAGON_FACEDOWN;
  }

  EngineResult FaceDownState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)command;
    if (!engine)
    {
      return buildError(engine, "Face-down draw: no engine");
    }
    if (engine->context.drawSource == 1)
    {
      return buildError(engine, "Draw blocked: can't draw from face-up and face-down piles at the same turn.");
    }
    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      return buildError(engine, "Face-down draw: no state");
    }
    std::shared_ptr<cardsState::SharedDeck<cardsState::WagonCard>> deck = state->cards.gameWagonCards;
    if (!deck || !deck->faceDownCards)
    {
      return buildError(engine, "Face-down draw: wagon deck not ready");
    }

    int playerIndex = engine->context.currentPlayer;
    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    if (players.empty() || playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
      return buildError(engine, "Face-down draw: invalid active player");
    }
    std::shared_ptr<cardsState::PlayerCards> hand = players[playerIndex]->getHand();
    if (!hand)
    {
      return buildError(engine, "Face-down draw: player hand missing");
    }

    if (deck->faceDownCards->cards.size() < 2)
    {
      return buildError(engine, "Face-down draw: not enough cards");
    }
    if (!hand->wagonCards)
    {
      return buildError(engine, "Face-down draw: hand missing wagon deck");
    }
    EngineResult result;
    result.ok = true;
    result.error = "";
    result.nextPhase = Phase::CONFIRMATION;

    for (int i = 0; i < 2; ++i)
    {
      std::shared_ptr<cardsState::Card> removed = deck->faceDownCards->takeLastCard();
      std::shared_ptr<cardsState::WagonCard> drawnCard =
          std::dynamic_pointer_cast<cardsState::WagonCard>(removed);
      if (!drawnCard)
      {
        return buildError(engine, "Face-down draw: invalid card");
      }
      hand->wagonCards->addCard(drawnCard);
      EngineEvent event;
      event.type = EngineEventType::INFO;
      std::ostringstream msg;
      msg << "Wagon card " << colorLabel(drawnCard->getColor()) << " drawn";
      event.message = msg.str();
      event.payload = "";
      result.events.push_back(event);
    }

    engine->context.drawsRemaining = 0;
    engine->context.drawSource = 2;

    std::shared_ptr<GameState> nextState(new ConfirmationState());
    if (engine->stateMachine)
    {
      engine->stateMachine->transitionTo(engine, nextState);
    }
    EngineEvent confirm;
    confirm.type = EngineEventType::INFO;
    confirm.message = "Confirmation state: end turn or borrow roads allowed.";
    confirm.payload = "";
    result.events.push_back(confirm);
    return result;
  }
}
