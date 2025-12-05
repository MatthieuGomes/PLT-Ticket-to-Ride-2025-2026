#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/playersState/Player.h"

using namespace ::mapState;



BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

int testId = 1;
bool testBlocked = false;
std::string testName = "TestStation";




BOOST_AUTO_TEST_CASE(TestConstructor)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  
  Station station = Station(&mapState, testId, testName, testOwner, testBlocked);
  BOOST_CHECK(station.id == testId);
  BOOST_CHECK(station.isBlocked == testBlocked);
  BOOST_CHECK(station.owner == testOwner);
  BOOST_CHECK(station.name == testName);
}

BOOST_AUTO_TEST_CASE(TestGetId)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station station = Station(&mapState, testId, testName, testOwner, testBlocked);
  BOOST_CHECK(station.getId() == testId);
}

BOOST_AUTO_TEST_CASE(TestGetOwner)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station station = Station(&mapState, testId, testName, testOwner, testBlocked);
  BOOST_CHECK(station.getOwner() == testOwner);
}

BOOST_AUTO_TEST_CASE(TestGetBlockStatus)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station station = Station(&mapState, testId, testName, testOwner, testBlocked);
  BOOST_CHECK(station.getBlockStatus() == testBlocked);
}

BOOST_AUTO_TEST_CASE(TestGetName)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  Station station = Station(&mapState, testId, testName, testOwner, testBlocked);
  BOOST_CHECK(station.getName() == testName);
}
/* vim: set sw=2 sts=2 et : */