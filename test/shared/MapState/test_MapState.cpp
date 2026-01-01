
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/playersState/Player.h"
#include "../../src/shared/mapState/Tunnel.h"
#include "../../src/shared/mapState/Ferry.h"
#include <boost/graph/adjacency_list.hpp>
#include <memory>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

using namespace ::mapState;

using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, bool, std::string>;
using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>;
using RoadInfo = std::pair<StationPair, RoadDetail>;
using TunnelDetail = RoadDetail;
using TunnelInfo = RoadInfo;
using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, int, bool>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Default)
{
  std::cout << "Default Constructor Test Started ..." << std::endl;
  mapState::MapState map_state = mapState::MapState();
#ifdef DEBUG
  map_state.display();
#endif

  BOOST_CHECK_EQUAL(map_state.stations.size(), 4); // As per the default constructor in MapState
  BOOST_CHECK_EQUAL(map_state.roads.size(), 4);    // As per the default constructor in MapState
  std::cout << "Default Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(Wrapper)
{
  std::cout << "Wrapper Constructor Test Started ..." << std::endl;
  boost::adjacency_list<> test_graph = boost::adjacency_list<>();
  std::vector<StationInfo> stationsInfos = {
      Station::genData(nullptr, false, "Station1"),
      Station::genData(nullptr, false, "Station2"),
  };
  auto station1 = std::make_shared<Station>("Station1", nullptr, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  auto station2 = std::make_shared<Station>("Station2", nullptr, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  std::vector<RoadInfo> roadsInfos = {
      Road::genData(station1, station2, 1, nullptr, cardsState::ColorCard::RED, 3, false),
  };
  std::vector<TunnelInfo> tunnelsInfos = {
      Tunnel::genData(station2, station1, 2, nullptr, cardsState::ColorCard::BLUE, 4, true),
  };
  std::vector<FerryInfo> ferrysInfos = {
      Ferry::genData(station1, station2, 3, nullptr, cardsState::ColorCard::GREEN, 5, 1, false),
  };

  mapState::MapState map_state = mapState::MapState(nullptr, stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos);
  BOOST_CHECK_EQUAL(map_state.stations.size(), 2);
  BOOST_CHECK_EQUAL(map_state.roads.size(), 3); // 1 road + 1 tunnel + 1 ferry
  for (int i = 0; i < static_cast<int>(map_state.stations.size()); i++)
  {
    BOOST_CHECK(map_state.stations[i]->name == "Station1" || map_state.stations[i]->name == "Station2");
  }
  for (int i = 0; i < static_cast<int>(map_state.roads.size()); i++)
  {
    const std::string &stationAName = map_state.roads[i]->stationA->getName();
    const std::string &stationBName = map_state.roads[i]->stationB->getName();
    BOOST_CHECK(stationAName == station1->getName() || stationAName == station2->getName());
    BOOST_CHECK(stationBName == station1->getName() || stationBName == station2->getName());
  }
  std::cout << "Wrapper Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(Empty)
{
  std::cout << "Empty Constructor Test Started ..." << std::endl;
  mapState::MapState map_state = mapState::MapState::Empty();
  BOOST_CHECK_EQUAL(map_state.stations.size(), 0);
  BOOST_CHECK_EQUAL(map_state.roads.size(), 0);
  std::cout << "Empty Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(Parameterized)
{
  std::cout << "Parameterized Constructor Test Started ..." << std::endl;
  mapState::MapState map_state = MapState::Empty();

  boost::adjacency_list<> test_graph = boost::adjacency_list<>();
  std::vector<StationInfo> stationsInfos = {
      Station::genData(nullptr, false, "StationA"),
      Station::genData(nullptr, false, "StationB"),
      Station::genData(nullptr, false, "StationC"),
  };
  auto stationA = std::make_shared<Station>("StationA", nullptr, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  auto stationB = std::make_shared<Station>("StationB", nullptr, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  auto stationC = std::make_shared<Station>("StationC", nullptr, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  std::vector<RoadInfo> roadsInfos = {
      Road::genData(stationA, stationB, 1, nullptr, cardsState::ColorCard::RED, 3, false),
      Road::genData(stationB, stationC, 2, nullptr, cardsState::ColorCard::BLUE, 4, true),
  };
  std::vector<TunnelInfo> tunnelsInfos = {
      Tunnel::genData(stationA, stationC, 3, nullptr, cardsState::ColorCard::GREEN, 5, false),
  };
  std::vector<FerryInfo> ferrysInfos = {
      Ferry::genData(stationC, stationA, 4, nullptr, cardsState::ColorCard::YELLOW, 6, 2, true),
  };

  map_state._MapState(map_state.gameGraph, stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos);
  BOOST_CHECK_EQUAL(map_state.stations.size(), 3);
  BOOST_CHECK_EQUAL(map_state.roads.size(), 4); // 2 roads + 1 tunnel + 1 ferry
  std::cout << "Parameterized Constructor Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)
BOOST_AUTO_TEST_CASE(GetStations)
{
  mapState::MapState test_map_state = mapState::MapState();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::cout << "GetStations Test Started ..." << std::endl;
  std::vector<std::shared_ptr<mapState::Station>> stations = test_map_state.getStations();
  BOOST_CHECK_EQUAL(stations.size(), 4); // As per the default constructor in MapState
  std::cout << "GetStations Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(GetRoads)
{
  mapState::MapState test_map_state = mapState::MapState();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::cout << "GetRoads Test Started ..." << std::endl;
  std::vector<std::shared_ptr<mapState::Road>> roads = test_map_state.getRoads();
  BOOST_CHECK_EQUAL(roads.size(), 4); // As per the default constructor in MapState
  std::cout << "GetRoads Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(GetStationByName)
{
  std::cout << "GetStationByName Test Started ..." << std::endl;
  {
    mapState::MapState test_map_state = mapState::MapState();
#ifdef DEBUG
    test_map_state.display();
#endif
    std::cout << "\tDefined case started ..." << std::endl;
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("paris");
    BOOST_CHECK(station != nullptr);
    BOOST_CHECK_EQUAL(station->getName(), "paris");
    std::cout << "\tDefined case finished !\n"
              << std::endl;
  }
  {
    mapState::MapState test_map_state = mapState::MapState();
#ifdef DEBUG
    test_map_state.display();
#endif
    std::cout << "\tUndefined case started ..." << std::endl;
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("nonexistent");
    BOOST_CHECK(station == nullptr);
    std::cout << "\tUndefined case finished !\n"
              << std::endl;
  }
  std::cout << "GetStationByName Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(GetRoadBetweenStations)
{
  mapState::MapState test_map_state = mapState::MapState();
  std::shared_ptr<mapState::Station> stationA = test_map_state.getStationByName("paris");
  std::shared_ptr<mapState::Station> stationB = test_map_state.getStationByName("berlin");
  std::vector<std::shared_ptr<mapState::Road>> road = test_map_state.getRoadBetweenStations(stationA, stationB);
  BOOST_CHECK_NE(road.empty(), true);
  for (const std::shared_ptr<mapState::Road> &road : road)
  {
    BOOST_CHECK_EQUAL(road->stationA->name, stationA->name);
    BOOST_CHECK_EQUAL(road->stationB->name, stationB->name);
  }
}

BOOST_AUTO_TEST_CASE(GetAdjacentStations)
{
  std::cout << "GetAdjacentStations Test Started ..." << std::endl;
  mapState::MapState test_map_state = mapState::MapState();
#ifdef DEBUG
  test_map_state.display();
#endif
  {
    std::cout << "\tDefined case started ..." << std::endl;
#ifdef DEBUG
    test_map_state.display();
#endif
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("paris");
    std::vector<std::shared_ptr<mapState::Station>> adjacentStations = test_map_state.getAdjacentStations(station);
    BOOST_CHECK_EQUAL(adjacentStations.size(), 2); // paris is connected to berlin and madrid
    std::cout << "\tDefined case finished !\n"
              << std::endl;
  }
  {
    std::cout << "\tUndefined case started ..." << std::endl;
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("lol");
    std::vector<std::shared_ptr<mapState::Station>> adjacentStations = test_map_state.getAdjacentStations(station);
    // BOOST_CHECK_EQUAL(adjacentStations.size(), 0); // lol does not exist = no adjacent stations
    std::cout << "\tUndefined case finished !\n"
              << std::endl;
  }
  std::cout << "GetAdjacentStations Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(FindShortestPath)
{
  mapState::MapState test_map_state = mapState::MapState();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::cout << "FindShortestPath Test Started ..." << std::endl;
  std::shared_ptr<mapState::Station> src = test_map_state.getStationByName("paris");
  std::shared_ptr<mapState::Station> dest = test_map_state.getStationByName("rome");
  Path shortestPath = test_map_state.findShortestPath(src, dest);
#ifdef DEBUG
  std::cout << "Shortest path from " << src->getName() << " to " << dest->getName() << ":\n";
  for (const std::shared_ptr<Station> &station : shortestPath.STATIONS)
  {
    std::cout << station->getName() << " ";
  }
  std::cout << "\nTotal Length: " << shortestPath.TOTALLENGTH << "\n";
  std::cout << "Number of Edges: " << shortestPath.NUMEDGES << "\n";
#endif
  BOOST_CHECK(!shortestPath.STATIONS.empty());
  BOOST_CHECK_EQUAL(shortestPath.STATIONS.front(), src);
  BOOST_CHECK_EQUAL(shortestPath.STATIONS.back(), dest);
  std::cout << "FindShortestPath Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
