#define BOOST_TEST_MODULE MapTests
#include <boost/test/included/unit_test.hpp>
#include "Map.h"

class Player {
public:
    std::string name;
    Player(const std::string& n) : name(n) {}
};

//check if map was well-generated

BOOST_AUTO_TEST_CASE(generate_map_creates_stations_and_roads)
{
    Map map;
    map.generateMap();

    BOOST_TEST(map.listStations().size() == 4);
    BOOST_TEST(map.listRoads().size() == 4);
}

//check if stations were found

BOOST_AUTO_TEST_CASE(find_station_by_name)
{
    Map map;
    map.generateMap();

    auto* berlin = map.getStationByName("Berlin");
    BOOST_REQUIRE(berlin != nullptr);
    BOOST_TEST(berlin->data->name == "Berlin");
}

//check road claim

BOOST_AUTO_TEST_CASE(claim_road_success)
{
    Map map;
    map.generateMap();

    Player alice("Alice");
    auto* paris = map.getStationByName("Paris");
    auto* madrid = map.getStationByName("Madrid");

    bool claimed = map.claimRoad(&alice, paris->data, madrid->data);
    BOOST_TEST(claimed);

    auto* road = map.getRoad(paris, madrid);
    BOOST_REQUIRE(road != nullptr);
    BOOST_TEST(road->data->owner == &alice);
}

//check adjacent stations view

BOOST_AUTO_TEST_CASE(adjacent_stations_are_correct)
{
    Map map;
    map.generateMap();

    auto* paris = map.getStationByName("Paris");
    auto adj = map.getAdjacentStations(paris);

    BOOST_TEST(adj.size() == 2); 

    std::vector<std::string> names;
    for (auto* s : adj) names.push_back(s->data->name);

    BOOST_CHECK(std::find(names.begin(), names.end(), "Berlin") != names.end());
    BOOST_CHECK(std::find(names.begin(), names.end(), "Madrid") != names.end());
}

//check shortest path

BOOST_AUTO_TEST_CASE(shortest_path_works)
{
    Map map;
    map.generateMap();

    auto* paris = map.getStationByName("Paris");
    auto* rome  = map.getStationByName("Rome");

    Path path = map.findShortestPath(paris->data, rome->data);

    BOOST_TEST(!path.STATIONS.empty());
    BOOST_TEST(path.STATIONS.front()->data->name == "Paris");
    BOOST_TEST(path.STATIONS.back()->data->name == "Rome");
    BOOST_TEST(path.NUMEDGES == 2); // Paris -> Berlin -> Rome
}