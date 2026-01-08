
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Ferry.h"
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
#define REQUIRE(...) BOOST_REQUIRE(__VA_ARGS__)
#define CHECK(x) BOOST_CHECK(x)

using namespace ::mapState;

using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, int, int>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

TEST(TestStaticAssert)
{
  CHECK(1);
}

std::shared_ptr<boost::adjacency_list<>> test_graph = std::make_shared<boost::adjacency_list<>>();
std::shared_ptr<mapState::Station> test_stationA = std::make_shared<mapState::Station>("StationA", std::make_shared<playersState::Player>("OwnerA", playersState::PlayerColor::RED, 0, 30, 2, 4, nullptr), std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
std::shared_ptr<mapState::Station> test_stationB = std::make_shared<mapState::Station>("StationB", std::make_shared<playersState::Player>("OwnerB", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr), std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
int test_ferry_id = 101;
int test_length = 5;
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, 6, nullptr);
std::shared_ptr<boost::adjacency_list<>::edge_descriptor> test_edge = std::make_shared<boost::adjacency_list<>::edge_descriptor>(
    boost::add_edge(*(test_stationA->getVertex().get()), *(test_stationB->getVertex().get()), *test_graph).first);
int test_locomotives = 2;

SUITE_START(Constructors)

TEST(Basic)
{
  ANN_START("Basic Constructor")
  mapState::Ferry test_ferry = mapState::Ferry(test_ferry_id, test_owner, test_stationA, test_stationB, test_locomotives, test_length, test_edge);
  CHECK_EQ(test_ferry.id, test_ferry_id);
  CHECK_EQ(test_ferry.owner->name, test_owner->name);
  CHECK_EQ(test_ferry.stationA, test_stationA);
  CHECK_EQ(test_ferry.stationB, test_stationB);
  CHECK_EQ(test_ferry.color, RoadColor::NONE);
  CHECK_EQ(test_ferry.length, test_length);
  CHECK_EQ(*test_ferry.edge, *test_edge);
  CHECK_EQ(test_ferry.locomotives, test_locomotives);
  ANN_END("Basic Constructor")
}

TEST(Default)
{
  ANN_START("Default Constructor")

  Ferry ferry;
  CHECK_EQ(ferry.id, -1);
  CHECK_EQ(ferry.owner, nullptr);
  CHECK_EQ(ferry.stationA, nullptr);
  CHECK_EQ(ferry.stationB, nullptr);
  CHECK_EQ(ferry.color, RoadColor::UNKNOWN);
  CHECK_EQ(ferry.length, -1);
  CHECK_EQ(ferry.edge, nullptr);
  CHECK_EQ(ferry.locomotives, -1);
  ANN_END("Default Constructor")
}

TEST(Init)
{
  ANN_START("Init Constructor")

  mapState::Ferry test_ferry = mapState::Ferry::Init(test_ferry_id, test_stationA, test_stationB, test_locomotives, test_length, test_graph);
  CHECK_EQ(test_ferry.id, test_ferry_id);
  CHECK_EQ(test_ferry.owner, nullptr);
  CHECK_EQ(test_ferry.stationA, test_stationA);
  CHECK_EQ(test_ferry.stationB, test_stationB);
  CHECK_EQ(test_ferry.color, RoadColor::NONE);
  CHECK_EQ(test_ferry.length, test_length);
  CHECK(test_ferry.edge != nullptr);
  CHECK_EQ(test_ferry.locomotives, test_locomotives);
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
    std::shared_ptr<playersState::Player> batch_owner1 = std::make_shared<playersState::Player>("BatchOwnerFerry1", playersState::PlayerColor::RED, 0, 50, 5, 7, nullptr);
    std::shared_ptr<playersState::Player> batch_owner2 = std::make_shared<playersState::Player>("BatchOwnerFerry2", playersState::PlayerColor::BLACK, 0, 55, 6, 8, nullptr);
    std::shared_ptr<playersState::Player> batch_owner3 = std::make_shared<playersState::Player>("BatchOwnerFerry3", playersState::PlayerColor::YELLOW, 0, 60, 7, 9, nullptr);
    std::vector<FerryInfo> ferryInfos = {
        Ferry::genData(batch_stationA, batch_stationB, 201, batch_owner1, 1, 3),
        Ferry::genData(batch_stationB, batch_stationC, 202, batch_owner2, 2, 4),
        Ferry::genData(batch_stationA, batch_stationC, 203, batch_owner3, 3, 5),
    };
    std::vector<std::shared_ptr<Ferry>> ferries = Ferry::BatchConstructor(ferryInfos, test_graph);
    CHECK_EQ(ferries.size(), 3);
    for (int i = 0; i < static_cast<int>(ferries.size()); i++)
    {
      CHECK_EQ(ferries[i]->id, std::get<0>(std::get<1>(ferryInfos[i])));
      CHECK_EQ(ferries[i]->owner->name, std::get<1>(std::get<1>(ferryInfos[i]))->name);
      CHECK_EQ(ferries[i]->stationA, std::get<0>(std::get<0>(ferryInfos[i])));
      CHECK_EQ(ferries[i]->stationB, std::get<1>(std::get<0>(ferryInfos[i])));
      CHECK_EQ(ferries[i]->color, RoadColor::NONE);
      CHECK_EQ(ferries[i]->locomotives, std::get<2>(std::get<1>(ferryInfos[i])));
      CHECK_EQ(ferries[i]->length, std::get<3>(std::get<1>(ferryInfos[i])));
    }
    ANN_END("BatchConstructor")
  }
  {
    ANN_START("BatchConstructor with no gameGraph")
    std::vector<FerryInfo> ferryInfos = {
        Ferry::genData(test_stationA, test_stationB, 301, test_owner, RoadColor::GREEN, 5)};
    // check if exception is thrown if no graph is provided
    CHECK_THROW(Ferry::BatchConstructor(ferryInfos, nullptr), std::invalid_argument);
    ANN_END("BatchConstructor with no gameGraph")
  }
}

