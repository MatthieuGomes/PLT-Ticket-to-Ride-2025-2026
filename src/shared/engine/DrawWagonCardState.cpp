#include "DrawWagonCardState.h"

#include "EngineCommand.h"
#include "Engine.h"
#include "EngineEvent.h"
#include "FaceDownState.h"
#include "FaceUpState.h"
#include "StateMachine.h"
#include "AIController.h"
#include "HumanController.h"
#include "PlayerController.h"
#include "parser/JSONParser.h"
#include "playersState/Player.h"

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
      engine->context.drawSource = 0;
    }

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

    if (std::dynamic_pointer_cast<HumanController>(controller))
    {
      return;
    }

    parser::CommandMessage message;
    message.kind = "command";
    message.origin = "tui";
    message.version = 1;

    if (engine->context.drawSource == 2)
    {
      message.name = "draw_facedown";
    }
    else
    {
      message.name = "draw_faceup";
      message.payload = Json::Value(Json::objectValue);
      message.payload["index"] = 1;
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
    EngineEvent event;
    event.type = EngineEventType::INFO;
    event.message = "AI " + playerName + ": " + message.name;
    event.payload = "";
    engine->pendingEvents.push_back(event);

    parser::JSONParser jsonParser;
    std::string json = jsonParser.serializeCommand(message);
    engine->applyCommand(json);
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
