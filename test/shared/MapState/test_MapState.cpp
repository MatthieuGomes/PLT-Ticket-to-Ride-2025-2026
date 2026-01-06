
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

using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;
using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int>;
using RoadInfo = std::pair<StationPair, RoadDetail>;
using TunnelDetail = RoadDetail;
using TunnelInfo = RoadInfo;
using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, int, int>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Empty)
{
  std::cout << "Empty Constructor Test Started ..." << std::endl;
  mapState::MapState map_state = mapState::MapState();
#ifdef DEBUG
  map_state.display();
#endif
  BOOST_CHECK(map_state.gameGraph != nullptr);
  BOOST_CHECK_EQUAL(map_state.stations.size(), 0);
  BOOST_CHECK_EQUAL(map_state.roads.size(), 0);
  std::cout << "Empty Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(Wrapper)
{
  std::cout << "Wrapper Constructor Test Started ..." << std::endl;
  boost::adjacency_list<> test_graph = boost::adjacency_list<>();
  std::vector<StationInfo> stationsInfos = {
      Station::genData(nullptr, "Station1"),
      Station::genData(nullptr, "Station2"),
  };
  auto station1 = std::make_shared<Station>("Station1", nullptr, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  auto station2 = std::make_shared<Station>("Station2", nullptr, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  std::vector<RoadInfo> roadsInfos = {
      Road::genData(station1, station2, 1, nullptr, cardsState::ColorCard::RED, 3),
  };
  std::vector<TunnelInfo> tunnelsInfos = {
      Tunnel::genData(station2, station1, 2, nullptr, cardsState::ColorCard::BLUE, 4),
  };
  std::vector<FerryInfo> ferrysInfos = {
      Ferry::genData(station1, station2, 3, nullptr, 5, 1),
  };
  std::shared_ptr<boost::adjacency_list<>> graph_ptr = std::make_shared<boost::adjacency_list<>>(test_graph);
  mapState::MapState map_state = mapState::MapState(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, graph_ptr);
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

BOOST_AUTO_TEST_CASE(Parameterized)
{
  std::cout << "Parameterized Constructor Test Started ..." << std::endl;
  mapState::MapState map_state;

  boost::adjacency_list<> test_graph = boost::adjacency_list<>();
  std::vector<StationInfo> stationsInfos = {
      Station::genData(nullptr, "StationA"),
      Station::genData(nullptr, "StationB"),
      Station::genData(nullptr, "StationC"),
  };
  auto stationA = std::make_shared<Station>("StationA", nullptr, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  auto stationB = std::make_shared<Station>("StationB", nullptr, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  auto stationC = std::make_shared<Station>("StationC", nullptr, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  std::vector<RoadInfo> roadsInfos = {
      Road::genData(stationA, stationB, 1, nullptr, cardsState::ColorCard::RED, 3),
      Road::genData(stationB, stationC, 2, nullptr, cardsState::ColorCard::BLUE, 4),
  };
  std::vector<TunnelInfo> tunnelsInfos = {
      Tunnel::genData(stationA, stationC, 3, nullptr, cardsState::ColorCard::GREEN, 5),
  };
  std::vector<FerryInfo> ferrysInfos = {
      Ferry::genData(stationC, stationA, 4, nullptr, 6, 2),
  };

  map_state.fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, map_state.gameGraph);
  BOOST_CHECK_EQUAL(map_state.stations.size(), 3);
  BOOST_CHECK_EQUAL(map_state.roads.size(), 4); // 2 roads + 1 tunnel + 1 ferry
  std::cout << "Parameterized Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(NamedMapState)
{
  std::cout << "NamedMapState Constructor Test Started ..." << std::endl;
  {
    MapState mapState = MapState::NamedMapState("test");
    BOOST_CHECK_EQUAL(mapState.stations.size(), 9); 
    BOOST_CHECK_EQUAL(mapState.roads.size(), 12);
  }
  {
    MapState mapState = MapState::NamedMapState("europe");
    BOOST_CHECK_EQUAL(mapState.stations.size(), 47); 
    BOOST_CHECK_EQUAL(mapState.roads.size(), 100);
  }
  {
    MapState mapState = MapState::NamedMapState("empty");
    BOOST_CHECK_EQUAL(mapState.stations.size(), 0);
    BOOST_CHECK_EQUAL(mapState.roads.size(), 0);
  }
  std::cout << "NamedMapState Constructor Test Finished !\n"
            << std::endl;
}


BOOST_AUTO_TEST_CASE(Test)
{
  std::cout << "Test Constructor Test Started ..." << std::endl;
  MapState mapState = MapState::Test();
  BOOST_CHECK_EQUAL(mapState.stations.size(), 9); 
  BOOST_CHECK_EQUAL(mapState.roads.size(), 12); 
  std::cout << "Test Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(Europe)
{
  std::cout << "Europe Constructor Test Started ..." << std::endl;
  std::shared_ptr<boost::adjacency_list<>> gameGraph = std::make_shared<boost::adjacency_list<>>(boost::adjacency_list<>());
    std::vector<std::string> stationNames = {
        "amsterdam",
        "angora",
        "athina",
        "barcelona",
        "berlin",
        "brest",
        "brindisi",
        "bruxelles",
        "bucuresti",
        "budapest",
        "cadiz",
        "constantinople",
        "danzig",
        "dieppe",
        "edinburgh",
        "erzurum",
        "essen",
        "frankfurt",
        "kharkov",
        "kobenhavn",
        "kyiv",
        "lisboa",
        "london",
        "madrid",
        "marseille",
        "moskva",
        "münchen",
        "palermo",
        "pamplona",
        "paris",
        "petrograd",
        "riga",
        "roma",
        "rostov",
        "sarajevo",
        "sevastopol",
        "smolensk",
        "smyrna",
        "sochi",
        "sofia",
        "stockholm",
        "venizia",
        "warszawa",
        "wien",
        "wilno",
        "zagrab",
        "zürich"};

    std::vector<StationInfo> stationsInfos;
    std::vector<std::shared_ptr<Station>> stationsObject;
    for (const std::string &name : stationNames)
    {
      stationsInfos.push_back(Station::initData(name));
    }
    stationsObject = Station::BatchConstructor(stationsInfos, gameGraph);

    std::vector<RoadInfo> roadsInfos = {
        Road::initDataByName(stationsObject, "lisboa", "madrid", 1, cardsState::ColorCard::PINK, 3),
        Road::initDataByName(stationsObject, "lisboa", "cadiz", 2, cardsState::ColorCard::BLUE, 2),
        Road::initDataByName(stationsObject, "cadiz", "madrid", 3, cardsState::ColorCard::ORANGE, 3),
        Road::initDataByName(stationsObject, "madrid", "barcelona", 4, cardsState::ColorCard::YELLOW, 2),
        Road::initDataByName(stationsObject, "barcelona", "marseille", 5, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "marseille", "paris", 6, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "marseille", "pamplona", 7, cardsState::ColorCard::RED, 4),
        Road::initDataByName(stationsObject, "pamplona", "paris", 8, cardsState::ColorCard::GREEN, 4),
        Road::initDataByName(stationsObject, "pamplona", "paris", 9, cardsState::ColorCard::BLUE, 4),
        Road::initDataByName(stationsObject, "pamplona", "brest", 10, cardsState::ColorCard::PINK, 4),
        Road::initDataByName(stationsObject, "brest", "dieppe", 11, cardsState::ColorCard::ORANGE, 2),
        Road::initDataByName(stationsObject, "brest", "paris", 12, cardsState::ColorCard::BLACK, 3),
        Road::initDataByName(stationsObject, "paris", "frankfurt", 13, cardsState::ColorCard::WHITE, 3),
        Road::initDataByName(stationsObject, "paris", "frankfurt", 14, cardsState::ColorCard::ORANGE, 3),
        Road::initDataByName(stationsObject, "paris", "bruxelles", 15, cardsState::ColorCard::YELLOW, 2),
        Road::initDataByName(stationsObject, "paris", "bruxelles", 16, cardsState::ColorCard::RED, 2),
        Road::initDataByName(stationsObject, "paris", "dieppe", 17, cardsState::ColorCard::PINK, 1),
        Road::initDataByName(stationsObject, "dieppe", "bruxelles", 18, cardsState::ColorCard::GREEN, 2),
        Road::initDataByName(stationsObject, "bruxelles", "frankfurt", 19, cardsState::ColorCard::BLUE, 2),
        Road::initDataByName(stationsObject, "bruxelles", "amsterdam", 20, cardsState::ColorCard::BLACK, 1),
        Road::initDataByName(stationsObject, "amsterdam", "frankfurt", 21, cardsState::ColorCard::WHITE, 2),
        Road::initDataByName(stationsObject, "amsterdam", "essen", 22, cardsState::ColorCard::YELLOW, 3),
        Road::initDataByName(stationsObject, "essen", "berlin", 23, cardsState::ColorCard::BLUE, 2),
        Road::initDataByName(stationsObject, "essen", "frankfurt", 24, cardsState::ColorCard::GREEN, 2),
        Road::initDataByName(stationsObject, "frankfurt", "berlin", 25, cardsState::ColorCard::BLACK, 3),
        Road::initDataByName(stationsObject, "frankfurt", "berlin", 26, cardsState::ColorCard::RED, 3),
        Road::initDataByName(stationsObject, "frankfurt", "münchen", 27, cardsState::ColorCard::PINK, 2),
        Road::initDataByName(stationsObject, "münchen", "wien", 28, cardsState::ColorCard::ORANGE, 3),
        Road::initDataByName(stationsObject, "wien", "zagrab", 29, cardsState::ColorCard::NONE, 2),
        Road::initDataByName(stationsObject, "wien", "budapest", 30, cardsState::ColorCard::RED, 1),
        Road::initDataByName(stationsObject, "wien", "budapest", 31, cardsState::ColorCard::WHITE, 1),
        Road::initDataByName(stationsObject, "wien", "berlin", 32, cardsState::ColorCard::GREEN, 3),
        Road::initDataByName(stationsObject, "wien", "warszawa", 33, cardsState::ColorCard::BLUE, 4),
        Road::initDataByName(stationsObject, "budapest", "sarajevo", 34, cardsState::ColorCard::PINK, 3),
        Road::initDataByName(stationsObject, "budapest", "zagrab", 35, cardsState::ColorCard::ORANGE, 2),
        Road::initDataByName(stationsObject, "zagrab", "sarajevo", 36, cardsState::ColorCard::RED, 3),
        Road::initDataByName(stationsObject, "zagrab", "venizia", 37, cardsState::ColorCard::NONE, 2),
        Road::initDataByName(stationsObject, "venizia", "roma", 38, cardsState::ColorCard::BLACK, 2),
        Road::initDataByName(stationsObject, "roma", "brindisi", 39, cardsState::ColorCard::WHITE, 2),
        Road::initDataByName(stationsObject, "sarajevo", "athina", 40, cardsState::ColorCard::GREEN, 4),
        Road::initDataByName(stationsObject, "athina", "sofia", 41, cardsState::ColorCard::PINK, 3),
        Road::initDataByName(stationsObject, "sofia", "constantinople", 42, cardsState::ColorCard::BLUE, 3),
        Road::initDataByName(stationsObject, "constantinople", "bucuresti", 43, cardsState::ColorCard::YELLOW, 3),
        Road::initDataByName(stationsObject, "bucuresti", "kyiv", 44, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "bucuresti", "sevastopol", 45, cardsState::ColorCard::WHITE, 4),
        Road::initDataByName(stationsObject, "sevastopol", "rostov", 46, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "rostov", "sochi", 47, cardsState::ColorCard::NONE, 2),
        Road::initDataByName(stationsObject, "rostov", "kharkov", 48, cardsState::ColorCard::GREEN, 2),
        Road::initDataByName(stationsObject, "kharkov", "kyiv", 49, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "kharkov", "moskva", 50, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "moskva", "smolensk", 51, cardsState::ColorCard::ORANGE, 2),
        Road::initDataByName(stationsObject, "moskva", "petrograd", 52, cardsState::ColorCard::WHITE, 4),
        Road::initDataByName(stationsObject, "petrograd", "wilno", 53, cardsState::ColorCard::BLUE, 4),
        Road::initDataByName(stationsObject, "petrograd", "riga", 54, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "wilno", "riga", 55, cardsState::ColorCard::GREEN, 4),
        Road::initDataByName(stationsObject, "wilno", "warszawa", 56, cardsState::ColorCard::RED, 3),
        Road::initDataByName(stationsObject, "wilno", "moskva", 57, cardsState::ColorCard::YELLOW, 3),
        Road::initDataByName(stationsObject, "wilno", "kyiv", 58, cardsState::ColorCard::NONE, 2),
        Road::initDataByName(stationsObject, "kyiv", "smolensk", 59, cardsState::ColorCard::RED, 3),
        Road::initDataByName(stationsObject, "kyiv", "warszawa", 60, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "warszawa", "berlin", 61, cardsState::ColorCard::PINK, 4),
        Road::initDataByName(stationsObject, "warszawa", "berlin", 62, cardsState::ColorCard::YELLOW, 4),
        Road::initDataByName(stationsObject, "warszawa", "danzig", 63, cardsState::ColorCard::NONE, 2),
        Road::initDataByName(stationsObject, "danzig", "berlin", 64, cardsState::ColorCard::NONE, 4),
        Road::initDataByName(stationsObject, "danzig", "riga", 65, cardsState::ColorCard::BLACK, 3),
        Road::initDataByName(stationsObject, "erzurum", "angora", 66, cardsState::ColorCard::BLACK, 3),
        Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 67, cardsState::ColorCard::WHITE, 3),
        Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 68, cardsState::ColorCard::YELLOW, 3),
        Road::initDataByName(stationsObject, "edinburgh", "london", 69, cardsState::ColorCard::BLACK, 4),
        Road::initDataByName(stationsObject, "edinburgh", "london", 70, cardsState::ColorCard::ORANGE, 4),
    };
    std::vector<TunnelInfo> tunnelsInfos = {
        Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 71, cardsState::ColorCard::BLACK, 3),
        Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 72, cardsState::ColorCard::WHITE, 3),
        Tunnel::initDataByName(stationsObject, "pamplona", "barcelona", 73, cardsState::ColorCard::NONE, 2),
        Tunnel::initDataByName(stationsObject, "zürich", "paris", 74, cardsState::ColorCard::NONE, 3),
        Tunnel::initDataByName(stationsObject, "zürich", "münchen", 75, cardsState::ColorCard::YELLOW, 2),
        Tunnel::initDataByName(stationsObject, "zürich", "marseille", 76, cardsState::ColorCard::PINK, 2),
        Tunnel::initDataByName(stationsObject, "marseille", "roma", 77, cardsState::ColorCard::NONE, 4),
        Tunnel::initDataByName(stationsObject, "zürich", "venizia", 78, cardsState::ColorCard::GREEN, 2),
        Tunnel::initDataByName(stationsObject, "venizia", "münchen", 79, cardsState::ColorCard::BLUE, 2),
        Tunnel::initDataByName(stationsObject, "petrograd", "stockholm", 80, cardsState::ColorCard::NONE, 8),
        Tunnel::initDataByName(stationsObject, "budapest", "kyiv", 81, cardsState::ColorCard::NONE, 6),
        Tunnel::initDataByName(stationsObject, "budapest", "bucuresti", 82, cardsState::ColorCard::NONE, 4),
        Tunnel::initDataByName(stationsObject, "sofia", "bucuresti", 83, cardsState::ColorCard::NONE, 2),
        Tunnel::initDataByName(stationsObject, "sofia", "sarajevo", 84, cardsState::ColorCard::NONE, 2),
        Tunnel::initDataByName(stationsObject, "constantinople", "smyrna", 85, cardsState::ColorCard::NONE, 2),
        Tunnel::initDataByName(stationsObject, "smyrna", "angora", 86, cardsState::ColorCard::ORANGE, 3),
        Tunnel::initDataByName(stationsObject, "angora", "constantinople", 87, cardsState::ColorCard::NONE, 2),
    };
    std::vector<FerryInfo> ferrysInfos = {
        Ferry::initDataByName(stationsObject, "london", "dieppe", 88, 1, 2),
        Ferry::initDataByName(stationsObject, "london", "dieppe", 89, 1, 2),
        Ferry::initDataByName(stationsObject, "london", "amsterdam", 90, 2, 2),
        Ferry::initDataByName(stationsObject, "berlin", "kobenhavn", 91, 1, 3),
        Ferry::initDataByName(stationsObject, "berlin", "kobenhavn", 92, 1, 3),
        Ferry::initDataByName(stationsObject, "sevastopol", "sochi", 93, 1, 2),
        Ferry::initDataByName(stationsObject, "sevastopol", "erzurum", 94, 2, 4),
        Ferry::initDataByName(stationsObject, "sevastopol", "constantinople", 95, 2, 4),
        Ferry::initDataByName(stationsObject, "athina", "smyrna", 96, 1, 2),
        Ferry::initDataByName(stationsObject, "athina", "brindisi", 97, 1, 4),
        Ferry::initDataByName(stationsObject, "palermo", "roma", 98, 1, 4),
        Ferry::initDataByName(stationsObject, "palermo", "brindisi", 99, 1, 3),
        Ferry::initDataByName(stationsObject, "palermo", "smyrna", 100, 2, 6),

    };

    std::vector<std::pair<std::string, std::string>> allRoadsPairs;

    for (const auto &info : ferrysInfos)
    {
      allRoadsPairs.push_back({std::get<0>(info).first->getName(), std::get<0>(info).second->getName()});
    }

    for (const auto &info : tunnelsInfos)
    {
      allRoadsPairs.push_back({std::get<0>(info).first->getName(), std::get<0>(info).second->getName()});
    }

    for (const auto &info : roadsInfos)
    {
      allRoadsPairs.push_back({std::get<0>(info).first->getName(), std::get<0>(info).second->getName()});
    }
    MapState europe_map_state = MapState::Europe();
    DEBUG_PRINT("europe_map_state stations size: " << europe_map_state.stations.size());
    DEBUG_PRINT("europe_map_state roads size: " << europe_map_state.roads.size());
    BOOST_CHECK_EQUAL(europe_map_state.stations.size(), stationNames.size());
    BOOST_CHECK_EQUAL(europe_map_state.roads.size(), allRoadsPairs.size());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)
BOOST_AUTO_TEST_CASE(GetStations)
{
  mapState::MapState test_map_state = mapState::MapState::Test();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::cout << "GetStations Test Started ..." << std::endl;
  std::vector<std::shared_ptr<mapState::Station>> stations = test_map_state.getStations();
  BOOST_CHECK_EQUAL(stations.size(), 9); // As per the default constructor in MapState
  std::cout << "GetStations Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(GetRoads)
{
  mapState::MapState test_map_state = mapState::MapState::Test();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::cout << "GetRoads Test Started ..." << std::endl;
  std::vector<std::shared_ptr<mapState::Road>> roads = test_map_state.getRoads();
  BOOST_CHECK_EQUAL(roads.size(), 12); // As per the default constructor in MapState
  std::cout << "GetRoads Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(GetStationByName)
{
  std::cout << "GetStationByName Test Started ..." << std::endl;
  {
    mapState::MapState test_map_state = MapState::Test();
#ifdef DEBUG
    test_map_state.display();
#endif
    std::cout << "\tDefined case started ..." << std::endl;
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("A");
    BOOST_CHECK(station != nullptr);
    BOOST_CHECK_EQUAL(station->getName(), "A");
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
BOOST_AUTO_TEST_CASE(GetRoadsBetweenStations)
{
  mapState::MapState test_map_state = MapState::Test();
  std::shared_ptr<mapState::Station> stationA = test_map_state.getStationByName("A");
  std::shared_ptr<mapState::Station> stationB = test_map_state.getStationByName("B");
  std::vector<std::shared_ptr<mapState::Road>> road = test_map_state.getRoadsBetweenStations(stationA, stationB);
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
  mapState::MapState test_map_state = MapState::Test();
#ifdef DEBUG
  test_map_state.display();
#endif
  {
    std::cout << "\tDefined case started ..." << std::endl;
#ifdef DEBUG
    test_map_state.display();
#endif
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("B");
    std::vector<std::shared_ptr<mapState::Station>> adjacentStations = test_map_state.getAdjacentStations(station);
    BOOST_CHECK_EQUAL(adjacentStations.size(), 5); // paris is connected to berlin and madrid
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
// FIXME : make the test WORK @fetohiaras
BOOST_AUTO_TEST_CASE(FindShortestPath)
{
  mapState::MapState test_map_state = mapState::MapState::Test();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::cout << "FindShortestPath Test Started ..." << std::endl;
  std::shared_ptr<mapState::Station> src = test_map_state.getStationByName("paris");
  std::shared_ptr<mapState::Station> dest = test_map_state.getStationByName("rome");
  Path shortestPath = test_map_state.findShortestPath(src, dest);
  std::cout << "Shortest path length: " << shortestPath.TOTALLENGTH << std::endl;
#ifdef DEBUG
  std::cout << "Shortest path from " << src->getName() << " to " << dest->getName() << ":\n";
  for (const std::shared_ptr<Station> &station : shortestPath.STATIONS)
  {
    std::cout << station->getName() << " ";
  }
  std::cout << "\nTotal Length: " << shortestPath.TOTALLENGTH << "\n";
  std::cout << "Number of Edges: " << shortestPath.NUMEDGES << "\n";
#endif
#ifdef DEBUG
  BOOST_CHECK(!shortestPath.STATIONS.empty());
  BOOST_CHECK_EQUAL(shortestPath.STATIONS.front(), src);
  BOOST_CHECK_EQUAL(shortestPath.STATIONS.back(), dest);
  std::cout << "FindShortestPath Test Finished !\n"
            << std::endl;
#endif
}

BOOST_AUTO_TEST_CASE(GetClaimableRoads)
{
  std::cout << "GetClaimableRoads Test Started ..." << std::endl;
  mapState::MapState test_map_state;
  playersState::Player test_player = playersState::Player("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, 6, nullptr);
  StationInfo stationAInfo = Station::initData("StationA");
  StationInfo stationBInfo = Station::initData("StationB");
  StationInfo stationCInfo = Station::initData("StationC");
  StationInfo stationDInfo = Station::initData("StationD");
  std::vector<StationInfo> stationsInfos = {stationAInfo, stationBInfo, stationCInfo, stationDInfo};
  std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationsInfos, test_map_state.gameGraph);
  RoadInfo roadABInfo = Road::genDataByName(stations, "StationA", "StationB", 1, std::make_shared<playersState::Player>(test_player), cardsState::ColorCard::RED, 3);
  RoadInfo roadACInfo = Road::genDataByName(stations, "StationA", "StationC", 2, nullptr, cardsState::BLUE, 4);
  RoadInfo roadADInfo = Road::genDataByName(stations, "StationA", "StationD", 3, nullptr, cardsState::GREEN, 2);
  std::vector<RoadInfo> roadsInfos = {roadABInfo, roadACInfo, roadADInfo};
  test_map_state.fillMapWithInfos(stationsInfos, roadsInfos, {}, {}, test_map_state.gameGraph);
  std::vector<std::shared_ptr<Road>> claimableRoads = test_map_state.getClaimableRoads(4, std::make_shared<playersState::Player>(test_player));
  BOOST_CHECK_EQUAL(claimableRoads.size(), 2); // roadAC and roadAD should be claimable
  std::cout << "GetClaimableRoads Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(IsRoadClaimable)
{
  std::cout << "IsRoadClaimable Test Started ..." << std::endl;
  mapState::MapState test_map_state;
  playersState::Player test_player = playersState::Player("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, 6, nullptr);
  StationInfo stationAInfo = Station::initData("StationA");
  StationInfo stationBInfo = Station::initData("StationB");
  StationInfo stationCInfo = Station::initData("StationC");
  StationInfo stationDInfo = Station::initData("StationD");
  std::vector<StationInfo> stationsInfos = {stationAInfo, stationBInfo, stationCInfo, stationDInfo};
  std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationsInfos, test_map_state.gameGraph);
  RoadInfo roadABInfo = Road::genDataByName(stations, "StationA", "StationB", 1, std::make_shared<playersState::Player>(test_player), cardsState::ColorCard::RED, 3);
  RoadInfo roadACInfo = Road::genDataByName(stations, "StationA", "StationC", 2, nullptr, cardsState::BLUE, 4);
  RoadInfo roadADInfo = Road::genDataByName(stations, "StationA", "StationD", 3, nullptr, cardsState::GREEN, 2);
  std::vector<RoadInfo> roadsInfos = {roadABInfo, roadACInfo, roadADInfo};
  test_map_state.fillMapWithInfos(stationsInfos, roadsInfos, {}, {}, test_map_state.gameGraph);
  std::shared_ptr<Road> roadAC = test_map_state.getRoadsBetweenStations(stations[0], stations[2])[0];
  bool isClaimable = test_map_state.isRoadClaimable(4, roadAC, std::make_shared<playersState::Player>(test_player));
  BOOST_CHECK_EQUAL(isClaimable, true); // roadAC should be claimable
  std::cout << "IsRoadClaimable Test Finished !\n"
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
