#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/MapState.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/playersState/Player.h"
#include "../../src/shared/mapState/Tunnel.h"
#include "../../src/shared/mapState/Ferry.h"
#include "../../src/shared/cardsState/DestinationCard.h"
#include <boost/graph/adjacency_list.hpp>
#include <memory>

#define DEBUG_MODE true
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

#define TEST(x) BOOST_AUTO_TEST_CASE(x)
#define SUITE_START(x) BOOST_AUTO_TEST_SUITE(x)
#define SUITE_END() BOOST_AUTO_TEST_SUITE_END()
#define ANN_START(x) std::cout << "Starting " << x << " test..." << std::endl;
#define ANN_END(x) std::cout << x << " test finished!" << std::endl;
#define CHECK_EQ(a, b) BOOST_CHECK_EQUAL(a, b)
#define CHECK_NE(a, b) BOOST_CHECK_NE(a, b)
#define CHECK_NTHROW(...) BOOST_CHECK_NO_THROW(__VA_ARGS__)
#define CHECK_THROW(...) BOOST_CHECK_THROW(__VA_ARGS__)
#define REQUIRE(...) BOOST_REQUIRE(__VA_ARGS__)
#define CHECK(x) BOOST_CHECK(x)

using namespace ::mapState;

using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;
using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, RoadColor, int>;
using RoadInfo = std::pair<StationPair, RoadDetail>;
using TunnelDetail = RoadDetail;
using TunnelInfo = RoadInfo;
using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, int, int>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

std::vector<std::shared_ptr<Road>> borrowedRoads;

TEST(TestStaticAssert)
{
  CHECK(1);
}

SUITE_START(Constructors)

TEST(Default)
{
  ANN_START("Default Constructor")
  mapState::MapState map_state = mapState::MapState();
#ifdef DEBUG
  map_state.display();
#endif
  CHECK(map_state.gameGraph != nullptr);
  CHECK_EQ(map_state.stations.size(), 0);
  CHECK_EQ(map_state.roads.size(), 0);
  ANN_END("Default Constructor")
}

TEST(Wrapper)
{
  ANN_START("Wrapper Constructor")
  boost::adjacency_list<> test_graph = boost::adjacency_list<>();
  std::vector<StationInfo> stationsInfos = {
      Station::genData(nullptr, "Station1"),
      Station::genData(nullptr, "Station2"),
  };
  auto station1 = std::make_shared<Station>("Station1", nullptr, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  auto station2 = std::make_shared<Station>("Station2", nullptr, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(test_graph)));
  std::vector<RoadInfo> roadsInfos = {
      Road::genData(station1, station2, 1, nullptr, RoadColor::RED, 3),
  };
  std::vector<TunnelInfo> tunnelsInfos = {
      Tunnel::genData(station2, station1, 2, nullptr, RoadColor::BLUE, 4),
  };
  std::vector<FerryInfo> ferrysInfos = {
      Ferry::genData(station1, station2, 3, nullptr, 5, 1),
  };
  std::shared_ptr<boost::adjacency_list<>> graph_ptr = std::make_shared<boost::adjacency_list<>>(test_graph);
  mapState::MapState map_state = mapState::MapState(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, graph_ptr);
  CHECK_EQ(map_state.stations.size(), 2);
  CHECK_EQ(map_state.roads.size(), 3); // 1 road + 1 tunnel + 1 ferry
  for (int i = 0; i < static_cast<int>(map_state.stations.size()); i++)
  {
    CHECK(map_state.stations[i]->name == "Station1" || map_state.stations[i]->name == "Station2");
  }
  for (int i = 0; i < static_cast<int>(map_state.roads.size()); i++)
  {
    const std::string &stationAName = map_state.roads[i]->stationA->getName();
    const std::string &stationBName = map_state.roads[i]->stationB->getName();
    CHECK(stationAName == station1->getName() || stationAName == station2->getName());
    CHECK(stationBName == station1->getName() || stationBName == station2->getName());
  }
  ANN_END("Wrapper Constructor")
}

