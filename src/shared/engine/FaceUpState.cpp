#include "FaceUpState.h"

#include <sstream>

#include <json/json.h>

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

    bool parsePayload(const std::string& payload, Json::Value& root, std::string& error)
    {
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

    int getIndexFromPayload(const Json::Value& root)
    {
      if (root.isMember("index"))
      {
        return root["index"].asInt();
      }
      if (root.isMember("slot"))
      {
        return root["slot"].asInt();
      }
      return -1;
    }
  }

  void FaceUpState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::DRAW_WAGON_FACEUP;
  }

  EngineResult FaceUpState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine)
    {
      return buildError(engine, "Face-up draw: no engine");
    }
    if (engine->context.drawSource == 2)
    {
      return buildError(engine, "Draw blocked: can't draw from face-up and face-down piles at the same turn.");
    }
    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      return buildError(engine, "Face-up draw: no state");
    }
    std::shared_ptr<cardsState::SharedDeck<cardsState::WagonCard>> deck = state->cards.gameWagonCards;
    if (!deck || !deck->faceUpCards)
    {
      return buildError(engine, "Face-up draw: wagon deck not ready");
    }

    Json::Value root;
    std::string error;
    if (!parsePayload(command.payload, root, error))
    {
      return buildError(engine, error);
    }
    int index = getIndexFromPayload(root);
    if (index <= 0)
    {
      return buildError(engine, "Face-up draw: invalid index");
    }

    int pos = index - 1;
    if (pos < 0 || pos >= static_cast<int>(deck->faceUpCards->cards.size()))
    {
      return buildError(engine, "Face-up draw: index out of range");
    }

    int playerIndex = engine->context.currentPlayer;
    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    if (players.empty() || playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
      return buildError(engine, "Face-up draw: invalid active player");
    }
    std::shared_ptr<cardsState::PlayerCards> hand = players[playerIndex]->getHand();
    if (!hand)
    {
      return buildError(engine, "Face-up draw: player hand missing");
    }

    std::shared_ptr<cardsState::WagonCard> selected = deck->faceUpCards->cards[pos];
    if (!selected)
    {
      return buildError(engine, "Face-up draw: selected card missing");
    }
    bool isLocomotive = (selected->getColor() == cardsState::ColorCard::LOCOMOTIVE);
    bool firstDraw = (engine->context.drawsRemaining >= 2);
    if (engine->context.drawSource == 1 && !firstDraw && isLocomotive)
    {
      return buildError(engine, "Draw blocked: can't draw a locomotive + another card from faceup pile.");
    }

    deck->drawCard(hand, selected, 0);
    deck->turnCardUp();

    if (engine->context.drawSource == 0)
    {
      engine->context.drawSource = 1;
    }
    if (isLocomotive && firstDraw)
    {
      engine->context.drawsRemaining = 0;
    }
    else
    {
      engine->context.drawsRemaining -= 1;
    }

    std::ostringstream msg;
    msg << "Wagon card " << colorLabel(selected->getColor()) << " drawn";

    if (engine->context.drawsRemaining <= 0)
    {
      std::shared_ptr<GameState> nextState(new ConfirmationState());
      if (engine->stateMachine)
      {
        engine->stateMachine->transitionTo(engine, nextState);
      }
      EngineResult result = buildInfo(Phase::CONFIRMATION, msg.str());
      if (isLocomotive && firstDraw)
      {
        EngineEvent extra;
        extra.type = EngineEventType::INFO;
        extra.message = "Draw phase ended: face-up locomotive obtained.";
        extra.payload = "";
        result.events.push_back(extra);
      }
      return result;
    }

    std::shared_ptr<GameState> nextState(new DrawWagonCardState());
    if (engine->stateMachine)
    {
      engine->stateMachine->transitionTo(engine, nextState);
    }
    return buildInfo(Phase::DRAW_WAGON, msg.str());
  }
}
