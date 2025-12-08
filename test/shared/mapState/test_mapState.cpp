
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/playersState/Player.h"
#include <boost/graph/adjacency_list.hpp>

using namespace ::mapState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
mapState::MapState test_map_state = mapState::MapState();

BOOST_AUTO_TEST_SUITE(Getters)
BOOST_AUTO_TEST_CASE(GetStations)
{
  std::vector<mapState::Station*> stations = test_map_state.getStations();
  BOOST_CHECK_EQUAL(stations.size(), 4); // As per the default constructor in MapState
}
BOOST_AUTO_TEST_CASE(GetRoads)
{
  std::vector<mapState::Road*> roads = test_map_state.getRoads();
  BOOST_CHECK_EQUAL(roads.size(), 4); // As per the default constructor in MapState
}
BOOST_AUTO_TEST_CASE(GetStationByName)
{
  mapState::Station* station = test_map_state.getStationByName("paris");
  BOOST_CHECK(station != nullptr);
  BOOST_CHECK_EQUAL(station->getName(), "paris");
}
BOOST_AUTO_TEST_CASE(GetRoadBetweenStations)
{
  mapState::Station* stationA = test_map_state.getStationByName("paris");
  mapState::Station* stationB = test_map_state.getStationByName("berlin");
  mapState::Road* road = test_map_state.getRoadBetweenStations(stationA, stationB);
  BOOST_CHECK(road != nullptr);
  BOOST_CHECK_EQUAL(road->getStationA(), stationA);
  BOOST_CHECK_EQUAL(road->getStationB(), stationB);
}
BOOST_AUTO_TEST_CASE(GetAdjacentStations)
{
  mapState::Station* station = test_map_state.getStationByName("paris");
  std::vector<mapState::Station*> adjacentStations = test_map_state.getAdjacentStations(station);
  BOOST_CHECK_EQUAL(adjacentStations.size(), 2); // paris is connected to berlin and madrid
}
BOOST_AUTO_TEST_CASE(FindShortestPath)
{
  mapState::Station* src = test_map_state.getStationByName("paris");
  mapState::Station* dest = test_map_state.getStationByName("rome");
  Path shortestPath = test_map_state.findShortestPath(src, dest);
  BOOST_CHECK(!shortestPath.STATIONS.empty());
  BOOST_CHECK_EQUAL(shortestPath.STATIONS.front(), src);
  BOOST_CHECK_EQUAL(shortestPath.STATIONS.back(), dest);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