TEST(Parameterized)
{
  ANN_START("Parameterized Constructor")
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
      Road::genData(stationA, stationB, 1, nullptr, RoadColor::RED, 3),
      Road::genData(stationB, stationC, 2, nullptr, RoadColor::BLUE, 4),
  };
  std::vector<TunnelInfo> tunnelsInfos = {
      Tunnel::genData(stationA, stationC, 3, nullptr, RoadColor::GREEN, 5),
  };
  std::vector<FerryInfo> ferrysInfos = {
      Ferry::genData(stationC, stationA, 4, nullptr, 6, 2),
  };

  map_state.fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, map_state.gameGraph);
  CHECK_EQ(map_state.stations.size(), 3);
  CHECK_EQ(map_state.roads.size(), 4); // 2 roads + 1 tunnel + 1 ferry
  ANN_END("Parameterized Constructor")
}

TEST(NamedMapState)
{
  ANN_START("NamedMapState Constructor")
  {
    MapState mapState = MapState::NamedMapState("test");
    CHECK_EQ(mapState.stations.size(), 9); 
    CHECK_EQ(mapState.roads.size(), 12);
  }
  {
    MapState mapState = MapState::NamedMapState("europe");
    CHECK_EQ(mapState.stations.size(), 47); 
    CHECK_EQ(mapState.roads.size(), 100);
  }
  {
    MapState mapState = MapState::NamedMapState("empty");
    CHECK_EQ(mapState.stations.size(), 0);
    CHECK_EQ(mapState.roads.size(), 0);
  }
  {
    MapState mapState = MapState::NamedMapState("nonexistent_map");
    CHECK_EQ(mapState.stations.size(), 0);
    CHECK_EQ(mapState.roads.size(), 0);
  }
  ANN_END("NamedMapState Constructor")
}


TEST(Test)
{
  ANN_START("Test Constructor")
  MapState mapState = MapState::Test();
  CHECK_EQ(mapState.stations.size(), 9); 
  CHECK_EQ(mapState.roads.size(), 12); 
  ANN_END("Test Constructor")
}

