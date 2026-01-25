#include "EndGameState.h"

#include "Engine.h"
#include "EngineEvent.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::END_GAME;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }
  }

  void EndGameState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::END_GAME;
    EngineEvent event;
    event.type = EngineEventType::INFO;
    event.message = "Game over.";
    event.payload = "";
    engine->pendingEvents.push_back(event);
  }

  EngineResult EndGameState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)command;
    return buildError(engine, "End game: command not allowed");
  }

  std::vector<EngineCommandType> EndGameState::getAllowedCommands()
  {
    std::vector<EngineCommandType> commands;
    commands.push_back(EngineCommandType::CMD_EXIT);
    return commands;
  }
}
