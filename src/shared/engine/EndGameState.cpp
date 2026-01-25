#include "EndGameState.h"

#include "Engine.h"
#include "EngineEvent.h"
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

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = "End game: missing state.";
      event.payload = "";
      engine->pendingEvents.push_back(event);
      return;
    }

    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    if (players.empty())
    {
      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = "End game: no players found.";
      event.payload = "";
      engine->pendingEvents.push_back(event);
      return;
    }

    int bestScore = -1000000;
    std::string bestName = "";

    for (const std::shared_ptr<playersState::Player>& player : players)
    {
      if (!player)
      {
        continue;
      }
      int destPoints = 0;
      for (const std::shared_ptr<cardsState::DestinationCard>& dest : player->completedDestinations)
      {
        if (dest)
        {
          destPoints += dest->getPoints();
        }
      }
      int wagonPoints = player->calculateWagonPoints();
      int total = player->getScore() + destPoints + wagonPoints;

      EngineEvent scoreEvent;
      scoreEvent.type = EngineEventType::INFO;
      scoreEvent.message = player->getName() + " final score: " + std::to_string(total);
      scoreEvent.payload = "";
      engine->pendingEvents.push_back(scoreEvent);

      if (total > bestScore)
      {
        bestScore = total;
        bestName = player->getName();
      }
    }

    EngineEvent event;
    event.type = EngineEventType::INFO;
    event.message = "Game over. Winner: " + bestName + ".";
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
