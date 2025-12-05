#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/MapElement.h"
#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/playersState/Player.h"

using namespace ::mapState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

int testId = 1;
bool testBlocked = false;



BOOST_AUTO_TEST_CASE(TestConstructor)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  MapElement mapElement = MapElement(&mapState, testId, testOwner, testBlocked);
  BOOST_CHECK(mapElement.id == testId);
  BOOST_CHECK(mapElement.isBlocked == testBlocked);
  BOOST_CHECK(mapElement.owner == testOwner);
}

BOOST_AUTO_TEST_CASE(TestGetId)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  MapElement mapElement = MapElement(&mapState, testId, testOwner, testBlocked);
  BOOST_CHECK(mapElement.getId() == testId);
}

BOOST_AUTO_TEST_CASE(TestGetOwner)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  MapElement mapElement = MapElement(&mapState, testId, testOwner, testBlocked);
  BOOST_CHECK(mapElement.getOwner() == testOwner);
}

BOOST_AUTO_TEST_CASE(TestGetBlockStatus)
{
  MapState mapState = MapState();
  playersState::Player* testOwner = nullptr;
  MapElement mapElement = MapElement(&mapState, testId, testOwner, testBlocked);
  BOOST_CHECK(mapElement.getBlockStatus() == testBlocked);
}

/* vim: set sw=2 sts=2 et : */