TEST(Europe)
{
  ANN_START("Europe Constructor")
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
        Road::initDataByName(stationsObject, "lisboa", "madrid", 1, RoadColor::PINK, 3),
        Road::initDataByName(stationsObject, "lisboa", "cadiz", 2, RoadColor::BLUE, 2),
        Road::initDataByName(stationsObject, "cadiz", "madrid", 3, RoadColor::ORANGE, 3),
        Road::initDataByName(stationsObject, "madrid", "barcelona", 4, RoadColor::YELLOW, 2),
        Road::initDataByName(stationsObject, "barcelona", "marseille", 5, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "marseille", "paris", 6, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "marseille", "pamplona", 7, RoadColor::RED, 4),
        Road::initDataByName(stationsObject, "pamplona", "paris", 8, RoadColor::GREEN, 4),
        Road::initDataByName(stationsObject, "pamplona", "paris", 9, RoadColor::BLUE, 4),
        Road::initDataByName(stationsObject, "pamplona", "brest", 10, RoadColor::PINK, 4),
        Road::initDataByName(stationsObject, "brest", "dieppe", 11, RoadColor::ORANGE, 2),
        Road::initDataByName(stationsObject, "brest", "paris", 12, RoadColor::BLACK, 3),
        Road::initDataByName(stationsObject, "paris", "frankfurt", 13, RoadColor::WHITE, 3),
        Road::initDataByName(stationsObject, "paris", "frankfurt", 14, RoadColor::ORANGE, 3),
        Road::initDataByName(stationsObject, "paris", "bruxelles", 15, RoadColor::YELLOW, 2),
        Road::initDataByName(stationsObject, "paris", "bruxelles", 16, RoadColor::RED, 2),
        Road::initDataByName(stationsObject, "paris", "dieppe", 17, RoadColor::PINK, 1),
        Road::initDataByName(stationsObject, "dieppe", "bruxelles", 18, RoadColor::GREEN, 2),
        Road::initDataByName(stationsObject, "bruxelles", "frankfurt", 19, RoadColor::BLUE, 2),
        Road::initDataByName(stationsObject, "bruxelles", "amsterdam", 20, RoadColor::BLACK, 1),
        Road::initDataByName(stationsObject, "amsterdam", "frankfurt", 21, RoadColor::WHITE, 2),
        Road::initDataByName(stationsObject, "amsterdam", "essen", 22, RoadColor::YELLOW, 3),
        Road::initDataByName(stationsObject, "essen", "berlin", 23, RoadColor::BLUE, 2),
        Road::initDataByName(stationsObject, "essen", "frankfurt", 24, RoadColor::GREEN, 2),
        Road::initDataByName(stationsObject, "frankfurt", "berlin", 25, RoadColor::BLACK, 3),
        Road::initDataByName(stationsObject, "frankfurt", "berlin", 26, RoadColor::RED, 3),
        Road::initDataByName(stationsObject, "frankfurt", "münchen", 27, RoadColor::PINK, 2),
        Road::initDataByName(stationsObject, "münchen", "wien", 28, RoadColor::ORANGE, 3),
        Road::initDataByName(stationsObject, "wien", "zagrab", 29, RoadColor::NONE, 2),
        Road::initDataByName(stationsObject, "wien", "budapest", 30, RoadColor::RED, 1),
        Road::initDataByName(stationsObject, "wien", "budapest", 31, RoadColor::WHITE, 1),
        Road::initDataByName(stationsObject, "wien", "berlin", 32, RoadColor::GREEN, 3),
        Road::initDataByName(stationsObject, "wien", "warszawa", 33, RoadColor::BLUE, 4),
        Road::initDataByName(stationsObject, "budapest", "sarajevo", 34, RoadColor::PINK, 3),
        Road::initDataByName(stationsObject, "budapest", "zagrab", 35, RoadColor::ORANGE, 2),
        Road::initDataByName(stationsObject, "zagrab", "sarajevo", 36, RoadColor::RED, 3),
        Road::initDataByName(stationsObject, "zagrab", "venizia", 37, RoadColor::NONE, 2),
        Road::initDataByName(stationsObject, "venizia", "roma", 38, RoadColor::BLACK, 2),
        Road::initDataByName(stationsObject, "roma", "brindisi", 39, RoadColor::WHITE, 2),
        Road::initDataByName(stationsObject, "sarajevo", "athina", 40, RoadColor::GREEN, 4),
        Road::initDataByName(stationsObject, "athina", "sofia", 41, RoadColor::PINK, 3),
        Road::initDataByName(stationsObject, "sofia", "constantinople", 42, RoadColor::BLUE, 3),
        Road::initDataByName(stationsObject, "constantinople", "bucuresti", 43, RoadColor::YELLOW, 3),
        Road::initDataByName(stationsObject, "bucuresti", "kyiv", 44, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "bucuresti", "sevastopol", 45, RoadColor::WHITE, 4),
        Road::initDataByName(stationsObject, "sevastopol", "rostov", 46, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "rostov", "sochi", 47, RoadColor::NONE, 2),
        Road::initDataByName(stationsObject, "rostov", "kharkov", 48, RoadColor::GREEN, 2),
        Road::initDataByName(stationsObject, "kharkov", "kyiv", 49, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "kharkov", "moskva", 50, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "moskva", "smolensk", 51, RoadColor::ORANGE, 2),
        Road::initDataByName(stationsObject, "moskva", "petrograd", 52, RoadColor::WHITE, 4),
        Road::initDataByName(stationsObject, "petrograd", "wilno", 53, RoadColor::BLUE, 4),
        Road::initDataByName(stationsObject, "petrograd", "riga", 54, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "wilno", "riga", 55, RoadColor::GREEN, 4),
        Road::initDataByName(stationsObject, "wilno", "warszawa", 56, RoadColor::RED, 3),
        Road::initDataByName(stationsObject, "wilno", "moskva", 57, RoadColor::YELLOW, 3),
        Road::initDataByName(stationsObject, "wilno", "kyiv", 58, RoadColor::NONE, 2),
        Road::initDataByName(stationsObject, "kyiv", "smolensk", 59, RoadColor::RED, 3),
        Road::initDataByName(stationsObject, "kyiv", "warszawa", 60, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "warszawa", "berlin", 61, RoadColor::PINK, 4),
        Road::initDataByName(stationsObject, "warszawa", "berlin", 62, RoadColor::YELLOW, 4),
        Road::initDataByName(stationsObject, "warszawa", "danzig", 63, RoadColor::NONE, 2),
        Road::initDataByName(stationsObject, "danzig", "berlin", 64, RoadColor::NONE, 4),
        Road::initDataByName(stationsObject, "danzig", "riga", 65, RoadColor::BLACK, 3),
        Road::initDataByName(stationsObject, "erzurum", "angora", 66, RoadColor::BLACK, 3),
        Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 67, RoadColor::WHITE, 3),
        Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 68, RoadColor::YELLOW, 3),
        Road::initDataByName(stationsObject, "edinburgh", "london", 69, RoadColor::BLACK, 4),
        Road::initDataByName(stationsObject, "edinburgh", "london", 70, RoadColor::ORANGE, 4),
    };
    std::vector<TunnelInfo> tunnelsInfos = {
        Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 71, RoadColor::BLACK, 3),
        Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 72, RoadColor::WHITE, 3),
        Tunnel::initDataByName(stationsObject, "pamplona", "barcelona", 73, RoadColor::NONE, 2),
        Tunnel::initDataByName(stationsObject, "zürich", "paris", 74, RoadColor::NONE, 3),
        Tunnel::initDataByName(stationsObject, "zürich", "münchen", 75, RoadColor::YELLOW, 2),
        Tunnel::initDataByName(stationsObject, "zürich", "marseille", 76, RoadColor::PINK, 2),
        Tunnel::initDataByName(stationsObject, "marseille", "roma", 77, RoadColor::NONE, 4),
        Tunnel::initDataByName(stationsObject, "zürich", "venizia", 78, RoadColor::GREEN, 2),
        Tunnel::initDataByName(stationsObject, "venizia", "münchen", 79, RoadColor::BLUE, 2),
        Tunnel::initDataByName(stationsObject, "petrograd", "stockholm", 80, RoadColor::NONE, 8),
        Tunnel::initDataByName(stationsObject, "budapest", "kyiv", 81, RoadColor::NONE, 6),
        Tunnel::initDataByName(stationsObject, "budapest", "bucuresti", 82, RoadColor::NONE, 4),
        Tunnel::initDataByName(stationsObject, "sofia", "bucuresti", 83, RoadColor::NONE, 2),
        Tunnel::initDataByName(stationsObject, "sofia", "sarajevo", 84, RoadColor::NONE, 2),
        Tunnel::initDataByName(stationsObject, "constantinople", "smyrna", 85, RoadColor::NONE, 2),
        Tunnel::initDataByName(stationsObject, "smyrna", "angora", 86, RoadColor::ORANGE, 3),
        Tunnel::initDataByName(stationsObject, "angora", "constantinople", 87, RoadColor::NONE, 2),
    };
    std::vector<FerryInfo> ferrysInfos = {
        Ferry::initDataByName(stationsObject, "london", "dieppe", 88, 1, 2),
        Ferry::initDataByName(stationsObject, "london", "dieppe", 89, 1, 2),
        Ferry::initDataByName(stationsObject, "london", "amsterdam", 90, 2, 2),
        Ferry::initDataByName(stationsObject, "berlin", "essen", 91, 1, 3),
        Ferry::initDataByName(stationsObject, "berlin", "essen", 92, 1, 3),
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
    CHECK_EQ(europe_map_state.stations.size(), stationNames.size());
    CHECK_EQ(europe_map_state.roads.size(), allRoadsPairs.size());
    ANN_END("Europe Constructor")
}

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)