SUITE_START(DataGenerators)

TEST(genData)
{
  ANN_START("genData")

  FerryInfo info = Ferry::genData(test_stationA, test_stationB, test_ferry_id, test_owner, test_locomotives, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_ferry_id);
  CHECK_EQ(std::get<1>(info.second)->name, test_owner->name);
  CHECK_EQ(std::get<2>(info.second), test_locomotives);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("genData")
}
TEST(initData)
{
  ANN_START("initData")

  FerryInfo info = Ferry::initData(test_stationA, test_stationB, test_ferry_id, test_locomotives, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_ferry_id);
  CHECK_EQ(std::get<1>(info.second), nullptr);
  CHECK_EQ(std::get<2>(info.second), test_locomotives);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("initData")
}
TEST(genDataByName)
{
  ANN_START("genDataByName")

  std::vector<std::shared_ptr<Station>> stations = {test_stationA, test_stationB};
  FerryInfo info = Ferry::genDataByName(stations, "StationA", "StationB", test_ferry_id, test_owner, test_locomotives, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_ferry_id);
  CHECK_EQ(std::get<1>(info.second)->name, test_owner->name);
  CHECK_EQ(std::get<2>(info.second), test_locomotives);
  CHECK_EQ(std::get<3>(info.second), test_length);

  ANN_END("genDataByName")
}
TEST(initDataByName)
{
  ANN_START("initDataByName")

  std::vector<std::shared_ptr<Station>> stations = {test_stationA, test_stationB};
  FerryInfo info = Ferry::initDataByName(stations, "StationA", "StationB", test_ferry_id, test_locomotives, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_ferry_id);
  CHECK_EQ(std::get<1>(info.second), nullptr);
  CHECK_EQ(std::get<2>(info.second), test_locomotives);
  CHECK_EQ(std::get<3>(info.second), test_length);

  ANN_END("initDataByName")
}

SUITE_END() // DataGenerators

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)
mapState::Ferry test_ferry_getters_and_setters = mapState::Ferry(test_ferry_id, test_owner, test_stationA, test_stationB, test_locomotives, test_length, test_edge);
SUITE_START(Getters)

TEST(getId)
{
  ANN_START("getId")

  CHECK_EQ(test_ferry_getters_and_setters.getId(), test_ferry_id);
  ANN_END("getId")
}

TEST(getStationA)
{
  ANN_START("getStationA")

  CHECK_EQ(test_ferry_getters_and_setters.getStationA(), test_stationA);
  ANN_END("getStationA")
}

TEST(getStationB)
{
  ANN_START("getStationB")

  CHECK_EQ(test_ferry_getters_and_setters.getStationB(), test_stationB);
  ANN_END("getStationB")
}

TEST(getLength)
{
  ANN_START("getLength")

  CHECK_EQ(test_ferry_getters_and_setters.getLength(), test_length);
  ANN_END("getLength")
}

TEST(getEdge)
{
  ANN_START("getEdge")

  CHECK_EQ(*test_ferry_getters_and_setters.getEdge(), *test_edge);
  ANN_END("getEdge")
}

TEST(getColor)
{
  ANN_START("getColor")

  CHECK_EQ(test_ferry_getters_and_setters.getColor(), RoadColor::NONE);
  ANN_END("getColor")
}

TEST(getOwner)
{
  ANN_START("getOwner")

  CHECK_EQ(test_ferry_getters_and_setters.getOwner()->name, test_owner->name);
  ANN_END("getOwner")
}

TEST(getVertexA)
{
  ANN_START("getVertexA")

  CHECK_EQ(*test_ferry_getters_and_setters.getVertexA(), *(test_stationA->vertex));
  ANN_END("getVertexA")
}

TEST(getVertexB)
{
  ANN_START("getVertexB")

  CHECK_EQ(*test_ferry_getters_and_setters.getVertexB(), *(test_stationB->vertex));
  ANN_END("getVertexB")
}

TEST(getLocomotives)
{
  ANN_START("getLocomotives")

  CHECK_EQ(test_ferry_getters_and_setters.getLocomotives(), test_locomotives);
  ANN_END("getLocomotives")
}

SUITE_END() // Getters
SUITE_START(Setters)
std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>("NewOwner", playersState::PlayerColor::BLACK, 0, 50, 5, 7, nullptr);
TEST(setOwner)
{
  ANN_START("setOwner")

  test_ferry_getters_and_setters.setOwner(test_set_owner);
  CHECK_EQ(test_ferry_getters_and_setters.owner->name, test_set_owner->name);
  ANN_END("setOwner")
}
bool new_block_status = true;

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)

SUITE_END() // Interactions

SUITE_END() // Operations
/* vim: set sw=2 sts=2 et : */
