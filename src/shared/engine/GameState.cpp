#include "GameState.h"

#include "EngineCommand.h"
#include "EngineEvent.h"
#include "Phase.h"

namespace engine
{
  void GameState::onEnter(std::shared_ptr<Engine> engine)
  {
    (void)engine;
  }

  void GameState::onExit(std::shared_ptr<Engine> engine)
  {
    (void)engine;
  }

  EngineResult GameState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)engine;
    (void)command;

    EngineResult result;
    result.ok = false;
    result.error = "Unhandled command";
    result.nextPhase = Phase::SETUP;

    EngineEvent event;
    event.type = EngineEventType::ERROR;
    event.message = result.error;
    event.payload = "";
    result.events.push_back(event);

    return result;
  }
}
