#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/playersState/Player.h"

using namespace ::mapState;



BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

int testId = 1;
bool testBlocked = false;
std::string testName1 = "TestStation1";
std::string testName2 = "TestStation2";
int testLength = 5;




BOOST_AUTO_TEST_CASE(TestConstructor)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Road road = Road(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, testBlocked);
  BOOST_CHECK(road.id == testId);
  BOOST_CHECK(road.isBlocked == testBlocked);
  BOOST_CHECK(road.owner == testOwner);
  BOOST_CHECK(road.color == testColor);
  BOOST_CHECK(road.length == testLength);
  BOOST_CHECK(road.stationA == &testStation1);
  BOOST_CHECK(road.stationB == &testStation2);
}

BOOST_AUTO_TEST_CASE(TestGetId)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Road road = Road(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, testBlocked);
  BOOST_CHECK(road.getId() == testId);
}

BOOST_AUTO_TEST_CASE(TestGetOwner)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Road road = Road(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, testBlocked);
  BOOST_CHECK(road.getOwner() == testOwner);
}

BOOST_AUTO_TEST_CASE(TestGetBlockStatus)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Road road = Road(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, testBlocked);
  BOOST_CHECK(road.getBlockStatus() == testBlocked);
}

BOOST_AUTO_TEST_CASE(TestGetStationA)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Road road = Road(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, testBlocked);
  BOOST_CHECK(road.getStationA() == &testStation1);
}

BOOST_AUTO_TEST_CASE(TestGetStationB)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Road road = Road(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, testBlocked);
  BOOST_CHECK(road.getStationB() == &testStation2);
}
/* vim: set sw=2 sts=2 et : */