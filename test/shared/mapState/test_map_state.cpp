#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/MapState.h"

using namespace ::mapState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}
// MapState(), listStations() and listRoads() unit tests

std::string stationName1 = "paris";
std::string stationName2 = "berlin";
std::string stationName3 = "madrid";
std::string stationName4 = "rome";
long unsigned int stationCount = 4;
int station1Id = 1;
int station2Id = 2;
int station3Id = 3;
int station4Id = 4;
playersState::Player* noOwner = nullptr;
bool notBlocked = false;
int road1Id = 1;
int road2Id = 2;
int road3Id = 3;
int road4Id = 4;
long unsigned int roadCount = 4;




BOOST_AUTO_TEST_CASE(TestConstructorDefault)
{
  MapState mapState = MapState();
  BOOST_CHECK(mapState.stations.size() == stationCount);
  BOOST_CHECK(mapState.roads.size() == roadCount);
  BOOST_CHECK(mapState.stations[0]->id == station1Id);
  BOOST_CHECK(mapState.stations[0]->name == stationName1);
  BOOST_CHECK(mapState.stations[1]->id == station2Id);
  BOOST_CHECK(mapState.stations[1]->name == stationName2);
  BOOST_CHECK(mapState.stations[2]->id == station3Id);
  BOOST_CHECK(mapState.stations[2]->name == stationName3);
  BOOST_CHECK(mapState.stations[3]->id == station4Id);
  BOOST_CHECK(mapState.stations[3]->name == stationName4);
  BOOST_CHECK(mapState.roads[0]->id == road1Id);
  BOOST_CHECK(mapState.roads[1]->id == road2Id);
  BOOST_CHECK(mapState.roads[2]->id == road3Id);
  BOOST_CHECK(mapState.roads[3]->id == road4Id);
  BOOST_CHECK(mapState.roads[0]->getStationA() == mapState.stations[0]);
  BOOST_CHECK(mapState.roads[0]->getStationB() == mapState.stations[1]);
  BOOST_CHECK(mapState.roads[1]->getStationA() == mapState.stations[1]);
  BOOST_CHECK(mapState.roads[1]->getStationB() == mapState.stations[3]);
  BOOST_CHECK(mapState.roads[2]->getStationA() == mapState.stations[0]);
  BOOST_CHECK(mapState.roads[2]->getStationB() == mapState.stations[2]);
  BOOST_CHECK(mapState.roads[3]->getStationA() == mapState.stations[2]);
  BOOST_CHECK(mapState.roads[3]->getStationB() == mapState.stations[3]);
}

BOOST_AUTO_TEST_CASE(getStations){
  MapState mapState = MapState();
  std::vector<Station*> stations = mapState.getStations();
  BOOST_CHECK(stations.size() == stationCount);
  BOOST_CHECK(stations[0] == mapState.stations[0]);
  BOOST_CHECK(stations[1] == mapState.stations[1]);
  BOOST_CHECK(stations[2] == mapState.stations[2]);
  BOOST_CHECK(stations[3] == mapState.stations[3]);
}

//getRoads()
BOOST_AUTO_TEST_CASE(getRoads)
{
  MapState mapState = MapState();
  std::vector<Road*> roads = mapState.getRoads();
  BOOST_CHECK(roads.size() == roadCount);
  BOOST_CHECK(roads[0] == mapState.roads[0]);
  BOOST_CHECK(roads[1] == mapState.roads[1]);
  BOOST_CHECK(roads[2] == mapState.roads[2]);
  BOOST_CHECK(roads[3] == mapState.roads[3]);

}

//getStationByName()
BOOST_AUTO_TEST_CASE(getStationByName)
{
  MapState mapState = MapState();
  Station* station = mapState.getStationByName(stationName2);
  BOOST_CHECK(station != nullptr);
  BOOST_CHECK(station->name == stationName2);
  BOOST_CHECK(station->id == station2Id);
}
//getRoadBetweenStations()
BOOST_AUTO_TEST_CASE(getRoadBetweenStations)
{
  MapState mapState = MapState();
  Station* stationA = mapState.getStationByName(stationName1);
  Station* stationB = mapState.getStationByName(stationName2);
  Road* road = mapState.getRoadBetweenStations(stationA, stationB);
  BOOST_CHECK(road != nullptr);
  BOOST_CHECK(road->id == road1Id);
  BOOST_CHECK(road->getStationA() == stationA || road->getStationB() == stationA);
  BOOST_CHECK(road->getStationA() == stationB || road->getStationB() == stationB);
}


//findShortestPath()
BOOST_AUTO_TEST_CASE(TestShortestPath)
{
}


//ToString()
BOOST_AUTO_TEST_CASE(TestToString)
{
  MapState mapState = MapState();
  std::string mapString = mapState.toString();
  BOOST_CHECK(mapString.find("===== MAP STATE =====") != std::string::npos);
  BOOST_CHECK(mapString.find("Stations:") != std::string::npos);
  BOOST_CHECK(mapString.find("Roads:") != std::string::npos);
  BOOST_CHECK(mapString.find("paris") != std::string::npos);
  BOOST_CHECK(mapString.find("berlin") != std::string::npos);
  BOOST_CHECK(mapString.find("madrid") != std::string::npos);
  BOOST_CHECK(mapString.find("rome") != std::string::npos);
  BOOST_CHECK(mapString.find("==========================") != std::string::npos);
}




/* vim: set sw=2 sts=2 et : */
