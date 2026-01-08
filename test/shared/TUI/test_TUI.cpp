#include <boost/test/unit_test.hpp>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../../src/shared/tui/BaseView.h"
#include "../../src/shared/tui/CommandInput.h"
#include "../../src/shared/tui/InfoPanel.h"
#include "../../src/shared/tui/MapView.h"
#include "../../src/shared/tui/StatusBar.h"
#include "../../src/shared/tui/Terminal.h"
#include "../../src/shared/tui/TUIManager.h"

#include "../../src/shared/cardsState/PlayerCards.h"
#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"

#include <boost/graph/adjacency_list.hpp>

namespace {

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
  cardsState::PlayerCards hand(destinationCards, wagonCards);
  playersState::Player player(1, "Alice", cardsState::ColorCard::RED, 0, 0, 0, 0, &hand);

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
  cardsState::PlayerCards hand(destinationCards, wagonCards);
  playersState::Player playerA(1, "Alice", cardsState::ColorCard::RED, 0, 0, 0, 0, &hand);
  playersState::Player playerB(2, "Bob", cardsState::ColorCard::BLUE, 0, 0, 0, 0, &hand);

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

BOOST_AUTO_TEST_SUITE_END()

// MapView rendering with minimal map data
BOOST_AUTO_TEST_SUITE(MapViewTests)

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
      std::make_shared<mapState::Station>("Alpha", nullptr, false, vertexA);
  std::shared_ptr<mapState::Station> stationB =
      std::make_shared<mapState::Station>("Beta", nullptr, false, vertexB);

  std::vector<std::tuple<std::shared_ptr<playersState::Player>, bool, std::string>> stationsInfos;
  stationsInfos.push_back(mapState::Station::genData(nullptr, false, "Alpha"));
  stationsInfos.push_back(mapState::Station::genData(nullptr, false, "Beta"));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>>> roadsInfos;
  roadsInfos.push_back(
      mapState::Road::genData(stationA, stationB, 1, nullptr, cardsState::ColorCard::LOCOMOTIVE, 4, false));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>>> tunnelsInfos;
  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, int, bool>>> ferrysInfos;

  mapState::MapState map_state = mapState::MapState::Empty();
  map_state._MapState(graph, stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos);

  tui::MapView view(1, 1, 80, 8);
  view.setMapState(&map_state);

  tui::Terminal term("");
  CoutCapture capture;
  view.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "Alpha"));
  BOOST_CHECK(containsText(out, "Beta"));
  BOOST_CHECK(containsText(out, "Alpha-Beta"));
  BOOST_CHECK(containsText(out, "\033[105m"));
  BOOST_CHECK(containsText(out, "\033[106m"));
}

// Ensures the placeholder appears when no map state is set
BOOST_AUTO_TEST_CASE(HandlesMissingMapState)
{
  tui::MapView view(1, 1, 40, 5);

  tui::Terminal term("");
  CoutCapture capture;
  view.refresh(term);
  term.flush();

  std::string out = capture.str();
  BOOST_CHECK(containsText(out, "No map data"));
}

// Validates highlights, owner tags, and block flags for stations/routes
BOOST_AUTO_TEST_CASE(HighlightsAndFlags)
{
  std::vector<std::shared_ptr<cardsState::DestinationCard>> destinationCards;
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCards;
  cardsState::PlayerCards hand(destinationCards, wagonCards);
  std::shared_ptr<playersState::Player> owner =
      std::make_shared<playersState::Player>(1, "Owner", cardsState::ColorCard::RED, 0, 0, 0, 0, &hand);

  std::shared_ptr<boost::adjacency_list<>> graph =
      std::make_shared<boost::adjacency_list<>>();
  std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA =
      std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*graph));
  std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB =
      std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*graph));

  std::shared_ptr<mapState::Station> stationA =
      std::make_shared<mapState::Station>("Alpha", owner, true, vertexA);
  std::shared_ptr<mapState::Station> stationB =
      std::make_shared<mapState::Station>("Beta", nullptr, false, vertexB);

  std::vector<std::tuple<std::shared_ptr<playersState::Player>, bool, std::string>> stationsInfos;
  stationsInfos.push_back(mapState::Station::genData(owner, true, "Alpha"));
  stationsInfos.push_back(mapState::Station::genData(nullptr, false, "Beta"));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>>> roadsInfos;
  roadsInfos.push_back(
      mapState::Road::genData(stationA, stationB, 1, owner, cardsState::ColorCard::RED, 4, true));

  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>>> tunnelsInfos;
  std::vector<std::pair<std::pair<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>>,
                        std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, int, bool>>> ferrysInfos;

  mapState::MapState map_state = mapState::MapState::Empty();
  map_state._MapState(graph, stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos);

  tui::MapView view(1, 1, 80, 8);
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
  BOOST_CHECK(containsText(out, "(blocked)"));
  BOOST_CHECK(containsText(out, "*Alpha-Beta"));
  BOOST_CHECK(containsText(out, "len:4"));
  BOOST_CHECK(containsText(out, " [Owner]"));
  BOOST_CHECK(containsText(out, " !"));
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
