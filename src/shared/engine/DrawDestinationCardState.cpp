#include "DrawDestinationCardState.h"

#include <sstream>

#include "DestinationChoiceState.h"
#include "Engine.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "cardsState/CardsState.h"
#include "cardsState/DestinationCard.h"
#include "cardsState/SharedDeck.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::DRAW_DESTINATION;

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

    std::string cardToString(const std::shared_ptr<cardsState::DestinationCard>& card)
    {
      if (!card)
      {
        return "?";
      }
      std::shared_ptr<mapState::Station> stationA = card->getstationA();
      std::shared_ptr<mapState::Station> stationB = card->getstationB();
      std::string nameA = stationA ? stationA->getName() : "?";
      std::string nameB = stationB ? stationB->getName() : "?";
      return nameA + "-" + nameB + " (" + std::to_string(card->getPoints()) + ")";
    }
  }

  void DrawDestinationCardState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine || !engine->stateMachine)
    {
      return;
    }

    engine->phase = Phase::DRAW_DESTINATION;

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      engine->pendingEvents.clear();
      engine->pendingEvents.push_back(EngineEvent{EngineEventType::ERROR, "Draw destination: missing state", ""});
      return;
    }

    std::shared_ptr<cardsState::CardsState> cardsState(state, &state->cards);
    if (!cardsState || !cardsState->gameDestinationCards || !cardsState->gameDestinationCards->faceDownCards)
    {
      engine->pendingEvents.clear();
      engine->pendingEvents.push_back(EngineEvent{EngineEventType::ERROR, "Draw destination: deck not ready", ""});
      return;
    }

    std::shared_ptr<cardsState::SharedDeck<cardsState::DestinationCard>> deck = cardsState->gameDestinationCards;
    if (deck->faceDownCards->cards.size() < 3)
    {
      engine->pendingEvents.clear();
      engine->pendingEvents.push_back(EngineEvent{EngineEventType::ERROR, "Draw destination: not enough cards", ""});
      return;
    }

    engine->context.pendingTickets.offered.clear();

    for (int i = 0; i < 3; ++i)
    {
      std::shared_ptr<cardsState::Card> removed = deck->faceDownCards->takeLastCard();
      std::shared_ptr<cardsState::DestinationCard> drawn = std::dynamic_pointer_cast<cardsState::DestinationCard>(removed);
      if (!drawn)
      {
        engine->pendingEvents.clear();
        engine->pendingEvents.push_back(EngineEvent{EngineEventType::ERROR, "Draw destination: invalid card", ""});
        return;
      }
      engine->context.pendingTickets.offered.push_back(drawn);
    }

    std::ostringstream prompt;
    prompt << "Destination choices (keep at least " << engine->context.minKeepTickets << "):";

    EngineResult result;
    result.ok = true;
    result.error = "";
    result.nextPhase = Phase::DESTINATION_CHOICE;
    addInfo(result, prompt.str());

    for (std::size_t i = 0; i < engine->context.pendingTickets.offered.size(); ++i)
    {
      std::string line = std::to_string(i + 1) + ". " + cardToString(engine->context.pendingTickets.offered[i]);
      addInfo(result, line);
    }

    std::shared_ptr<GameState> nextState(new DestinationChoiceState());
    engine->stateMachine->transitionTo(engine, nextState);
    engine->pendingEvents = result.events;
  }

  EngineResult DrawDestinationCardState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)command;
    return buildError(engine, "Draw destination: use select command");
  }
}
