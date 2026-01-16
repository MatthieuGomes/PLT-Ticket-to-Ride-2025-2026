#include "Engine.h"

#include "CommandParser.h"

namespace engine
{
  namespace
  {
    const int kNoFinalRoundStarter = -1;
    const int kDefaultPlayer = 0;
    const int kDefaultDrawsRemaining = 0;
    const int kDefaultMinKeepTickets = 0;
  }

  Engine::Engine()
  {
    this->state = std::shared_ptr<state::State>(new state::State());
    this->phase = Phase::SETUP;
    this->stateMachine = std::shared_ptr<StateMachine>(new StateMachine());

    this->context.currentPlayer = kDefaultPlayer;
    this->context.drawsRemaining = kDefaultDrawsRemaining;
    this->context.finalRound = false;
    this->context.finalRoundStarter = kNoFinalRoundStarter;
    this->context.minKeepTickets = kDefaultMinKeepTickets;
    this->context.pendingTickets.offered.clear();
    this->context.pendingTunnel.route.reset();
    this->context.pendingTunnel.color = mapState::UNKNOWN;
    this->context.pendingTunnel.baseLength = 0;
    this->context.pendingTunnel.extraRequired = 0;
    this->context.pendingTunnel.revealed.clear();
  }

  Engine::Engine(std::shared_ptr<state::State> state)
  {
    if (state)
    {
      this->state = state;
    }
    else
    {
      this->state = std::shared_ptr<state::State>(new state::State());
    }

    this->phase = Phase::SETUP;
    this->stateMachine = std::shared_ptr<StateMachine>(new StateMachine());

    this->context.currentPlayer = kDefaultPlayer;
    this->context.drawsRemaining = kDefaultDrawsRemaining;
    this->context.finalRound = false;
    this->context.finalRoundStarter = kNoFinalRoundStarter;
    this->context.minKeepTickets = kDefaultMinKeepTickets;
    this->context.pendingTickets.offered.clear();
    this->context.pendingTunnel.route.reset();
    this->context.pendingTunnel.color = mapState::UNKNOWN;
    this->context.pendingTunnel.baseLength = 0;
    this->context.pendingTunnel.extraRequired = 0;
    this->context.pendingTunnel.revealed.clear();
  }

  EngineResult Engine::applyCommand(const std::string& json)
  {
    CommandParser parser;
    // Non-owning shared_ptr wrapper for passing this engine into the state machine.
    std::shared_ptr<Engine> engineRef(this, [](Engine*) {});
    return parser.parseAndApply(engineRef, json);
  }

  void Engine::setState(std::shared_ptr<state::State> state)
  {
    this->state = state;
  }

  std::shared_ptr<state::State> Engine::getState()
  {
    return this->state;
  }

  Phase Engine::getPhase()
  {
    return this->phase;
  }

  TurnContext Engine::getContext()
  {
    return this->context;
  }
}
