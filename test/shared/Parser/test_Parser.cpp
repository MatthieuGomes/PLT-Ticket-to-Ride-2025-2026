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
  return path.substr(0, pos) + "/" + filename;
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
// Covers borrowedRoads by id, station ownership, playerCards hand indexing, and completed destinations.
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
  players.setupFromJSON(jsonContent, std::make_shared<mapState::MapState>(map), std::make_shared<cardsState::CardsState>(cards));

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

// Exercises alternative JSON shapes to hit parser helpers and fallbacks.
// Covers numeric colors, roadType fallback, locos alias, mixed station definitions, and bad road ids.
TEST(ParseInitStateVariantInputs)
{
  ANN_START("ParseInitStateVariantInputs")
  Json::Value root(Json::objectValue);

  Json::Value players(Json::arrayValue);
  Json::Value player0(Json::objectValue);
  player0["name"] = "NumId";
  player0["color"] = 1;
  player0["score"] = 12;
  player0["wagons"] = 30;
  player0["stations"] = 1;
  player0["hand"] = 0;
  Json::Value borrowed(Json::arrayValue);
  borrowed.append("0");
  borrowed.append(2);
  borrowed.append("bad");
  player0["borrowedRoads"] = borrowed;
  players.append(player0);

  Json::Value player1(Json::objectValue);
  player1["name"] = "TxtId";
  player1["color"] = "RED";
  player1["score"] = 1;
  player1["nbWagons"] = 40;
  player1["nbStations"] = 2;
  player1["hand"] = 1;
  players.append(player1);

  root["players"] = players;

  Json::Value map(Json::objectValue);
  Json::Value stations(Json::arrayValue);
  Json::Value alphaObj(Json::objectValue);
  alphaObj["name"] = "alpha";
  alphaObj["TUIrow"] = 0;
  alphaObj["TUIcolumn"] = 0;
  alphaObj["TUInickname"] = "ALP";
  stations.append(alphaObj);
  Json::Value stationObj(Json::objectValue);
  stationObj["name"] = "beta";
  stationObj["owner"] = "NumId";
  stationObj["TUIrow"] = 1;
  stationObj["TUIcolumn"] = 4;
  stationObj["TUInickname"] = "BET";
  stations.append(stationObj);
  Json::Value stationObj2(Json::objectValue);
  stationObj2["name"] = "gamma";
  stationObj2["owner"] = Json::Value(Json::nullValue);
  stationObj2["TUIrow"] = 2;
  stationObj2["TUIcolumn"] = 8;
  stationObj2["TUInickname"] = "GAM";
  stations.append(stationObj2);
  map["stations"] = stations;

  Json::Value roads(Json::arrayValue);
  Json::Value road0(Json::objectValue);
  road0["id"] = 0;
  road0["from"] = "alpha";
  road0["to"] = "beta";
  road0["length"] = 2;
  road0["color"] = 2;
  road0["roadType"] = "ROAD";
  road0["owner"] = "NumId";
  roads.append(road0);

  Json::Value road1(Json::objectValue);
  road1["id"] = 1;
  road1["stationA"] = "beta";
  road1["stationB"] = "gamma";
  road1["length"] = 3;
  road1["color"] = "GREEN";
  road1["type"] = "TUNNEL";
  roads.append(road1);

  Json::Value road2(Json::objectValue);
  road2["id"] = 2;
  road2["stationA"] = "alpha";
  road2["stationB"] = "gamma";
  road2["length"] = 4;
  road2["locos"] = 1;
  road2["type"] = "FERRY";
  roads.append(road2);

  map["roads"] = roads;
  root["map"] = map;

  Json::Value cards(Json::objectValue);
  Json::Value outOfGame(Json::objectValue);
  outOfGame["destinationCards"] = Json::Value(Json::arrayValue);
  outOfGame["wagonCards"] = Json::Value(Json::arrayValue);
  cards["outOfGame"] = outOfGame;

  Json::Value faceUp(Json::objectValue);
  faceUp["destinationCards"] = Json::Value(Json::arrayValue);
  faceUp["wagonCards"] = Json::Value(Json::arrayValue);
  cards["faceUp"] = faceUp;

  Json::Value faceDown(Json::objectValue);
  Json::Value dests(Json::arrayValue);
  Json::Value dest(Json::objectValue);
  dest["from"] = "alpha";
  dest["to"] = "gamma";
  dest["points"] = 5;
  dest["isLong"] = false;
  dests.append(dest);
  faceDown["destinationCards"] = dests;
  Json::Value wagons(Json::arrayValue);
  wagons.append("BLUE");
  Json::Value wagonObj(Json::objectValue);
  wagonObj["color"] = 3;
  wagons.append(wagonObj);
  faceDown["wagonCards"] = wagons;
  cards["faceDown"] = faceDown;

  Json::Value playerCards(Json::arrayValue);
  Json::Value p0(Json::objectValue);
  p0["destinationCards"] = dests;
  p0["wagonCards"] = wagons;
  playerCards.append(p0);
  Json::Value p1(Json::objectValue);
  p1["destinationCards"] = Json::Value(Json::arrayValue);
  p1["wagonCards"] = Json::Value(Json::arrayValue);
  playerCards.append(p1);
  cards["playerCards"] = playerCards;

  root["cards"] = cards;

  std::string jsonPlayers = jsonSection(root, "players");
  std::string jsonMap = jsonSection(root, "map");
  std::string jsonCards = jsonSection(root, "cards");

  playersState::PlayersState playersState = playersState::PlayersState::InitFromJSON(jsonPlayers);
  mapState::MapState mapState = mapState::MapState::ParseFromJSON(jsonMap, std::make_shared<playersState::PlayersState>(playersState));
  cardsState::CardsState cardsState = cardsState::CardsState::ParseFromJSON(jsonCards, std::make_shared<mapState::MapState>(mapState));
  std::string jsonFull = writeJson(root);
  playersState.setupFromJSON(jsonFull, std::make_shared<mapState::MapState>(mapState), std::make_shared<cardsState::CardsState>(cardsState));

  CHECK_EQ(playersState.getPlayers().size(), 2);
  CHECK_EQ(mapState.getStations().size(), 3);
  CHECK_EQ(mapState.getRoads().size(), 3);

  std::shared_ptr<playersState::Player> numId = playersState.getPlayerByName("NumId");
  REQUIRE(numId);
  CHECK_EQ(numId->getBorrowedRoads().size(), 2);

  std::shared_ptr<mapState::Station> beta = mapState.getStationByName("beta");
  REQUIRE(beta);
  REQUIRE(beta->getOwner());
  CHECK_EQ(beta->getOwner()->getName(), "NumId");

  REQUIRE(numId->getHand());
  REQUIRE(numId->getHand()->wagonCards);
  CHECK_EQ(numId->getHand()->wagonCards->countCards(), 2);
  ANN_END("ParseInitStateVariantInputs")
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
