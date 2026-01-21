#include "ConfirmationState.h"

#include "EngineCommand.h"
#include "BorrowRoadState.h"
#include "EndTurnState.h"
#include "Engine.h"
#include "EngineEvent.h"
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

  void ConfirmationState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::CONFIRMATION;
    engine->context.drawsRemaining = 0;
    engine->context.drawSource = 0;
    EngineEvent event;
    event.type = EngineEventType::INFO;
    event.message = "Confirmation state: end turn or borrow roads allowed.";
    event.payload = "";
    engine->pendingEvents.push_back(event);
  }

  std::vector<EngineCommandType> ConfirmationState::getAllowedCommands()
  {
    std::vector<EngineCommandType> commands;
    commands.push_back(EngineCommandType::CMD_CONFIRM_ENDTURN);
    commands.push_back(EngineCommandType::CMD_BORROW_ROAD);
    return commands;
  }

  EngineResult ConfirmationState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Confirmation: engine not initialized");
    }

    if (command.type == EngineCommandType::CMD_CONFIRM_ENDTURN)
    {
      std::shared_ptr<GameState> nextState(new EndTurnState());
      engine->stateMachine->transitionTo(engine, nextState);
      return buildInfo(Phase::END_TURN, "Turn confirmed");
    }

    if (command.type == EngineCommandType::CMD_BORROW_ROAD)
    {
      std::shared_ptr<GameState> nextState(new BorrowRoadState());
      engine->stateMachine->transitionTo(engine, nextState);
      return buildInfo(Phase::BORROW_ROAD, "Borrow road");
    }

    return buildError(engine, "Confirmation: command not allowed");
  }
}
