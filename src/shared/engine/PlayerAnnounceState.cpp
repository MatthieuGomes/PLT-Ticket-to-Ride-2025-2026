#include "PlayerAnnounceState.h"

#include <iostream>

#include "Engine.h"
#include "PlayerTurnState.h"
#include "StateMachine.h"
#include "playersState/Player.h"

namespace engine
{
  namespace
  {
    const int kDefaultPlayerIndex = 0;
  }

  void PlayerAnnounceState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }

    engine->phase = Phase::PLAYER_ANNOUNCE;

    std::shared_ptr<state::State> state = engine->getState();
    std::string playerName = "Player";
    int index = engine->context.currentPlayer;

    if (state)
    {
      std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
      if (players.empty())
      {
        playerName = "Player";
        engine->context.currentPlayer = kDefaultPlayerIndex;
      }
      else
      {
        // Early final-round trigger for testing: if any player is at or below 2 wagons,
        // mark final round before announcing the next player.
        if (!engine->context.finalRound)
        {
          for (std::size_t i = 0; i < players.size(); ++i)
          {
            if (players[i] && players[i]->getNbWagons() <= 2)
            {
              engine->context.finalRound = true;
              engine->context.finalRoundStarter = (index < 0) ? kDefaultPlayerIndex : index;
              break;
            }
          }
        }

        if (index < 0 || index >= static_cast<int>(players.size()))
        {
          index = kDefaultPlayerIndex;
          engine->context.currentPlayer = index;
        }
        if (players[index])
        {
          playerName = players[index]->getName();
        }
      }
    }

    std::cout << "It's " << playerName << "'s turn!" << std::endl;

    if (engine->stateMachine)
    {
      std::shared_ptr<GameState> nextState(new PlayerTurnState());
      engine->stateMachine->transitionTo(engine, nextState);
    }
  }
}
