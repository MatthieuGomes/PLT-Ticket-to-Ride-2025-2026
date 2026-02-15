#include "StateMachine.h"

#include "EngineCommand.h"
#include "EngineEvent.h"
#include "Phase.h"

#include <iostream>
#include <sstream>

#define DEBUG_MODE true
#if DEBUG_MODE == true
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace engine
{
  namespace
  {
    const char* phaseLabel(Phase phase)
    {
      switch (phase)
      {
        case Phase::INTRO:
          return "INTRO";
        case Phase::PRINT_INSTRUCTIONS:
          return "PRINT_INSTRUCTIONS";
        case Phase::SETUP:
          return "SETUP";
        case Phase::PLAYER_ANNOUNCE:
          return "PLAYER_ANNOUNCE";
        case Phase::WAIT_TURN:
          return "WAIT_TURN";
        case Phase::PLAYER_TURN:
          return "PLAYER_TURN";
        case Phase::DRAW_DESTINATION:
          return "DRAW_DESTINATION";
        case Phase::DESTINATION_CHOICE:
          return "DESTINATION_CHOICE";
        case Phase::DRAW_WAGON:
          return "DRAW_WAGON";
        case Phase::DRAW_WAGON_FACEUP:
          return "DRAW_WAGON_FACEUP";
        case Phase::DRAW_WAGON_FACEDOWN:
          return "DRAW_WAGON_FACEDOWN";
        case Phase::CLAIM_ROAD:
          return "CLAIM_ROAD";
        case Phase::ROAD_RESOLVE:
          return "ROAD_RESOLVE";
        case Phase::TUNNEL_RESOLVE:
          return "TUNNEL_RESOLVE";
        case Phase::CLAIM_STATION:
          return "CLAIM_STATION";
        case Phase::BORROW_ROAD:
          return "BORROW_ROAD";
        case Phase::CONFIRMATION:
          return "CONFIRMATION";
        case Phase::END_TURN:
          return "END_TURN";
        case Phase::END_GAME:
          return "END_GAME";
        case Phase::EXIT:
          return "EXIT";
      }
      return "UNKNOWN";
    }
  }

  StateMachine::StateMachine(){
  }

  void StateMachine::transitionTo(std::shared_ptr<Engine> engine, std::shared_ptr<GameState> gamestate)
  {
    if (this->currentState)
    {
      if (engine)
      {
        EngineEvent event;
        event.type = EngineEventType::INFO;
        event.message = std::string("DEBUG: Exit ") + phaseLabel(engine->phase);
        event.payload = "";
        engine->pendingEvents.push_back(event);
        DEBUG_PRINT(event.message);
      }
      this->currentState->onExit(engine);
    }
    this->currentState = gamestate;
    if (this->currentState)
    {
      this->currentState->onEnter(engine);
      if (engine)
      {
        EngineEvent event;
        event.type = EngineEventType::INFO;
        event.message = std::string("DEBUG: Enter ") + phaseLabel(engine->phase);
        event.payload = "";
        engine->pendingEvents.push_back(event);
        DEBUG_PRINT(event.message);
      }
    }
  }

  void StateMachine::setState(std::shared_ptr<GameState> state)
  {
    std::shared_ptr<Engine> engine;
    transitionTo(engine, state);
  }

  std::shared_ptr<GameState> StateMachine::getState()
  {
    return this->currentState;
  }

  EngineResult StateMachine::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!this->currentState)
    {
      EngineResult result;
      result.ok = false;
      result.error = "No active state";
      result.nextPhase = Phase::SETUP;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = result.error;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }

    return this->currentState->handleCommand(engine, command);
  }
}
