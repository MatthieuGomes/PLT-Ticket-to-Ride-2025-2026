#include <boost/test/unit_test.hpp>

#include "../../src/shared/playersState/PlayersState.h"
#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/mapState/Ferry.h"
#include "../../src/shared/mapState/Tunnel.h"
#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/state/State.h"
#include "../../src/shared/parser/JSONParser.h"

#include <json/json.h>
#include <fstream>
#include <sstream>

#define DEBUG_MODE true
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

#define TEST(x) BOOST_AUTO_TEST_CASE(x)
#define SUITE_START(x) BOOST_AUTO_TEST_SUITE(x)
#define SUITE_END() BOOST_AUTO_TEST_SUITE_END()
#define ANN_START(x) std::cout << "Starting " << x << " test..." << std::endl;
#define ANN_END(x) std::cout << x << " test finished!" << std::endl;
#define CHECK_EQ(a, b) BOOST_CHECK_EQUAL(a, b)
#define CHECK_NE(a, b) BOOST_CHECK_NE(a, b)
#define CHECK_NTHROW(...) BOOST_CHECK_NO_THROW(__VA_ARGS__)
#define CHECK_THROW(...) BOOST_CHECK_THROW(__VA_ARGS__)
#define REQUIRE(...) BOOST_REQUIRE(__VA_ARGS__)
#define CHECK(x) BOOST_CHECK(x)