SUITE_START(Getters)
TEST(getStations)
{
  ANN_START("getStations")
  mapState::MapState test_map_state = mapState::MapState::Test();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::vector<std::shared_ptr<mapState::Station>> stations = test_map_state.getStations();
  CHECK_EQ(stations.size(), 9); // As per the default constructor in MapState
  ANN_END("getStations")
}
TEST(getRoads)
{
  ANN_START("getRoads")
  mapState::MapState test_map_state = mapState::MapState::Test();
#ifdef DEBUG
  test_map_state.display();
#endif
  std::vector<std::shared_ptr<mapState::Road>> roads = test_map_state.getRoads();
  CHECK_EQ(roads.size(), 12); // As per the default constructor in MapState
  ANN_END("getRoads")
}
TEST(getStationByName)
{
  ANN_START("getStationByName")
  {
    ANN_START("defined case")
    mapState::MapState test_map_state = MapState::Test();
#ifdef DEBUG
    test_map_state.display();
#endif
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("A");
    CHECK(station != nullptr);
    CHECK_EQ(station->getName(), "A");
    ANN_END("defined case")
  }
  {
    ANN_START("undefined case")
    mapState::MapState test_map_state = mapState::MapState();
#ifdef DEBUG
    test_map_state.display();
#endif
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("nonexistent");
    CHECK(station == nullptr);
    ANN_END("undefined case")
  }
  ANN_END("getStationByName")
}

SUITE_END() // Getters

SUITE_START(Setters)

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)

