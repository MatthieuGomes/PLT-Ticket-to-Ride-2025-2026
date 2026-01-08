#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Road.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"
#include <boost/graph/adjacency_list.hpp>
#include <memory>
#include <stdexcept>

#define DEBUG_MODE false
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
#define CHECK(x) BOOST_CHECK(x)

using namespace ::mapState;

using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, RoadColor, int>;
using RoadInfo = std::pair<StationPair, RoadDetail>;

TEST(TestStaticAssert)
{
  BOOST_CHECK(1);
}

std::shared_ptr<boost::adjacency_list<>> test_graph = std::make_shared<boost::adjacency_list<>>();
std::shared_ptr<mapState::Station> test_stationA = std::make_shared<mapState::Station>("StationA", std::make_shared<playersState::Player>("OwnerA", playersState::PlayerColor::RED, 0, 30, 2, 4, nullptr), std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
std::shared_ptr<mapState::Station> test_stationB = std::make_shared<mapState::Station>("StationB", std::make_shared<playersState::Player>("OwnerB", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr), std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
int test_road_id = 101;
RoadColor test_color = RoadColor::GREEN;
int test_length = 5;
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, 6, nullptr);
std::shared_ptr<boost::adjacency_list<>::edge_descriptor> test_edge = std::make_shared<boost::adjacency_list<>::edge_descriptor>(
    boost::add_edge(*(test_stationA->getVertex().get()), *(test_stationB->getVertex().get()), *test_graph).first);

SUITE_START(Constructors)

TEST(Default)
{
  ANN_START("Default Constructor")

  Road road;
  CHECK_EQ(road.id, -1);
  CHECK_EQ(road.owner, nullptr);
  CHECK_EQ(road.stationA, nullptr);
  CHECK_EQ(road.stationB, nullptr);
  CHECK_EQ(road.color, RoadColor::UNKNOWN);
  CHECK_EQ(road.length, -1);
  CHECK_EQ(road.edge, nullptr);
  ANN_END("Default Constructor")
}

TEST(Basic)
{
  ANN_START("Basic Constructor")
  mapState::Road test_road = mapState::Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
  CHECK_EQ(test_road.id, test_road_id);
  CHECK_EQ(test_road.owner->name, test_owner->name);
  CHECK_EQ(test_road.stationA, test_stationA);
  CHECK_EQ(test_road.stationB, test_stationB);
  CHECK_EQ(test_road.color, test_color);
  CHECK_EQ(test_road.length, test_length);
  CHECK_EQ(*test_road.edge, *test_edge);
}

TEST(Init)
{
  ANN_START("Init Constructor")
  mapState::Road test_road = mapState::Road::Init(test_road_id, test_stationA, test_stationB, test_color, test_length, test_graph);
  CHECK_EQ(test_road.id, test_road_id);
  CHECK_EQ(test_road.owner, nullptr);
  CHECK_EQ(test_road.stationA, test_stationA);
  CHECK_EQ(test_road.stationB, test_stationB);
  CHECK_EQ(test_road.color, test_color);
  CHECK_EQ(test_road.length, test_length);
  BOOST_CHECK(test_road.edge != nullptr);
  ANN_END("Init Constructor")
}

TEST(BatchConstructor)
{

  {
    ANN_START("BatchConstructor")
    std::string stationA_name = "BatchStationA";
    std::string stationB_name = "BatchStationB";
    std::string stationC_name = "BatchStationC";
    std::shared_ptr<playersState::Player> stationA_owner = std::make_shared<playersState::Player>("BatchOwnerStationA", playersState::PlayerColor::RED, 0, 30, 2, 4, nullptr);
    std::shared_ptr<playersState::Player> stationB_owner = std::make_shared<playersState::Player>("BatchOwnerStationB", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr);
    std::shared_ptr<playersState::Player> stationC_owner = std::make_shared<playersState::Player>("BatchOwnerStationC", playersState::PlayerColor::GREEN, 0, 50, 4, 6, nullptr);
    std::shared_ptr<mapState::Station> batch_stationA = std::make_shared<mapState::Station>(stationA_name, stationA_owner, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
    std::shared_ptr<mapState::Station> batch_stationB = std::make_shared<mapState::Station>(stationB_name, stationB_owner, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
    std::shared_ptr<mapState::Station> batch_stationC = std::make_shared<mapState::Station>(stationC_name, stationC_owner, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
    std::shared_ptr<playersState::Player> batch_owner1 = std::make_shared<playersState::Player>("BatchOwnerRoad1", playersState::PlayerColor::RED, 0, 50, 5, 7, nullptr);
    std::shared_ptr<playersState::Player> batch_owner2 = std::make_shared<playersState::Player>("BatchOwnerRoad2", playersState::PlayerColor::BLACK, 0, 55, 6, 8, nullptr);
    std::shared_ptr<playersState::Player> batch_owner3 = std::make_shared<playersState::Player>("BatchOwnerRoad3", playersState::PlayerColor::YELLOW, 0, 60, 7, 9, nullptr);
    std::vector<RoadInfo> roadInfos = {
        Road::genData(
            batch_stationA,
            batch_stationB,
            202,
            batch_owner1,
            RoadColor::ORANGE,
            7),
        Road::genData(
            batch_stationB,
            batch_stationC,
            203,
            batch_owner2,
            RoadColor::PINK,
            3),
        Road::genData(
            batch_stationA,
            batch_stationC,
            204,
            batch_owner3,
            RoadColor::WHITE,
            4),
    };
    std::vector<std::shared_ptr<mapState::Road>> roads = mapState::Road::BatchConstructor(roadInfos, test_graph);
    CHECK_EQ(roads.size(), 3);
    for (int i = 0; i < 3; i++)
    {
      CHECK_EQ(roads[i]->id, std::get<0>(std::get<1>(roadInfos[i])));
      CHECK_EQ(roads[i]->owner->name, std::get<1>(std::get<1>(roadInfos[i]))->name);
      CHECK_EQ(roads[i]->stationA, std::get<0>(std::get<0>(roadInfos[i])));
      CHECK_EQ(roads[i]->stationB, std::get<1>(std::get<0>(roadInfos[i])));
      CHECK_EQ(roads[i]->color, std::get<2>(std::get<1>(roadInfos[i])));
      CHECK_EQ(roads[i]->length, std::get<3>(std::get<1>(roadInfos[i])));
    }
    ANN_END("BatchConstructor")
  }
  {
    ANN_START("BatchConstructor with no gameGraph")
    std::vector<RoadInfo> roadInfos = {
        Road::genData(test_stationA, test_stationB, 301, test_owner, RoadColor::GREEN, 5)};
    // check if exception is thrown if no graph is provided
    CHECK_THROW(Road::BatchConstructor(roadInfos, nullptr), std::invalid_argument);
    ANN_END("BatchConstructor with no gameGraph")
  }
}

SUITE_START(DataGenerators)

TEST(genData)
{
  ANN_START("genData")
  RoadInfo info = Road::genData(test_stationA, test_stationB, test_road_id, test_owner, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_road_id);
  CHECK_EQ(std::get<1>(info.second)->name, test_owner->name);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("genData")
}
TEST(initData)
{
  ANN_START("initData")
  RoadInfo info = Road::initData(test_stationA, test_stationB, test_road_id, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_road_id);
  CHECK_EQ(std::get<1>(info.second), nullptr);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("initData")
}

TEST(genDataByName)
{
  ANN_START("genDataByName")
  RoadInfo info = Road::genDataByName({test_stationA, test_stationB}, test_stationA->name, test_stationB->name, test_road_id, test_owner, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_road_id);
  CHECK_EQ(std::get<1>(info.second)->name, test_owner->name);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("genDataByName")
}
TEST(initDataByName)
{
  ANN_START("initDataByName")
  RoadInfo info = Road::initDataByName({test_stationA, test_stationB}, test_stationA->name, test_stationB->name, test_road_id, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_road_id);
  CHECK_EQ(std::get<1>(info.second), nullptr);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("initDataByName")
}

SUITE_END() // DataGenerators

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)
mapState::Road test_road_getters_and_setters = mapState::Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);

SUITE_START(Getters)

TEST(getId)
{
  ANN_START("getId")
  CHECK_EQ(test_road_getters_and_setters.getId(), test_road_id);
  ANN_END("getId")
}

TEST(getStationA)
{
  ANN_START("getStationA")
  CHECK_EQ(test_road_getters_and_setters.getStationA()->name, test_stationA->name);
  ANN_END("getStationA")
}

TEST(getStationB)
{
  ANN_START("getStationB")
  CHECK_EQ(test_road_getters_and_setters.getStationB()->name, test_stationB->name);
  ANN_END("getStationB")
}

TEST(getLength)
{
  ANN_START("getLength")
  CHECK_EQ(test_road_getters_and_setters.getLength(), test_length);
  ANN_END("getLength")
}

TEST(getEdge)
{
  ANN_START("getEdge")
  CHECK_EQ(*test_road_getters_and_setters.getEdge(), *test_edge);
  ANN_END("getEdge")
}

TEST(getColor)
{
  ANN_START("getColor")
  CHECK_EQ(test_road_getters_and_setters.getColor(), test_color);
  ANN_END("getColor")
}

TEST(getOwner)
{
  ANN_START("getOwner")
  CHECK_EQ(test_road_getters_and_setters.getOwner()->name, test_owner->name);
  ANN_END("getOwner")
}

TEST(getVertexA)
{
  ANN_START("getVertexA")
  CHECK_EQ(*test_road_getters_and_setters.getVertexA(), *(test_stationA->vertex));
  ANN_END("getVertexA")
}

TEST(getVertexB)
{
  ANN_START("getVertexB")
  CHECK_EQ(*test_road_getters_and_setters.getVertexB(), *(test_stationB->vertex));
  ANN_END("getVertexB")
}

SUITE_END()
SUITE_START(Setters)
std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>("NewOwner", playersState::PlayerColor::BLACK, 0, 50, 5, 7, nullptr);
TEST(setOwner)
{
  ANN_START("setOwner")
  test_road_getters_and_setters.setOwner(test_set_owner);
  CHECK_EQ(test_road_getters_and_setters.owner->name, test_set_owner->name);
  ANN_END("setOwner")
}

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)

TEST(isClaimable)
{
  ANN_START("isClaimable")
  int smallPartySize = 3;
  int largePartySize = 4;
  {
    ANN_START("owned road case")
    Road ownedRoad = Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
    std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad)};
    CHECK_EQ(ownedRoad.isClaimable(roads, smallPartySize, test_owner), false);
    ANN_END("owned road case")
  }
  {
    ANN_START("normal claimable road case")
    Road claimableRoad = Road(test_road_id, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
    std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(claimableRoad)};
    CHECK_EQ(claimableRoad.isClaimable(roads, smallPartySize, test_owner), true);
    ANN_END("normal claimable road case")
  }
  {
      ANN_START("double road case"){
          ANN_START("owned by himself case"){
              ANN_START("small party size case")
                  Road ownedRoad = Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
  Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
  std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
  CHECK_EQ(otherRoad.isClaimable(roads, smallPartySize, test_owner), false);
  ANN_END("small party size case")
}
{
  ANN_START("large party size case")
  Road ownedRoad = Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
  Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
  std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
  CHECK_EQ(otherRoad.isClaimable(roads, largePartySize, test_owner), false);
  ANN_END("large party size case")
}
ANN_END("owned by himself case")
}
{
    ANN_START("owned by another case"){
        ANN_START("small party size case")
            Road ownedRoad = Road(test_road_id, std::make_shared<playersState::Player>("AnotherOwner", playersState::PlayerColor::GREEN, 0, 35, 2, 4, nullptr), test_stationA, test_stationB, test_color, test_length, test_edge);
Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
CHECK_EQ(otherRoad.isClaimable(roads, smallPartySize, test_owner), false);
ANN_END("small party size case")
}
{
  ANN_START("large party size case")

  Road ownedRoad = Road(test_road_id, std::make_shared<playersState::Player>("AnotherOwner", playersState::PlayerColor::GREEN, 0, 35, 2, 4, nullptr), test_stationA, test_stationB, test_color, test_length, test_edge);
  Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
  std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
  CHECK_EQ(otherRoad.isClaimable(roads, largePartySize, test_owner), true);
  ANN_END("large party size case")
}
ANN_END("owned by another case")
}
ANN_END("double road case")
}
ANN_END("isClaimable")
}

