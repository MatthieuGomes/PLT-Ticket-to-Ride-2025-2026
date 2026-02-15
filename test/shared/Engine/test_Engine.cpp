#include <boost/test/unit_test.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../../src/shared/engine/CommandParser.h"
#include "../../src/shared/engine/Engine.h"
#include "../../src/shared/engine/EngineResult.h"
#include "../../src/shared/engine/Phase.h"
#include "../../src/shared/engine/EndTurnState.h"
#include "../../src/shared/engine/ConfirmationState.h"
#include "../../src/shared/engine/PlayerTurnState.h"
#include "../../src/shared/engine/PlayerAnnounceState.h"
#include "../../src/shared/engine/AIController.h"
#include "../../src/shared/engine/HumanController.h"
#include "../../src/shared/playersState/Player.h"
#include "../../src/shared/cardsState/PlayerCards.h"
#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/state/State.h"

#include <json/json.h>

#define DEBUG_MODE true
#if DEBUG_MODE == true
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

/*
// Engine JSON command handling tests.
// Disabled on this branch: requires parser::JSONParser implementation.
BOOST_AUTO_TEST_SUITE(EngineCommandParserTests)

// Verifies that a JSON command becomes an EngineCommand with payload preserved.
BOOST_AUTO_TEST_CASE(ParsesCommandToEngineCommand)
{
  engine::CommandParser parser;
  std::string json = "{\"kind\":\"command\",\"origin\":\"tui\",\"name\":\"draw\","
                     "\"payload\":{\"count\":2}}";
  engine::EngineCommand cmd = parser.parse(json);
  DEBUG_PRINT("Parsed command name: " << cmd.name);
  DEBUG_PRINT("Parsed command payload: " << cmd.payload);
  BOOST_CHECK_EQUAL(cmd.name, "draw");
  BOOST_CHECK(cmd.payload.find("count") != std::string::npos);
}

// Ensures invalid JSON returns an error result without crashing.
BOOST_AUTO_TEST_CASE(ParseAndApplyRejectsInvalidJson)
{
  engine::CommandParser parser;
  std::shared_ptr<engine::Engine> enginePtr =
      std::make_shared<engine::Engine>();
  engine::EngineResult result = parser.parseAndApply(enginePtr, "{bad json");
  DEBUG_PRINT("Invalid JSON result ok: " << result.ok << " error: " << result.error);
  BOOST_CHECK(!result.ok);
  BOOST_CHECK(!result.error.empty());
}

// Ensures encodeResult emits JSON with result fields and events.
BOOST_AUTO_TEST_CASE(EncodesEngineResult)
{
  engine::CommandParser parser;
  engine::EngineResult result;
  result.ok = true;
  result.error = "";
  result.nextPhase = engine::Phase::PLAYER_TURN;
  std::string json = parser.encodeResult(result);
  DEBUG_PRINT("Encoded result JSON: " << json);
  BOOST_CHECK(json.find("\"kind\"") != std::string::npos);
  BOOST_CHECK(json.find("\"origin\"") != std::string::npos);
  BOOST_CHECK(json.find("\"nextPhase\"") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
*/

// Placeholder to keep the test target valid while JSON parsing is wired in.
BOOST_AUTO_TEST_CASE(EngineJsonTestsDisabled)
{
  BOOST_CHECK(true);
}

namespace
{
  std::shared_ptr<engine::Engine> buildEngineWithPlayers(int count)
  {
    std::shared_ptr<state::State> state(new state::State());
    std::vector<std::shared_ptr<playersState::Player>> players;
    for (int i = 0; i < count; ++i)
    {
      std::ostringstream name;
      name << "Player" << (i + 1);
      std::shared_ptr<cardsState::PlayerCards> hand(new cardsState::PlayerCards());
      std::vector<std::shared_ptr<mapState::Road>> borrowed;
      std::shared_ptr<playersState::Player> player(
          new playersState::Player(name.str(),
                                   playersState::PlayerColor::RED,
                                   0,
                                   45,
                                   3,
                                   borrowed,
                                   hand));
      players.push_back(player);
    }
    state->players.players = players;

    std::shared_ptr<engine::Engine> engine(new engine::Engine(state));
    engine->context.controllers.clear();
    engine->context.currentPlayer = 0;
    engine->context.finalRound = false;
    engine->context.finalRoundStarter = -1;
    engine->pendingEvents.clear();
    for (int i = 0; i < count; ++i)
    {
      if (i == 0)
      {
        engine->context.controllers.push_back(std::shared_ptr<engine::PlayerController>(new engine::HumanController()));
      }
      else
      {
        std::shared_ptr<engine::AIController> ai(new engine::AIController());
        ai->seed = i + 1;
        engine->context.controllers.push_back(ai);
      }
    }
    return engine;
  }

  bool hasTurnAnnouncement(const std::vector<engine::EngineEvent>& events, const std::string& playerName)
  {
    for (std::size_t i = 0; i < events.size(); ++i)
    {
      if (events[i].message.find("It's ") != std::string::npos &&
          events[i].message.find(playerName) != std::string::npos)
      {
        return true;
      }
    }
    return false;
  }
}

BOOST_AUTO_TEST_CASE(EndTurnTransitionsToNextPlayer)
{
  std::shared_ptr<engine::Engine> engine = buildEngineWithPlayers(2);
  engine->context.currentPlayer = 0;
  engine->pendingEvents.clear();

  std::shared_ptr<engine::GameState> endState(new engine::EndTurnState());
  engine->stateMachine->transitionTo(engine, endState);

  std::shared_ptr<engine::GameState> current = engine->stateMachine->getState();
  BOOST_CHECK(current != nullptr);
  BOOST_CHECK(std::dynamic_pointer_cast<engine::PlayerTurnState>(current) != nullptr);
  BOOST_CHECK_EQUAL(engine->context.currentPlayer, 1);
  BOOST_CHECK(hasTurnAnnouncement(engine->pendingEvents, "Player2"));
}

BOOST_AUTO_TEST_CASE(ConfirmationAutoAdvancesForAI)
{
  std::shared_ptr<engine::Engine> engine = buildEngineWithPlayers(2);
  engine->context.currentPlayer = 1;
  engine->pendingEvents.clear();

  std::shared_ptr<engine::GameState> confirmState(new engine::ConfirmationState());
  engine->stateMachine->transitionTo(engine, confirmState);

  std::shared_ptr<engine::GameState> current = engine->stateMachine->getState();
  BOOST_CHECK(current != nullptr);
  BOOST_CHECK(std::dynamic_pointer_cast<engine::ConfirmationState>(current) == nullptr);
  BOOST_CHECK_EQUAL(engine->context.currentPlayer, 0);
  BOOST_CHECK(hasTurnAnnouncement(engine->pendingEvents, "Player1"));
}

/* vim: set sw=2 sts=2 et : */