TEST(getRoadsBetweenStations)
{
  ANN_START("getRoadsBetweenStations")
  mapState::MapState test_map_state = MapState::Test();
  std::shared_ptr<mapState::Station> stationA = test_map_state.getStationByName("A");
  std::shared_ptr<mapState::Station> stationB = test_map_state.getStationByName("B");
  std::vector<std::shared_ptr<mapState::Road>> road = test_map_state.getRoadsBetweenStations(stationA, stationB);
  CHECK_NE(road.empty(), true);
  for (const std::shared_ptr<mapState::Road> &road : road)
  {
    CHECK_EQ(road->stationA->name, stationA->name);
    CHECK_EQ(road->stationB->name, stationB->name);
  }
  ANN_END("getRoadsBetweenStations")
}

TEST(getAdjacentStations)
{
  ANN_START("getAdjacentStations")
  mapState::MapState test_map_state = MapState::Test();
#ifdef DEBUG
  test_map_state.display();
#endif
  {
    ANN_START("defined case")
#ifdef DEBUG
    test_map_state.display();
#endif
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("B");
    std::vector<std::shared_ptr<mapState::Station>> adjacentStations = test_map_state.getAdjacentStations(station);
    CHECK_EQ(adjacentStations.size(), 5); // paris is connected to berlin and madrid
    ANN_END("defined case")
  }
  {
    ANN_START("undefined case")
    std::shared_ptr<mapState::Station> station = test_map_state.getStationByName("lol");
    std::vector<std::shared_ptr<mapState::Station>> adjacentStations = test_map_state.getAdjacentStations(station);
    // CHECK_EQ(adjacentStations.size(), 0); // lol does not exist = no adjacent stations
    ANN_END("undefined case")
  }
  ANN_END("getAdjacentStations")
}
// check shortest path function in a small synthetic map
TEST(getShortestPath)
{
  ANN_START("getShortestPath")
  std::shared_ptr<boost::adjacency_list<>> gameGraph =
      std::make_shared<boost::adjacency_list<>>();
  mapState::MapState test_map_state;

  std::vector<StationInfo> stationsInfos = {
      Station::initData("A"),
      Station::initData("B"),
      Station::initData("C"),
  };

  std::vector<std::shared_ptr<Station>> stations =
      Station::BatchConstructor(stationsInfos, gameGraph);

  std::vector<RoadInfo> roadsInfos = {
      Road::initDataByName(stations, "A", "B", 1, RoadColor::NONE, 5),
      Road::initDataByName(stations, "B", "C", 2, RoadColor::NONE, 5),
      Road::initDataByName(stations, "A", "C", 3, RoadColor::NONE, 20),
  };

  test_map_state.fillMapWithInfos(stationsInfos, roadsInfos, {}, {}, gameGraph);

  std::shared_ptr<mapState::Station> src = test_map_state.getStationByName("A");
  std::shared_ptr<mapState::Station> dest = test_map_state.getStationByName("C");
  std::vector<std::shared_ptr<Station>> mapStations = test_map_state.getStations();
  std::vector<std::shared_ptr<Road>> mapRoads = test_map_state.getRoads();
  Path shortestPath = test_map_state.getShortestPath(src, dest, mapStations, mapRoads);

  CHECK(!shortestPath.STATIONS.empty());
  CHECK_EQ(shortestPath.STATIONS.front(), src);
  CHECK_EQ(shortestPath.STATIONS.back(), dest);
  CHECK_EQ(shortestPath.NUMEDGES, 1);
  CHECK_EQ(shortestPath.TOTALLENGTH, 1);
  ANN_END("getShortestPath")
}
// check most efficient path function in a small synthetic map
TEST(getMostEfficientPath)
{
  ANN_START("getMostEfficientPath")
  std::shared_ptr<boost::adjacency_list<>> gameGraph =
      std::make_shared<boost::adjacency_list<>>();
  mapState::MapState test_map_state;

  std::vector<StationInfo> stationsInfos = {
      Station::initData("A"),
      Station::initData("B"),
      Station::initData("C"),
  };

  std::vector<std::shared_ptr<Station>> stations =
      Station::BatchConstructor(stationsInfos, gameGraph);

  std::vector<RoadInfo> roadsInfos = {
      Road::initDataByName(stations, "A", "B", 1, RoadColor::NONE, 5),
      Road::initDataByName(stations, "B", "C", 2, RoadColor::NONE, 5),
      Road::initDataByName(stations, "A", "C", 3, RoadColor::NONE, 20),
  };

  test_map_state.fillMapWithInfos(stationsInfos, roadsInfos, {}, {}, gameGraph);

  std::shared_ptr<mapState::Station> src = test_map_state.getStationByName("A");
  std::shared_ptr<mapState::Station> dest = test_map_state.getStationByName("C");
  std::vector<std::shared_ptr<Station>> mapStations = test_map_state.getStations();
  std::vector<std::shared_ptr<Road>> mapRoads = test_map_state.getRoads();
  Path bestPath = test_map_state.getMostEfficientPath(src, dest, mapStations, mapRoads);

  CHECK(!bestPath.STATIONS.empty());
  CHECK_EQ(bestPath.STATIONS.front(), src);
  CHECK_EQ(bestPath.STATIONS.back(), dest);
  CHECK_EQ(bestPath.NUMEDGES, 2);
  CHECK_EQ(bestPath.TOTALLENGTH, 10);
  ANN_END("getMostEfficientPath")
}
// check shortest path using examples from the europe map
TEST(getShortestPathEuropePairs)
{
  ANN_START("getShortestPathEuropePairs")
  mapState::MapState map_state = mapState::MapState::Europe();
  std::vector<std::shared_ptr<Station>> mapStations = map_state.getStations();
  std::vector<std::shared_ptr<Road>> mapRoads = map_state.getRoads();

  std::vector<std::pair<std::string, std::string>> pairs = {
      {"paris", "berlin"},
      {"lisboa", "madrid"},
      {"frankfurt", "wien"},
      {"roma", "athina"},
      {"moskva", "kyiv"},
  };

  for (const std::pair<std::string, std::string> &pair : pairs)
  {
    DEBUG_PRINT("Testing shortest path: " << pair.first << " -> " << pair.second);
    std::shared_ptr<Station> src = map_state.getStationByName(pair.first);
    std::shared_ptr<Station> dest = map_state.getStationByName(pair.second);
    if (!src)
    {
      DEBUG_PRINT("Source station not found: " << pair.first);
    }
    if (!dest)
    {
      DEBUG_PRINT("Destination station not found: " << pair.second);
    }
    REQUIRE(src != nullptr);
    REQUIRE(dest != nullptr);

    Path shortestPath = map_state.getShortestPath(src, dest, mapStations, mapRoads);
    if (shortestPath.STATIONS.empty())
    {
      DEBUG_PRINT("No path found. Stations: " << mapStations.size()
                  << " Roads: " << mapRoads.size());
    }
    else
    {
      DEBUG_PRINT("Path length: " << shortestPath.TOTALLENGTH
                  << " edges: " << shortestPath.NUMEDGES);
      std::ostringstream pathOut;
      pathOut << "Path: ";
      for (const std::shared_ptr<Station> &station : shortestPath.STATIONS)
      {
        if (station)
        {
          pathOut << station->getName() << " ";
        }
      }
      DEBUG_PRINT(pathOut.str());
    }

    REQUIRE(!shortestPath.STATIONS.empty());
    CHECK_EQ(shortestPath.STATIONS.front(), src);
    CHECK_EQ(shortestPath.STATIONS.back(), dest);
    CHECK(shortestPath.NUMEDGES > 0);
    CHECK(shortestPath.TOTALLENGTH > 0);
    CHECK_EQ(shortestPath.NUMEDGES, static_cast<int>(shortestPath.STATIONS.size()) - 1);
  }

  ANN_END("getShortestPathEuropePairs")
}

