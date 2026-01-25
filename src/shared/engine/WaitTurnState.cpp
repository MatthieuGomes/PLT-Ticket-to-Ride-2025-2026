#include "WaitTurnState.h"

#include "Engine.h"
#include "EngineEvent.h"
#include "EngineEventType.h"
#include "PlayerAnnounceState.h"
#include "StateMachine.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::WAIT_TURN;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }
  }

  void WaitTurnState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::WAIT_TURN;
    EngineEvent event;
    event.type = EngineEventType::INFO;
    event.message = "Waiting for other players...";
    event.payload = "";
    engine->pendingEvents.push_back(event);

    if (engine->stateMachine)
    {
      std::shared_ptr<GameState> nextState(new PlayerAnnounceState());
      engine->stateMachine->transitionTo(engine, nextState);
    }
  }

  EngineResult WaitTurnState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)command;
    return buildError(engine, "Wait turn: command not allowed");
  }

  std::vector<EngineCommandType> WaitTurnState::getAllowedCommands()
  {
    std::vector<EngineCommandType> commands;
    commands.push_back(EngineCommandType::CMD_EXIT);
    return commands;
  }
}
