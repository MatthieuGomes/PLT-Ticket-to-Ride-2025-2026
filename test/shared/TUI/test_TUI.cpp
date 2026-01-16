#include <boost/test/unit_test.hpp>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../../src/shared/tui/BaseView.h"
#include "../../src/shared/tui/CommandParser.h"
#include "../../src/shared/tui/CommandInput.h"
#include "../../src/shared/tui/InfoPanel.h"
#include "../../src/shared/tui/GameView.h"
#include "../../src/shared/tui/StatusBar.h"
#include "../../src/shared/tui/Terminal.h"
#include "../../src/shared/tui/TUIManager.h"

#include "../../src/shared/cardsState/PlayerCards.h"
#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"

#include <boost/graph/adjacency_list.hpp>
#include <json/json.h>

namespace {

#define DEBUG_MODE true
#if DEBUG_MODE == true
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

// Captures std::cout output for ANSI/string assertions
class CoutCapture {
 public:
  CoutCapture() : oldBuffer(std::cout.rdbuf(buffer.rdbuf())) {}
  ~CoutCapture() {
    std::cout.rdbuf(oldBuffer);
  }

  std::string str() const {
    return buffer.str();
  }

 private:
  std::ostringstream buffer;
  std::streambuf* oldBuffer;
};

// Substring lookup helper for test assertions
bool containsText(const std::string& str1, const std::string& str2) {
  return str1.find(str2) != std::string::npos;
}

// Helper to parse JSON from a string for command assertions.
bool parseJsonText(const std::string& text, Json::Value& root) {
  Json::CharReaderBuilder builder;
  builder["collectComments"] = false;
  std::string error;
  std::istringstream input(text);
  return Json::parseFromStream(builder, input, &root, &error);
}

// Minimal BaseView subclass that writes a small content marker
class TestView : public tui::BaseView {
 public:
  TestView(int x, int y, int width, int height)
      : BaseView(x, y, width, height, "Test") {}

  bool needsRedrawPublic() const {
    return getNeedsRedraw();
  }

  int getXPublic() const {
    return getX();
  }

  int getYPublic() const {
    return getY();
  }

  int getWidthPublic() const {
    return getWidth();
  }

  int getHeightPublic() const {
    return getHeight();
  }

 protected:
  void drawContent(tui::Terminal& term) override {
    term.moveTo(y + 1, x + 1);
    term.write("content");
  }
};

}  // namespace

// Terminal behavior and ANSI output smoke tests
BOOST_AUTO_TEST_SUITE(TerminalTests)

// Verifies basic ANSI output for cursor positioning and colors
BOOST_AUTO_TEST_CASE(WriteAndColors)
{
  tui::Terminal term("");
  CoutCapture capture;

  term.moveTo(0, 0);
  term.setFg(tui::Color::Red);
  term.setBg(tui::Color::Blue);
  term.write("X");
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "\033[1;1H"));
  BOOST_CHECK(containsText(out, "\033[31m"));
  BOOST_CHECK(containsText(out, "\033[44m"));
  BOOST_CHECK(containsText(out, "X"));
}

// Validates 256-color output for orange variants
BOOST_AUTO_TEST_CASE(ExtendedOrangeColors)
{
  tui::Terminal term("");
  CoutCapture capture;

  term.setFg(tui::Color::Orange);
  term.setBg(tui::Color::BrightOrange);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "\033[38;5;208m"));
  BOOST_CHECK(containsText(out, "\033[48;5;214m"));
}

// Checks cursor hide/show escape sequences are emitted
BOOST_AUTO_TEST_CASE(CursorVisibility)
{
  tui::Terminal term("");
  CoutCapture capture;

  term.hideCursor();
  term.showCursor();
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "\033[?25l"));
  BOOST_CHECK(containsText(out, "\033[?25h"));
}

BOOST_AUTO_TEST_SUITE_END()

// TUIManager initialization, drawing, and external state wiring
BOOST_AUTO_TEST_SUITE(TUIManagerTests)