// check most efficient path using examples from the europe map
TEST(getMostEfficientPathEuropePairs)
{
  ANN_START("getMostEfficientPathEuropePairs")
  mapState::MapState map_state = mapState::MapState::Europe();
  std::vector<std::shared_ptr<Station>> mapStations = map_state.getStations();
  std::vector<std::shared_ptr<Road>> mapRoads = map_state.getRoads();

  std::vector<std::pair<std::string, std::string>> pairs = {
      {"paris", "berlin"},
      {"lisboa", "madrid"},
      {"frankfurt", "wien"},
      {"roma", "athina"},
      {"moskva", "kyiv"},
  };

  for (const std::pair<std::string, std::string> &pair : pairs)
  {
    DEBUG_PRINT("Testing most efficient path: " << pair.first << " -> " << pair.second);
    std::shared_ptr<Station> src = map_state.getStationByName(pair.first);
    std::shared_ptr<Station> dest = map_state.getStationByName(pair.second);
    if (!src)
    {
      DEBUG_PRINT("Source station not found: " << pair.first);
    }
    if (!dest)
    {
      DEBUG_PRINT("Destination station not found: " << pair.second);
    }
    REQUIRE(src != nullptr);
    REQUIRE(dest != nullptr);

    Path bestPath = map_state.getMostEfficientPath(src, dest, mapStations, mapRoads);
    if (bestPath.STATIONS.empty())
    {
      DEBUG_PRINT("No path found. Stations: " << mapStations.size()
                  << " Roads: " << mapRoads.size());
    }
    else
    {
      DEBUG_PRINT("Path length: " << bestPath.TOTALLENGTH
                  << " edges: " << bestPath.NUMEDGES);
      std::ostringstream pathOut;
      pathOut << "Path: ";
      for (const std::shared_ptr<Station> &station : bestPath.STATIONS)
      {
        if (station)
        {
          pathOut << station->getName() << " ";
        }
      }
      DEBUG_PRINT(pathOut.str());
    }

    REQUIRE(!bestPath.STATIONS.empty());
    CHECK_EQ(bestPath.STATIONS.front(), src);
    CHECK_EQ(bestPath.STATIONS.back(), dest);
    CHECK(bestPath.NUMEDGES > 0);
    CHECK(bestPath.TOTALLENGTH > 0);
    CHECK_EQ(bestPath.NUMEDGES, static_cast<int>(bestPath.STATIONS.size()) - 1);
  }

  ANN_END("getMostEfficientPathEuropePairs")
}

