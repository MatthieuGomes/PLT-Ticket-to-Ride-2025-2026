#include "EndTurnState.h"

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
      result.nextPhase = engine ? engine->phase : Phase::END_TURN;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }
  }

  void EndTurnState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (engine)
    {
      engine->phase = Phase::END_TURN;
    }
  }

  EngineResult EndTurnState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)command;
    return buildError(engine, "End turn: command not allowed");
  }
}