// End-to-end smoke test for draw and input handling
BOOST_AUTO_TEST_CASE(InitialDrawAndInput)
{
  tui::Terminal term("");
  tui::TUIManager manager(&term, 80, 24);
  manager.init();

  {
    CoutCapture capture;
    manager.drawAll();
    std::string out = capture.str();
    BOOST_CHECK(containsText(out, "Ticket to Ride"));
    BOOST_CHECK(containsText(out, "Turn 1 | Your turn"));
    BOOST_CHECK(containsText(out, "Welcome to Ticket to Ride!"));
  }

  manager.handleInput("hello");

  {
    CoutCapture capture;
    manager.drawAll();
    std::string out = capture.str();
    BOOST_CHECK(containsText(out, "hello"));
  }

  manager.shutdown();
}

// Ensures external state pointers are accepted and drawn
BOOST_AUTO_TEST_CASE(ExternalStatesConstructor)
{
  tui::Terminal term("");
  std::shared_ptr<mapState::MapState> map_state =
      std::make_shared<mapState::MapState>();
  std::shared_ptr<playersState::PlayersState> players_state =
      std::make_shared<playersState::PlayersState>();
  std::shared_ptr<cardsState::CardsState> cards_state =
      std::make_shared<cardsState::CardsState>();

  tui::TUIManager manager(&term, 80, 24,
                          map_state.get(),
                          players_state.get(),
                          cards_state.get());
  manager.init();

  {
    CoutCapture capture;
    manager.drawAll();
    std::string out = capture.str();
    BOOST_CHECK(containsText(out, "Ticket to Ride"));
  }

  manager.shutdown();
}

BOOST_AUTO_TEST_SUITE_END()

/*
// Command parser JSON output and context wiring
// Disabled on this branch: requires parser::JSONParser implementation
BOOST_AUTO_TEST_SUITE(CommandParserTests)

// Ensures command JSON includes kind/origin/name and context fields.
BOOST_AUTO_TEST_CASE(EncodesBasicCommand)
{
  tui::CommandParser parser;
  parser.setContext(1, 2, "req-1");

  tui::ParseResult result = parser.parse("exit");
  DEBUG_PRINT("Command JSON: " << result.json);
  BOOST_CHECK(result.ok);

  Json::Value root;
  BOOST_CHECK(parseJsonText(result.json, root));
  BOOST_CHECK_EQUAL(root["kind"].asString(), "command");
  BOOST_CHECK_EQUAL(root["origin"].asString(), "tui");
  BOOST_CHECK_EQUAL(root["name"].asString(), "exit");
  BOOST_CHECK_EQUAL(root["playerId"].asInt(), 1);
  BOOST_CHECK_EQUAL(root["turn"].asInt(), 2);
  BOOST_CHECK_EQUAL(root["requestId"].asString(), "req-1");
  BOOST_CHECK(root.isMember("payload"));
}

// Verifies args are captured in the payload and quit maps to exit.
BOOST_AUTO_TEST_CASE(EncodesArgsAndAlias)
{
  tui::CommandParser parser;
  parser.setContext(3, 4, "req-2");

  tui::ParseResult result = parser.parse("quit Alpha Beta");
  DEBUG_PRINT("Command JSON with args: " << result.json);
  BOOST_CHECK(result.ok);

  Json::Value root;
  BOOST_CHECK(parseJsonText(result.json, root));
  BOOST_CHECK_EQUAL(root["name"].asString(), "exit");
  BOOST_CHECK_EQUAL(root["payload"]["args"].asString(), "Alpha Beta");
}

// Empty input should be rejected without JSON output.
BOOST_AUTO_TEST_CASE(RejectsEmptyInput)
{
  tui::CommandParser parser;
  tui::ParseResult result = parser.parse("   ");
  DEBUG_PRINT("Empty input ok: " << result.ok << " json: " << result.json);
  BOOST_CHECK(!result.ok);
  BOOST_CHECK(result.json.empty());
}

BOOST_AUTO_TEST_SUITE_END()
*/

