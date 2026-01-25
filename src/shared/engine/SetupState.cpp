#include "SetupState.h"

#include "Engine.h"
#include "AIController.h"
#include "HumanController.h"
#include "PlayerAnnounceState.h"
#include "StateMachine.h"
#include "playersState/PlayersState.h"

namespace engine
{
  namespace
  {
    const int kInitialPlayerIndex = 0;
  }

  void SetupState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }

    if (!engine->getState())
    {
      std::shared_ptr<state::State> newState(new state::State());
      engine->setState(newState);
    }
    engine->phase = Phase::SETUP;
    engine->context.currentPlayer = kInitialPlayerIndex;

    engine->context.controllers.clear();
    if (engine->getState())
    {
      std::vector<std::shared_ptr<playersState::Player>> players = engine->getState()->players.getPlayers();
      for (std::size_t i = 0; i < players.size(); ++i)
      {
        if (i == 0)
        {
          engine->context.controllers.push_back(std::shared_ptr<PlayerController>(new HumanController()));
        }
        else
        {
          std::shared_ptr<AIController> ai(new AIController());
          ai->seed = static_cast<int>(i + 1);
          engine->context.controllers.push_back(ai);
        }
      }
    }

    if (engine->stateMachine)
    {
      std::shared_ptr<GameState> nextState(new PlayerAnnounceState());
      engine->stateMachine->transitionTo(engine, nextState);
    }
  }
}
