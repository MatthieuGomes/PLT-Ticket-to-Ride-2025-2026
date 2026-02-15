#include "ConfirmationState.h"

#include "EngineCommand.h"
#include "BorrowRoadState.h"
#include "EndTurnState.h"
#include "Engine.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "AIController.h"
#include "HumanController.h"
#include "PlayerController.h"
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
    if (!engine->stateMachine || engine->context.controllers.empty())
    {
      return;
    }

    int playerIndex = engine->context.currentPlayer;
    if (playerIndex < 0 || playerIndex >= static_cast<int>(engine->context.controllers.size()))
    {
      return;
    }

    std::shared_ptr<PlayerController> controller = engine->context.controllers[static_cast<std::size_t>(playerIndex)];
    if (!controller)
    {
      return;
    }

    bool isHuman = (std::dynamic_pointer_cast<HumanController>(controller) != nullptr);
    if (isHuman)
    {
      EngineEvent event;
      event.type = EngineEventType::INFO;
      event.message = "Confirmation state: end turn or borrow roads allowed.";
      event.payload = "";
      engine->pendingEvents.push_back(event);
      return;
    }

    std::string playerName = "AI";
    std::shared_ptr<state::State> state = engine->getState();
    if (state)
    {
      std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
      if (playerIndex >= 0 && playerIndex < static_cast<int>(players.size()) && players[playerIndex])
      {
        playerName = players[playerIndex]->getName();
      }
    }

    std::vector<EngineEvent> priorEvents = engine->pendingEvents;
    engine->pendingEvents.clear();
    EngineCommand autoCommand;
    autoCommand.name = "confirm";
    autoCommand.type = EngineCommandType::CMD_CONFIRM_ENDTURN;
    autoCommand.payload = "";
    EngineResult result = engine->stateMachine->handleCommand(engine, autoCommand);

    EngineEvent logEvent;
    logEvent.type = EngineEventType::INFO;
    logEvent.message = "AI " + playerName + ": confirm";
    logEvent.payload = "";
    result.events.insert(result.events.begin(), logEvent);
    if (!priorEvents.empty())
    {
      result.events.insert(result.events.begin(), priorEvents.begin(), priorEvents.end());
    }

    std::shared_ptr<GameState> current = engine->stateMachine->getState();
    if (current && current.get() == this)
    {
      std::shared_ptr<GameState> nextState(new EndTurnState());
      engine->stateMachine->transitionTo(engine, nextState);
    }

    if (!result.events.empty())
    {
      engine->pendingEvents.insert(engine->pendingEvents.end(),
                                   result.events.begin(),
                                   result.events.end());
    }
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
