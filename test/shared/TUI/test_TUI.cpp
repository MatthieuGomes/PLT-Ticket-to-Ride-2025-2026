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

bool containsText(const std::string& haystack, const std::string& needle) {
  return haystack.find(needle) != std::string::npos;
}

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

BOOST_AUTO_TEST_SUITE(TerminalTests)

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

BOOST_AUTO_TEST_SUITE(BaseViewTests)

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

BOOST_AUTO_TEST_SUITE(StatusBarTests)

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

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CommandInputTests)

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

BOOST_AUTO_TEST_SUITE(InfoPanelTests)

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

BOOST_AUTO_TEST_SUITE(MapViewTests)

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

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TUIManagerTests)

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