// BaseView redraw and geometry behavior
BOOST_AUTO_TEST_SUITE(BaseViewTests)

// Ensures redraw flags toggle correctly on updates
BOOST_AUTO_TEST_CASE(RedrawFlags)
{
  TestView view(1, 1, 10, 5);
  BOOST_CHECK(view.needsRedrawPublic());

  tui::Terminal term("");
  view.refresh(term);
  BOOST_CHECK(!view.needsRedrawPublic());

  view.setPosition(2, 2);
  BOOST_CHECK(view.needsRedrawPublic());

  view.refresh(term);
  BOOST_CHECK(!view.needsRedrawPublic());

  view.setPosition(2, 2);
  BOOST_CHECK(!view.needsRedrawPublic());
}

// Confirms resizing and title updates are applied
BOOST_AUTO_TEST_CASE(SizeAndTitle)
{
  TestView view(1, 1, 10, 5);
  view.setSize(20, 7);
  BOOST_CHECK_EQUAL(view.getWidthPublic(), 20);
  BOOST_CHECK_EQUAL(view.getHeightPublic(), 7);
  view.setTitle("NewTitle");
  BOOST_CHECK_EQUAL(view.getTitle(), "NewTitle");
}

BOOST_AUTO_TEST_SUITE_END()

// StatusBar text rendering and state changes
BOOST_AUTO_TEST_SUITE(StatusBarTests)

// Verifies composed status line with game/turn/player data
BOOST_AUTO_TEST_CASE(RendersStatusLine)
{
  std::vector<std::shared_ptr<cardsState::DestinationCard>> destinationCards;
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCards;
  std::shared_ptr<cardsState::PlayerCards> hand =
      std::make_shared<cardsState::PlayerCards>(destinationCards, wagonCards);
  std::vector<std::shared_ptr<mapState::Road>> borrowedRoads;
  playersState::Player player("Alice", playersState::PlayerColor::RED, 0, 0, 0,
                              borrowedRoads, hand);

  tui::StatusBar bar(1, 1, 80, 3);
  bar.setGameInfo("TestGame", "1.0");
  bar.setTurn(2);
  bar.setCurrentPlayer(&player);

  tui::Terminal term("");
  CoutCapture capture;
  bar.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "TestGame v1.0"));
  BOOST_CHECK(containsText(out, "Turn 2 | Your turn"));
  BOOST_CHECK(containsText(out, "Alice"));
}

// Ensures missing fields produce a minimal status line
BOOST_AUTO_TEST_CASE(HandlesMissingInfo)
{
  tui::StatusBar bar(1, 1, 40, 3);
  bar.setGameInfo("", "2.0");
  bar.setTurn(0);
  bar.setCurrentPlayer(nullptr);

  tui::Terminal term("");
  CoutCapture capture;
  bar.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "v2.0"));
  BOOST_CHECK(!containsText(out, "Turn"));
}

// Confirms updates propagate when turn/player changes
BOOST_AUTO_TEST_CASE(UpdatesTurnAndPlayer)
{
  std::vector<std::shared_ptr<cardsState::DestinationCard>> destinationCards;
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCards;
  std::shared_ptr<cardsState::PlayerCards> hand =
      std::make_shared<cardsState::PlayerCards>(destinationCards, wagonCards);
  std::vector<std::shared_ptr<mapState::Road>> borrowedRoads;
  playersState::Player playerA("Alice", playersState::PlayerColor::RED, 0, 0, 0,
                               borrowedRoads, hand);
  playersState::Player playerB("Bob", playersState::PlayerColor::BLUE, 0, 0, 0,
                               borrowedRoads, hand);

  tui::StatusBar bar(1, 1, 60, 3);
  bar.setGameInfo("TestGame", "1.0");
  bar.setTurn(1);
  bar.setCurrentPlayer(&playerA);

  {
    tui::Terminal term("");
    CoutCapture capture;
    bar.refresh(term);
    term.flush();
    std::string out = capture.str();
    BOOST_CHECK(containsText(out, "Turn 1 | Your turn"));
    BOOST_CHECK(containsText(out, "Alice"));
  }

  bar.setTurn(3);
  bar.setCurrentPlayer(&playerB);

  {
    tui::Terminal term("");
    CoutCapture capture;
    bar.refresh(term);
    term.flush();
    std::string out = capture.str();
    BOOST_CHECK(containsText(out, "Turn 3 | Your turn"));
    BOOST_CHECK(containsText(out, "Bob"));
  }
}

