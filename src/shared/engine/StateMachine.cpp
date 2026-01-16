#include "StateMachine.h"

#include "EngineCommand.h"
#include "EngineEvent.h"
#include "Phase.h"

namespace engine
{
  StateMachine::StateMachine(){
  }

  void StateMachine::transitionTo(std::shared_ptr<Engine> engine, std::shared_ptr<GameState> gamestate)
  {
    if (this->currentState)
    {
      this->currentState->onExit(engine);
    }
    this->currentState = gamestate;
    if (this->currentState)
    {
      this->currentState->onEnter(engine);
    }
  }

  void StateMachine::setState(std::shared_ptr<GameState> state)
  {
    transitionTo(engine::Engine::getInstance(), state);
  }

  std::shared_ptr<GameState> StateMachine::getState()
  {
    return this->currentState;
  }

  EngineResult StateMachine::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!this->currentState)
    {
      EngineResult result;
      result.ok = false;
      result.error = "No active state";
      result.nextPhase = Phase::SETUP;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = result.error;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }

    return this->currentState->handleCommand(engine, command);
  }
}
