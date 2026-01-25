#include "EndTurnState.h"

#include "Engine.h"
#include "EngineEvent.h"
#include "EndGameState.h"
#include "PlayerAnnounceState.h"
#include "StateMachine.h"
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
      result.nextPhase = engine ? engine->phase : Phase::END_TURN;

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

  void EndTurnState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (engine)
    {
      engine->phase = Phase::END_TURN;
    }

    if (!engine || !engine->stateMachine)
    {
      return;
    }

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      pushInfo(engine, "End turn: missing state");
      return;
    }

    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    if (players.empty())
    {
      pushInfo(engine, "End turn: no players");
      return;
    }

    int current = engine->context.currentPlayer;
    if (current < 0 || current >= static_cast<int>(players.size()))
    {
      current = 0;
    }

    std::shared_ptr<playersState::Player> currentPlayer = players[current];
    if (!engine->context.finalRound && currentPlayer && currentPlayer->getNbWagons() <= 2)
    {
      engine->context.finalRound = true;
      engine->context.finalRoundStarter = current;
      pushInfo(engine, "Final round triggered.");
    }

    int next = (current + 1) % static_cast<int>(players.size());
    engine->context.currentPlayer = next;

    if (engine->context.finalRound && next == engine->context.finalRoundStarter)
    {
      std::shared_ptr<GameState> nextState(new EndGameState());
      engine->stateMachine->transitionTo(engine, nextState);
      return;
    }

    std::shared_ptr<GameState> nextState(new PlayerAnnounceState());
    engine->stateMachine->transitionTo(engine, nextState);
  }

  EngineResult EndTurnState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    (void)command;
    return buildError(engine, "End turn: command not allowed");
  }

  std::vector<EngineCommandType> EndTurnState::getAllowedCommands()
  {
    std::vector<EngineCommandType> commands;
    commands.push_back(EngineCommandType::CMD_EXIT);
    return commands;
  }
}