BOOST_AUTO_TEST_SUITE_END()

// GameView rendering with minimal map data
BOOST_AUTO_TEST_SUITE(GameViewTests)

// Renders a simple map and validates station/route output
BOOST_AUTO_TEST_CASE(RendersStationsAndRoutes)
{
  std::shared_ptr<boost::adjacency_list<>> graph =
      std::make_shared<boost::adjacency_list<>>();

  std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA =
      std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*graph));
  std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB =
      std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*graph));

  std::shared_ptr<mapState::Station> stationA =
      std::make_shared<mapState::Station>("Alpha", nullptr, vertexA);
  std::shared_ptr<mapState::Station> stationB =
      std::make_shared<mapState::Station>("Beta", nullptr, vertexB);

  std::vector<std::tuple<std::shared_ptr<playersState::Player>, std::string>> stationsInfos;
  stationsInfos.push_back(mapState::Station::genData(nullptr, "Alpha"));
  stationsInfos.push_back(mapState::Station::genData(nullptr, "Beta"));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, mapState::RoadColor, int>>> roadsInfos;
  roadsInfos.push_back(
      mapState::Road::genData(stationA, stationB, 1, nullptr, mapState::RoadColor::ORANGE, 4));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, mapState::RoadColor, int>>> tunnelsInfos;
  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, int, int>>> ferrysInfos;

  mapState::MapState map_state;
  map_state.fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, graph);

  tui::GameView view(1, 1, 80, 8);
  view.setMapState(&map_state);

  tui::Terminal term("");
  CoutCapture capture;
  view.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "Alpha"));
  BOOST_CHECK(containsText(out, "Beta"));
  BOOST_CHECK(containsText(out, "Alpha-Beta"));
  BOOST_CHECK(containsText(out, "len:4"));
  BOOST_CHECK(containsText(out, "ORG"));
}

// Ensures the placeholder appears when no map state is set
BOOST_AUTO_TEST_CASE(HandlesMissingMapState)
{
  tui::GameView view(1, 1, 40, 5);

  tui::Terminal term("");
  CoutCapture capture;
  view.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "No map data"));
}

// Validates highlights and owner tags for stations/routes
BOOST_AUTO_TEST_CASE(HighlightsAndFlags)
{
  std::vector<std::shared_ptr<cardsState::DestinationCard>> destinationCards;
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCards;
  std::shared_ptr<cardsState::PlayerCards> hand =
      std::make_shared<cardsState::PlayerCards>(destinationCards, wagonCards);
  std::vector<std::shared_ptr<mapState::Road>> borrowedRoads;
  std::shared_ptr<playersState::Player> owner =
      std::make_shared<playersState::Player>("Owner", playersState::PlayerColor::RED, 0, 0, 0,
                                             borrowedRoads, hand);

  std::shared_ptr<boost::adjacency_list<>> graph =
      std::make_shared<boost::adjacency_list<>>();
  std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA =
      std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*graph));
  std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB =
      std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*graph));

  std::shared_ptr<mapState::Station> stationA =
      std::make_shared<mapState::Station>("Alpha", owner, vertexA);
  std::shared_ptr<mapState::Station> stationB =
      std::make_shared<mapState::Station>("Beta", nullptr, vertexB);

  std::vector<std::tuple<std::shared_ptr<playersState::Player>, std::string>> stationsInfos;
  stationsInfos.push_back(mapState::Station::genData(owner, "Alpha"));
  stationsInfos.push_back(mapState::Station::genData(nullptr, "Beta"));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, mapState::RoadColor, int>>> roadsInfos;
  roadsInfos.push_back(
      mapState::Road::genData(stationA, stationB, 1, owner, mapState::RoadColor::RED, 4));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, mapState::RoadColor, int>>> tunnelsInfos;
  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, int, int>>> ferrysInfos;

  mapState::MapState map_state;
  map_state.fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, graph);

  tui::GameView view(1, 1, 80, 8);
  view.setMapState(&map_state);
  view.setHighlightNode(0);
  view.setHighlightActivePlayer(true);

  tui::Terminal term("");
  CoutCapture capture;
  view.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, ">Alpha"));
  BOOST_CHECK(containsText(out, "Alpha [Owner]"));
  BOOST_CHECK(containsText(out, "*Alpha-Beta"));
  BOOST_CHECK(containsText(out, "len:4"));
  BOOST_CHECK(containsText(out, " [Owner]"));
}