// check isDestinationReached functions
TEST(isDestinationReachedStatic)
{
  ANN_START("isDestinationReachedStatic")
  mapState::MapState map_state = mapState::MapState::Europe();
  std::shared_ptr<Station> lisboa = map_state.getStationByName("lisboa");
  std::shared_ptr<Station> madrid = map_state.getStationByName("madrid");
  std::shared_ptr<Station> paris = map_state.getStationByName("paris");
  REQUIRE(lisboa != nullptr);
  REQUIRE(madrid != nullptr);
  REQUIRE(paris != nullptr);

  std::vector<std::shared_ptr<Road>> lisboaMadrid = map_state.getRoadsBetweenStations(lisboa, madrid);
  REQUIRE(!lisboaMadrid.empty());

  CHECK(MapState::isDestinationReached(lisboaMadrid, lisboa, madrid));
  CHECK(!MapState::isDestinationReached(lisboaMadrid, lisboa, paris));
  ANN_END("isDestinationReachedStatic")
}

// check isDestinationReached calls via player
TEST(isDestinationReachedPlayer)
{
  ANN_START("isDestinationReachedPlayer")
  mapState::MapState map_state = mapState::MapState::Europe();
  std::shared_ptr<Station> lisboa = map_state.getStationByName("lisboa");
  std::shared_ptr<Station> madrid = map_state.getStationByName("madrid");
  std::shared_ptr<Station> paris = map_state.getStationByName("paris");
  REQUIRE(lisboa != nullptr);
  REQUIRE(madrid != nullptr);
  REQUIRE(paris != nullptr);

  std::vector<std::shared_ptr<Road>> lisboaMadrid = map_state.getRoadsBetweenStations(lisboa, madrid);
  REQUIRE(!lisboaMadrid.empty());

  std::shared_ptr<playersState::Player> player =
      std::make_shared<playersState::Player>("Tester", playersState::PlayerColor::RED, 0, 0, 0, lisboaMadrid, nullptr);

  CHECK(map_state.isDestinationReached(player, lisboa, madrid));
  CHECK(!map_state.isDestinationReached(player, lisboa, paris));
  ANN_END("isDestinationReachedPlayer")
}

// check isDestinationReached calls via destination card
TEST(isDestinationReachedDestinationCard)
{
  ANN_START("isDestinationReachedDestinationCard")
  mapState::MapState map_state = mapState::MapState::Europe();
  std::shared_ptr<Station> lisboa = map_state.getStationByName("lisboa");
  std::shared_ptr<Station> madrid = map_state.getStationByName("madrid");
  std::shared_ptr<Station> paris = map_state.getStationByName("paris");
  REQUIRE(lisboa != nullptr);
  REQUIRE(madrid != nullptr);
  REQUIRE(paris != nullptr);

  std::vector<std::shared_ptr<Road>> lisboaMadrid = map_state.getRoadsBetweenStations(lisboa, madrid);
  REQUIRE(!lisboaMadrid.empty());

  std::shared_ptr<playersState::Player> player =
      std::make_shared<playersState::Player>("Tester", playersState::PlayerColor::RED, 0, 0, 0, lisboaMadrid, nullptr);

  std::shared_ptr<cardsState::DestinationCard> reachableCard =
      std::make_shared<cardsState::DestinationCard>(lisboa, madrid, 0, false);
  std::shared_ptr<cardsState::DestinationCard> unreachableCard =
      std::make_shared<cardsState::DestinationCard>(lisboa, paris, 0, false);

  CHECK(map_state.isDestinationReached(player, reachableCard));
  CHECK(!map_state.isDestinationReached(player, unreachableCard));
  ANN_END("isDestinationReachedDestinationCard")
}

