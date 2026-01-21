#include "DrawWagonCardState.h"

#include "EngineCommand.h"
#include "Engine.h"
#include "EngineEvent.h"
#include "FaceDownState.h"
#include "FaceUpState.h"
#include "StateMachine.h"

namespace engine
{
  namespace
  {
    const int kDefaultDrawsRemaining = 2;

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
  }

  void DrawWagonCardState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }

    engine->phase = Phase::DRAW_WAGON;
    if (engine->context.drawsRemaining <= 0)
    {
      engine->context.drawsRemaining = kDefaultDrawsRemaining;
    }
  }

  EngineResult DrawWagonCardState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Draw wagon: engine not initialized");
    }

    if (command.type == EngineCommandType::CMD_DRAW_WAGON_FACEUP)
    {
      std::shared_ptr<GameState> nextState(new FaceUpState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }

    if (command.type == EngineCommandType::CMD_DRAW_WAGON_FACEDOWN)
    {
      std::shared_ptr<GameState> nextState(new FaceDownState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }

    return buildError(engine, "Draw wagon: command not allowed");
  }
}