BOOST_AUTO_TEST_SUITE_END()

// InfoPanel message buffer and scrolling
BOOST_AUTO_TEST_SUITE(InfoPanelTests)

// Verifies message buffer trimming at the max limit
BOOST_AUTO_TEST_CASE(BufferLimit)
{
  tui::InfoPanel panel(1, 1, 40, 5, 2);
  panel.addMessage("MessageOne");
  panel.addMessage("MessageTwo");
  panel.addMessage("MessageThree");

  tui::Terminal term("");
  CoutCapture capture;
  panel.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "MessageTwo"));
  BOOST_CHECK(containsText(out, "MessageThree"));
  BOOST_CHECK(!containsText(out, "MessageOne"));
}

// Checks scrolling changes which lines are visible
BOOST_AUTO_TEST_CASE(ScrollsMessages)
{
  tui::InfoPanel panel(1, 1, 40, 4, 10);
  panel.addMessage("First");
  panel.addMessage("Second");
  panel.addMessage("Third");

  panel.scroll(1);

  tui::Terminal term("");
  CoutCapture capture;
  panel.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "First"));
  BOOST_CHECK(containsText(out, "Second"));
  BOOST_CHECK(!containsText(out, "Third"));
}

// Ensures clearMessages removes all buffered content
BOOST_AUTO_TEST_CASE(ClearMessagesEmptiesPanel)
{
  tui::InfoPanel panel(1, 1, 40, 4, 10);
  panel.addMessage("Alpha");
  panel.addMessage("Beta");

  panel.clearMessages();

  tui::Terminal term("");
  CoutCapture capture;
  panel.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(!containsText(out, "Alpha"));
  BOOST_CHECK(!containsText(out, "Beta"));
}

// Verifies scroll offsets are clamped to the valid range
BOOST_AUTO_TEST_CASE(ScrollClampsToBounds)
{
  tui::InfoPanel panel(1, 1, 40, 3, 10);
  panel.addMessage("First");
  panel.addMessage("Second");
  panel.addMessage("Third");

  panel.scroll(100);

  {
    tui::Terminal term("");
    CoutCapture capture;
    panel.refresh(term);
    term.flush();

    std::string out = capture.str();
    BOOST_CHECK(containsText(out, "First"));
  }

  panel.scroll(-100);

  {
    tui::Terminal term("");
    CoutCapture capture;
    panel.refresh(term);
    term.flush();

    std::string out = capture.str();
    BOOST_CHECK(containsText(out, "Third"));
  }
}

BOOST_AUTO_TEST_SUITE_END()

// CommandInput parsing and prompt rendering
BOOST_AUTO_TEST_SUITE(CommandInputTests)

// Validates parsing and prompt rendering output
BOOST_AUTO_TEST_CASE(ParseAndRender)
{
  tui::CommandInput input(1, 1, 40, 3);
  input.setInput("hello");

  std::string parsed;
  input.parseCommand(parsed);
  BOOST_CHECK_EQUAL(parsed, "hello");

  tui::Terminal term("");
  CoutCapture capture;
  input.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "> hello"));
}

BOOST_AUTO_TEST_SUITE_END()