TEST(getClaimableRoads)
{
  ANN_START("getClaimableRoads")
  mapState::MapState test_map_state;
  playersState::Player test_player = playersState::Player("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, borrowedRoads, nullptr);
  StationInfo stationAInfo = Station::initData("StationA");
  StationInfo stationBInfo = Station::initData("StationB");
  StationInfo stationCInfo = Station::initData("StationC");
  StationInfo stationDInfo = Station::initData("StationD");
  std::vector<StationInfo> stationsInfos = {stationAInfo, stationBInfo, stationCInfo, stationDInfo};
  std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationsInfos, test_map_state.gameGraph);
  RoadInfo roadABInfo = Road::genDataByName(stations, "StationA", "StationB", 1, std::make_shared<playersState::Player>(test_player), RoadColor::RED, 3);
  RoadInfo roadACInfo = Road::genDataByName(stations, "StationA", "StationC", 2, nullptr, RoadColor::BLUE, 4);
  RoadInfo roadADInfo = Road::genDataByName(stations, "StationA", "StationD", 3, nullptr, RoadColor::GREEN, 2);
  std::vector<RoadInfo> roadsInfos = {roadABInfo, roadACInfo, roadADInfo};
  test_map_state.fillMapWithInfos(stationsInfos, roadsInfos, {}, {}, test_map_state.gameGraph);
  std::vector<std::shared_ptr<Road>> claimableRoads = test_map_state.getClaimableRoads(4, std::make_shared<playersState::Player>(test_player));
  CHECK_EQ(claimableRoads.size(), 2); // roadAC and roadAD should be claimable
  ANN_END("getClaimableRoads")
}

TEST(isRoadClaimable)
{
  ANN_START("isRoadClaimable")
  mapState::MapState test_map_state;
  playersState::Player test_player = playersState::Player("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, borrowedRoads, nullptr);
  StationInfo stationAInfo = Station::initData("StationA");
  StationInfo stationBInfo = Station::initData("StationB");
  StationInfo stationCInfo = Station::initData("StationC");
  StationInfo stationDInfo = Station::initData("StationD");
  std::vector<StationInfo> stationsInfos = {stationAInfo, stationBInfo, stationCInfo, stationDInfo};
  std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationsInfos, test_map_state.gameGraph);
  RoadInfo roadABInfo = Road::genDataByName(stations, "StationA", "StationB", 1, std::make_shared<playersState::Player>(test_player), RoadColor::RED, 3);
  RoadInfo roadACInfo = Road::genDataByName(stations, "StationA", "StationC", 2, nullptr, RoadColor::BLUE, 4);
  RoadInfo roadADInfo = Road::genDataByName(stations, "StationA", "StationD", 3, nullptr, RoadColor::GREEN, 2);
  std::vector<RoadInfo> roadsInfos = {roadABInfo, roadACInfo, roadADInfo};
  test_map_state.fillMapWithInfos(stationsInfos, roadsInfos, {}, {}, test_map_state.gameGraph);
  std::shared_ptr<Road> roadAC = test_map_state.getRoadsBetweenStations(stations[0], stations[2])[0];
  bool isClaimable = test_map_state.isRoadClaimable(4, roadAC, std::make_shared<playersState::Player>(test_player));
  CHECK_EQ(isClaimable, true); // roadAC should be claimable
  ANN_END("isRoadClaimable")
}
TEST(getRoadsOwnedByPlayer){

}
TEST(getRoadsUsableByPlayer){
  
}
TEST(isDestinationReachableStatic){
  
}
TEST(isDestinationReachableIntern){
  
}
TEST(isDestinationReachable){
  
}

SUITE_END() // Interactions 

SUITE_END() // Operations


/* vim: set sw=2 sts=2 et : */