namespace {

std::string fixturePath(const std::string& filename)
{
  std::string path = __FILE__;
  std::size_t pos = path.find_last_of("/\\");
  if (pos == std::string::npos)
  {
    return filename;
  }
  return path.substr(0, pos) + "/fixtures/" + filename;
}

std::string loadFile(const std::string& path)
{
  std::ifstream file(path.c_str());
  if (!file)
  {
    return std::string();
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

bool parseJson(const std::string& text, Json::Value& root)
{
  Json::CharReaderBuilder builder;
  builder["collectComments"] = false;
  std::string error;
  std::istringstream input(text);
  return Json::parseFromStream(builder, input, &root, &error);
}

std::string jsonSection(const Json::Value& root, const char* key)
{
  if (!root.isObject() || !root.isMember(key))
  {
    return std::string();
  }
  Json::StreamWriterBuilder writer;
  writer["commentStyle"] = "None";
  writer["indentation"] = "";
  return Json::writeString(writer, root[key]);
}

// Serialize a Json::Value for inline test fixtures.
std::string writeJson(const Json::Value& root)
{
  Json::StreamWriterBuilder writer;
  writer["commentStyle"] = "None";
  writer["indentation"] = "";
  return Json::writeString(writer, root);
}

} // namespace

TEST(TestStaticAssert)
{
  CHECK(1);
}

SUITE_START(Parser)

// Verifies full state construction via InitFromJSON/ParseFromJSON/setupFromJSON.
TEST(ParseInitStateFixture)
{
  ANN_START("ParseInitStateFixture")
  std::string fixture = fixturePath("init_state.json");
  std::string jsonContent = loadFile(fixture);
  DEBUG_PRINT("Fixture: " << fixture);
  DEBUG_PRINT("Fixture size: " << jsonContent.size());
  REQUIRE(!jsonContent.empty());

  Json::Value root;
  REQUIRE(parseJson(jsonContent, root));

  std::string jsonPlayers = jsonSection(root, "players");
  std::string jsonMap = jsonSection(root, "map");
  std::string jsonCards = jsonSection(root, "cards");

  DEBUG_PRINT("Players JSON: " << jsonPlayers);
  DEBUG_PRINT("Map JSON: " << jsonMap);
  DEBUG_PRINT("Cards JSON: " << jsonCards);

  REQUIRE(!jsonPlayers.empty());
  REQUIRE(!jsonMap.empty());
  REQUIRE(!jsonCards.empty());

  playersState::PlayersState players = playersState::PlayersState::InitFromJSON(jsonPlayers);
  mapState::MapState map = mapState::MapState::ParseFromJSON(jsonMap, std::make_shared<playersState::PlayersState>(players));
  cardsState::CardsState cards = cardsState::CardsState::ParseFromJSON(jsonCards, std::make_shared<mapState::MapState>(map));
  players.setupFromJSON(jsonPlayers, std::make_shared<mapState::MapState>(map), std::make_shared<cardsState::CardsState>(cards));

  DEBUG_PRINT("Players count: " << players.getPlayers().size());
  DEBUG_PRINT("Stations count: " << map.getStations().size());
  DEBUG_PRINT("Roads count: " << map.getRoads().size());

  CHECK_EQ(players.getPlayers().size(), 2);
  CHECK_EQ(map.getStations().size(), 3);
  CHECK_EQ(map.getRoads().size(), 3);

  std::shared_ptr<playersState::Player> alice = players.getPlayerByName("Alice");
  std::shared_ptr<playersState::Player> bob = players.getPlayerByName("Bob");
  REQUIRE(alice);
  REQUIRE(bob);

  DEBUG_PRINT("Alice borrowed roads: " << alice->getBorrowedRoads().size());
  DEBUG_PRINT("Bob borrowed roads: " << bob->getBorrowedRoads().size());
  DEBUG_PRINT("Alice score: " << alice->getScore());
  DEBUG_PRINT("Bob score: " << bob->getScore());

  CHECK_EQ(alice->getBorrowedRoads().size(), 2);
  CHECK_EQ(bob->getBorrowedRoads().size(), 1);

  CHECK_EQ(map.getRoadsOwnedByPlayer(alice).size(), 2);
  CHECK_EQ(map.getRoadsOwnedByPlayer(bob).size(), 1);

  std::shared_ptr<mapState::Station> alpha = map.getStationByName("alpha");
  REQUIRE(alpha);
  REQUIRE(alpha->getOwner());
  DEBUG_PRINT("Alpha owner: " << alpha->getOwner()->getName());
  CHECK_EQ(alpha->getOwner()->getName(), "Alice");

  REQUIRE(alice->getHand());
  REQUIRE(alice->getHand()->destinationCards);
  REQUIRE(alice->getHand()->wagonCards);
  DEBUG_PRINT("Alice destination cards: " << alice->getHand()->destinationCards->countCards());
  DEBUG_PRINT("Alice wagon cards: " << alice->getHand()->wagonCards->countCards());
  CHECK_EQ(alice->getHand()->destinationCards->countCards(), 1);
  CHECK_EQ(alice->getHand()->wagonCards->countCards(), 2);
  CHECK_EQ(alice->completedDestinations.size(), 1);

  REQUIRE(bob->getHand());
  REQUIRE(bob->getHand()->wagonCards);
  DEBUG_PRINT("Bob wagon cards: " << bob->getHand()->wagonCards->countCards());
  CHECK_EQ(bob->getHand()->wagonCards->countCards(), 2);

  REQUIRE(cards.gameDestinationCards);
  REQUIRE(cards.gameDestinationCards->faceDownCards);
  DEBUG_PRINT("Shared destination faceDown count: " << cards.gameDestinationCards->faceDownCards->countCards());
  CHECK_EQ(cards.gameDestinationCards->faceDownCards->countCards(), 1);
  REQUIRE(cards.gameWagonCards);
  REQUIRE(cards.gameWagonCards->faceDownCards);
  DEBUG_PRINT("Shared wagon faceDown count: " << cards.gameWagonCards->faceDownCards->countCards());
  CHECK_EQ(cards.gameWagonCards->faceDownCards->countCards(), 2);

  int ferryCount = 0;
  int tunnelCount = 0;
  std::vector<std::shared_ptr<mapState::Road>> roads = map.getRoads();
  for (std::size_t i = 0; i < roads.size(); ++i)
  {
    std::string kind = "ROAD";
    if (std::dynamic_pointer_cast<mapState::Ferry>(roads[i]))
    {
      ferryCount += 1;
      kind = "FERRY";
    }
    if (std::dynamic_pointer_cast<mapState::Tunnel>(roads[i]))
    {
      tunnelCount += 1;
      kind = "TUNNEL";
    }
    std::string ownerName = "NONE";
    if (roads[i]->getOwner())
    {
      ownerName = roads[i]->getOwner()->getName();
    }
    DEBUG_PRINT("Road " << i << ": " << roads[i]->getStationA()->getName()
                        << " -> " << roads[i]->getStationB()->getName()
                        << " | " << kind
                        << " | length " << roads[i]->getLength()
                        << " | owner " << ownerName);
  }
  CHECK_EQ(ferryCount, 1);
  CHECK_EQ(tunnelCount, 1);
  ANN_END("ParseInitStateFixture")
}

// Ensures command JSON survives a serialize/parse round-trip.
TEST(JSONParserCommandRoundTrip)
{
  ANN_START("JSONParserCommandRoundTrip")
  parser::JSONParser parser;
  parser::CommandMessage command;
  command.name = "draw";
  command.playerID = 1;
  command.turn = 2;
  command.requestID = "req-1";
  command.version = 1;
  command.payload["count"] = 2;

  std::string json = parser.serializeCommand(command);
  parser::CommandMessage parsed;
  std::string error;
  bool ok = parser.parseCommand(json, parsed, error);
  CHECK(ok);
  CHECK_EQ(parsed.name, "draw");
  CHECK_EQ(parsed.playerID, 1);
  CHECK_EQ(parsed.turn, 2);
  CHECK_EQ(parsed.requestID, "req-1");
  CHECK_EQ(parsed.payload["count"].asInt(), 2);
  ANN_END("JSONParserCommandRoundTrip")
}

// Accepts missing kind/origin and defaults them for commands.
TEST(JSONParserCommandDefaults)
{
  ANN_START("JSONParserCommandDefaults")
  parser::JSONParser parser;
  Json::Value root(Json::objectValue);
  root["name"] = "exit";
  std::string json = writeJson(root);
  DEBUG_PRINT("Command defaults JSON: " << json);

  parser::CommandMessage parsed;
  std::string error;
  bool ok = parser.parseCommand(json, parsed, error);
  DEBUG_PRINT("Command defaults ok: " << ok << " error: " << error);
  DEBUG_PRINT("Command defaults parsed kind: " << parsed.kind
              << " origin: " << parsed.origin
              << " name: " << parsed.name);
  CHECK(ok);
  CHECK_EQ(parsed.name, "exit");
  CHECK_EQ(parsed.kind, "command");
  CHECK_EQ(parsed.origin, "tui");
  CHECK_EQ(parsed.version, 1);
  ANN_END("JSONParserCommandDefaults")
}

// Rejects commands that claim to originate from the engine.
TEST(JSONParserCommandRejectsOrigin)
{
  ANN_START("JSONParserCommandRejectsOrigin")
  parser::JSONParser parser;
  Json::Value root(Json::objectValue);
  root["name"] = "exit";
  root["kind"] = "command";
  root["origin"] = "engine";
  std::string json = writeJson(root);
  DEBUG_PRINT("Command bad origin JSON: " << json);

  parser::CommandMessage parsed;
  std::string error;
  bool ok = parser.parseCommand(json, parsed, error);
  DEBUG_PRINT("Command bad origin ok: " << ok << " error: " << error);
  CHECK(!ok);
  CHECK(!error.empty());
  ANN_END("JSONParserCommandRejectsOrigin")
}

// Ensures result JSON and its events round-trip correctly.
TEST(JSONParserResultRoundTrip)
{
  ANN_START("JSONParserResultRoundTrip")
  parser::JSONParser parser;
  parser::ResultMessage result;
  result.ok = true;
  result.error = "";
  result.kind = "result";
  result.origin = "engine";
  result.version = 1;
  result.requestID = "req-9";
  result.playerID = 3;
  result.turn = 7;
  result.payload["nextPhase"] = 2;
  parser::EventMessage event;
  event.kind = "event";
  event.origin = "engine";
  event.version = 1;
  event.requestID = "req-9";
  event.playerID = 3;
  event.turn = 7;
  event.type = "INFO";
  event.message = "hello";
  event.payload["note"] = "test";
  result.events.push_back(event);

  std::string json = parser.serializeResult(result);
  DEBUG_PRINT("Result roundtrip JSON: " << json);

  parser::ResultMessage parsed;
  std::string error;
  bool ok = parser.parseResult(json, parsed, error);
  DEBUG_PRINT("Result roundtrip ok: " << ok << " error: " << error);
  DEBUG_PRINT("Result roundtrip parsed kind: " << parsed.kind
              << " origin: " << parsed.origin
              << " events: " << parsed.events.size());
  CHECK(ok);
  CHECK_EQ(parsed.ok, true);
  CHECK_EQ(parsed.kind, "result");
  CHECK_EQ(parsed.origin, "engine");
  CHECK_EQ(parsed.requestID, "req-9");
  CHECK_EQ(parsed.playerID, 3);
  CHECK_EQ(parsed.turn, 7);
  CHECK_EQ(parsed.payload["nextPhase"].asInt(), 2);
  CHECK_EQ(parsed.events.size(), 1);
  CHECK_EQ(parsed.events[0].type, "INFO");
  CHECK_EQ(parsed.events[0].message, "hello");
  CHECK_EQ(parsed.events[0].origin, "engine");
  CHECK_EQ(parsed.events[0].playerID, 3);
  CHECK_EQ(parsed.events[0].turn, 7);
  ANN_END("JSONParserResultRoundTrip")
}

// Rejects result payloads that declare the wrong kind.
TEST(JSONParserResultRejectsKind)
{
  ANN_START("JSONParserResultRejectsKind")
  parser::JSONParser parser;
  Json::Value root(Json::objectValue);
  root["kind"] = "command";
  root["origin"] = "engine";
  root["ok"] = true;
  root["payload"] = Json::Value(Json::objectValue);
  std::string json = writeJson(root);
  DEBUG_PRINT("Result bad kind JSON: " << json);

  parser::ResultMessage parsed;
  std::string error;
  bool ok = parser.parseResult(json, parsed, error);
  DEBUG_PRINT("Result bad kind ok: " << ok << " error: " << error);
  CHECK(!ok);
  CHECK(!error.empty());
  ANN_END("JSONParserResultRejectsKind")
}

// Rejects events that claim to originate from the TUI.
TEST(JSONParserEventRejectsOrigin)
{
  ANN_START("JSONParserEventRejectsOrigin")
  parser::JSONParser parser;
  Json::Value root(Json::objectValue);
  root["kind"] = "result";
  root["origin"] = "engine";
  root["ok"] = true;
  Json::Value event(Json::objectValue);
  event["kind"] = "event";
  event["type"] = "INFO";
  event["message"] = "bad-origin";
  event["origin"] = "tui";
  root["events"] = Json::Value(Json::arrayValue);
  root["events"].append(event);
  std::string json = writeJson(root);
  DEBUG_PRINT("Event bad origin JSON: " << json);

  parser::ResultMessage parsed;
  std::string error;
  bool ok = parser.parseResult(json, parsed, error);
  DEBUG_PRINT("Event bad origin ok: " << ok << " error: " << error);
  CHECK(!ok);
  CHECK(!error.empty());
  ANN_END("JSONParserEventRejectsOrigin")
}

SUITE_END() // Parser

/* vim: set sw=2 sts=2 et : */
