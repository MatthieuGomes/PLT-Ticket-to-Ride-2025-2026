#include "ExitState.h"

#include "Engine.h"
#include "EngineEvent.h"
#include "EngineEventType.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::EXIT;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }

    void pushInfo(std::shared_ptr<Engine> engine, const std::string& message)
    {
      if (!engine)
      {
        return;
      }
      EngineEvent event;
      event.type = EngineEventType::INFO;
      event.message = message;
      event.payload = "";
      engine->pendingEvents.push_back(event);
    }
  }

  void ExitState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }

    engine->phase = Phase::EXIT;
    pushInfo(engine, "Exit requested.");

    // Defer teardown to this state so command parsing stays simple.
    engine->stateMachine.reset();
    engine->state.reset();
  }

  EngineResult ExitState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)command;
    return buildError(engine, "Exit state: command not allowed");
  }

  std::vector<EngineCommandType> ExitState::getAllowedCommands()
  {
    std::vector<EngineCommandType> commands;
    commands.push_back(EngineCommandType::CMD_EXIT);
    return commands;
  }
}