TEST(getClaimableRoads)
{
  ANN_START("getClaimableRoads")
  int smallPartySize = 3;
  int largePartySize = 4;

  std::shared_ptr<playersState::Player> another_player = std::make_shared<playersState::Player>("AnotherPlayer", playersState::PlayerColor::GREEN, 0, 35, 2, 4, nullptr);
  std::vector<std::shared_ptr<mapState::Station>> test_stations = mapState::Station::BatchConstructor({
                                                                                                          Station::genData(std::make_shared<playersState::Player>("StationOwnerA", playersState::PlayerColor::RED, 0, 30, 2, 4, nullptr), "StationA"),
                                                                                                          Station::genData(nullptr, "StationB"),
                                                                                                          Station::genData(std::make_shared<playersState::Player>("StationOwnerC", playersState::PlayerColor::GREEN, 0, 50, 4, 6, nullptr), "StationC"),
                                                                                                          Station::genData(nullptr, "StationD"),
                                                                                                      },
                                                                                                      test_graph);
  std::vector<std::shared_ptr<mapState::Road>> roads = mapState::Road::BatchConstructor({
                                                                                            Road::genData(test_stations[0], test_stations[1], 401, nullptr, RoadColor::RED, 5),
                                                                                            Road::genData(test_stations[1], test_stations[2], 402, test_owner, RoadColor::BLUE, 4),
                                                                                            Road::genData(test_stations[0], test_stations[2], 403, another_player, RoadColor::GREEN, 6),
                                                                                            Road::genData(test_stations[1], test_stations[2], 404, nullptr, RoadColor::YELLOW, 3),
                                                                                            Road::genData(test_stations[0], test_stations[1], 405, nullptr, RoadColor::BLACK, 2),
                                                                                        },
                                                                                        test_graph);

  {
    ANN_START("small party size case")
    std::vector<std::shared_ptr<mapState::Road>> claimableRoads = mapState::Road::getClaimableRoads(roads, smallPartySize, test_owner);
    CHECK_EQ(claimableRoads.size(), 2);
    ANN_END("small party size case")
  }
  {
    ANN_START("large party size case")

    std::vector<std::shared_ptr<mapState::Road>> claimableRoads = mapState::Road::getClaimableRoads(roads, largePartySize, test_owner);
    CHECK_EQ(claimableRoads.size(), 2);
    ANN_END("large party size case")
  }
  ANN_END("getClaimableRoads")
}
// TODO : Add tests for display and _display methods
TEST(_display){
    ANN_START("_display")

        ANN_END("_display")}

TEST(display){
    ANN_START("display")
        ANN_END("display")}

SUITE_END() // Interactions

    SUITE_END() // Operations

    /* vim: set sw=2 sts=2 et : */
