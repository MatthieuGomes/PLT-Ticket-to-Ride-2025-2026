#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Ferry.h"
#include "../../src/shared/mapState/Ferry.h"
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
int locomotivesTest = 2;




BOOST_AUTO_TEST_CASE(TestConstructor)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Ferry ferry = Ferry(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, locomotivesTest, testBlocked);
  BOOST_CHECK(ferry.id == testId);
  BOOST_CHECK(ferry.isBlocked == testBlocked);
  BOOST_CHECK(ferry.owner == testOwner);
  BOOST_CHECK(ferry.color == testColor);
  BOOST_CHECK(ferry.length == testLength);
  BOOST_CHECK(ferry.stationA == &testStation1);
  BOOST_CHECK(ferry.stationB == &testStation2);
}

BOOST_AUTO_TEST_CASE(TestGetId)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Ferry ferry = Ferry(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, locomotivesTest, testBlocked);
  BOOST_CHECK(ferry.getId() == testId);
}

BOOST_AUTO_TEST_CASE(TestGetOwner)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Ferry ferry = Ferry(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, locomotivesTest, testBlocked);
  BOOST_CHECK(ferry.getOwner() == testOwner);
}

BOOST_AUTO_TEST_CASE(TestGetBlockStatus)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Ferry ferry = Ferry(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, locomotivesTest, testBlocked);
  BOOST_CHECK(ferry.getBlockStatus() == testBlocked);
}

BOOST_AUTO_TEST_CASE(TestGetStationA)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Ferry ferry = Ferry(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, locomotivesTest, testBlocked);
  BOOST_CHECK(ferry.getStationA() == &testStation1);
}

BOOST_AUTO_TEST_CASE(TestGetStationB)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Ferry ferry = Ferry(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, locomotivesTest, testBlocked);
  BOOST_CHECK(ferry.getStationB() == &testStation2);
}

BOOST_AUTO_TEST_CASE(TestGetLocomotives)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station testStation1 = Station(&mapState, testId, testName1, testOwner, testBlocked);
  Station testStation2 = Station(&mapState, testId + 1, testName2, testOwner, testBlocked);
  cardsState::ColorCard testColor = cardsState::ColorCard::RED;

  Ferry ferry = Ferry(&mapState, testId, testOwner, &testStation1, &testStation2, testColor, testLength, locomotivesTest, testBlocked);
  BOOST_CHECK(ferry.getLocomotives() == locomotivesTest);
}
/* vim: set sw=2 sts=2 et : */