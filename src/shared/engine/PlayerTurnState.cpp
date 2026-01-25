#include "PlayerTurnState.h"

#include "DrawDestinationCardState.h"
#include "DrawWagonCardState.h"
#include "Engine.h"
#include "EngineCommand.h"
#include "EngineEvent.h"
#include "ClaimRoadState.h"
#include "ClaimStationState.h"
#include "BorrowRoadState.h"
#include "StateMachine.h"
#include "AIController.h"
#include "HumanController.h"
#include "PlayerController.h"
#include "parser/JSONParser.h"

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
  }

  void PlayerTurnState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::PLAYER_TURN;

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

    // If not a human controller, auto-play one command.
    if (std::dynamic_pointer_cast<HumanController>(controller))
    {
      return;
    }

    parser::CommandMessage command = controller->nextCommand(engine->getState(), engine);
    parser::JSONParser jsonParser;
    std::string json = jsonParser.serializeCommand(command);
    engine->applyCommand(json);
  }

  std::vector<EngineCommandType> PlayerTurnState::getAllowedCommands()
  {
    std::vector<EngineCommandType> commands;
    commands.push_back(EngineCommandType::CMD_EXIT);
    commands.push_back(EngineCommandType::CMD_DRAW_WAGON_FACEUP);
    commands.push_back(EngineCommandType::CMD_DRAW_WAGON_FACEDOWN);
    commands.push_back(EngineCommandType::CMD_DRAW_DESTINATION);
    commands.push_back(EngineCommandType::CMD_CLAIM_STATION);
    commands.push_back(EngineCommandType::CMD_BORROW_ROAD);
    commands.push_back(EngineCommandType::CMD_TAKE_ROAD);
    return commands;
  }

  EngineResult PlayerTurnState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Player turn: engine not initialized");
    }

    if (command.type == EngineCommandType::CMD_DRAW_WAGON_FACEUP ||
        command.type == EngineCommandType::CMD_DRAW_WAGON_FACEDOWN)
    {
      std::shared_ptr<GameState> nextState(new DrawWagonCardState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }

    if (command.type == EngineCommandType::CMD_TAKE_ROAD)
    {
      std::shared_ptr<GameState> nextState(new ClaimRoadState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }
    if (command.type == EngineCommandType::CMD_DRAW_DESTINATION)
    {
      std::shared_ptr<GameState> nextState(new DrawDestinationCardState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }
    if (command.type == EngineCommandType::CMD_CLAIM_STATION)
    {
      std::shared_ptr<GameState> nextState(new ClaimStationState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }
    if (command.type == EngineCommandType::CMD_BORROW_ROAD)
    {
      std::shared_ptr<GameState> nextState(new BorrowRoadState());
      engine->stateMachine->transitionTo(engine, nextState);
      return engine->stateMachine->handleCommand(engine, command);
    }

    return buildError(engine, "Player turn: command not allowed");
  }
}
