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
    if (command.name.empty())
    {
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

    parser::JSONParser jsonParser;
    std::string json = jsonParser.serializeCommand(command);
    std::vector<EngineEvent> priorEvents = engine->pendingEvents;
    engine->pendingEvents.clear();
    EngineResult result = engine->applyCommand(json);

    EngineEvent event;
    event.type = EngineEventType::INFO;
    event.message = "AI " + playerName + ": " + command.name;
    event.payload = "";
    result.events.insert(result.events.begin(), event);
    if (!priorEvents.empty())
    {
      result.events.insert(result.events.begin(), priorEvents.begin(), priorEvents.end());
    }

    if (!result.events.empty())
    {
      engine->pendingEvents.insert(engine->pendingEvents.end(),
                                   result.events.begin(),
                                   result.events.end());
    }
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
