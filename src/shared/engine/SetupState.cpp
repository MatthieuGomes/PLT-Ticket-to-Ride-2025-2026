#include "SetupState.h"

#include "Engine.h"
#include "PlayerAnnounceState.h"
#include "StateMachine.h"

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

    std::shared_ptr<state::State> newState(new state::State());
    engine->setState(newState);
    engine->phase = Phase::SETUP;
    engine->context.currentPlayer = kInitialPlayerIndex;

    if (engine->stateMachine)
    {
      std::shared_ptr<GameState> nextState(new PlayerAnnounceState());
      engine->stateMachine->transitionTo(engine, nextState);
    }
  }
}
