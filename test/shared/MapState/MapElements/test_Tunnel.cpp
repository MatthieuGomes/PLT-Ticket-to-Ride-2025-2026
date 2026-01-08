#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Tunnel.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"
#include "../../src/shared/playersState/PlayerColor.h"
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
using TunnelDetail = std::tuple<int, std::shared_ptr<playersState::Player>, RoadColor, int>;
using TunnelInfo = std::pair<StationPair, TunnelDetail>;

TEST(TestStaticAssert)
{
  CHECK(1);
}

std::shared_ptr<boost::adjacency_list<>> test_graph = std::make_shared<boost::adjacency_list<>>();
std::shared_ptr<mapState::Station> test_stationA = std::make_shared<mapState::Station>("StationA", std::make_shared<playersState::Player>("OwnerA", playersState::PlayerColor::RED, 0, 30, 2, 4, nullptr), std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
std::shared_ptr<mapState::Station> test_stationB = std::make_shared<mapState::Station>("StationB", std::make_shared<playersState::Player>("OwnerB", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr), std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
int test_tunnel_id = 101;
RoadColor test_color = RoadColor::GREEN;
int test_length = 5;
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, 6, nullptr);
std::shared_ptr<boost::adjacency_list<>::edge_descriptor> test_edge = std::make_shared<boost::adjacency_list<>::edge_descriptor>(
    boost::add_edge(*(test_stationA->getVertex().get()), *(test_stationB->getVertex().get()), *test_graph).first);

SUITE_START(Constructors)

TEST(Basic)
{
  ANN_START("Basic Constructor")
  mapState::Tunnel test_tunnel = mapState::Tunnel(test_tunnel_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
  CHECK_EQ(test_tunnel.id, test_tunnel_id);
  CHECK_EQ(test_tunnel.owner->name, test_owner->name);
  CHECK_EQ(test_tunnel.stationA, test_stationA);
  CHECK_EQ(test_tunnel.stationB, test_stationB);
  CHECK_EQ(test_tunnel.color, test_color);
  CHECK_EQ(test_tunnel.length, test_length);
  CHECK_EQ(*test_tunnel.edge, *test_edge);
  ANN_END("Basic Constructor")
}

TEST(Default)
{
  ANN_START("Default Constructor")
  Tunnel tunnel;
  CHECK_EQ(tunnel.id, -1);
  CHECK_EQ(tunnel.owner, nullptr);
  CHECK_EQ(tunnel.stationA, nullptr);
  CHECK_EQ(tunnel.stationB, nullptr);
  CHECK_EQ(tunnel.color, RoadColor::UNKNOWN);
  CHECK_EQ(tunnel.length, -1);
  CHECK_EQ(tunnel.edge, nullptr);
  ANN_END("Default Constructor")
}

TEST(Init)
{
  ANN_START("Init Constructor")
  mapState::Tunnel test_tunnel = mapState::Tunnel::Init(test_tunnel_id, test_stationA, test_stationB, test_color, test_length, test_graph);
  CHECK_EQ(test_tunnel.id, test_tunnel_id);
  CHECK_EQ(test_tunnel.owner, nullptr);
  CHECK_EQ(test_tunnel.stationA, test_stationA);
  CHECK_EQ(test_tunnel.stationB, test_stationB);
  CHECK_EQ(test_tunnel.color, test_color);
  CHECK_EQ(test_tunnel.length, test_length);
  CHECK(test_tunnel.edge != nullptr);
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
    std::shared_ptr<playersState::Player> batch_owner1 = std::make_shared<playersState::Player>("BatchOwnerTunnel1", playersState::PlayerColor::RED, 0, 50, 5, 7, nullptr);
    std::shared_ptr<playersState::Player> batch_owner2 = std::make_shared<playersState::Player>("BatchOwnerTunnel2", playersState::PlayerColor::BLACK, 0, 55, 6, 8, nullptr);
    std::shared_ptr<playersState::Player> batch_owner3 = std::make_shared<playersState::Player>("BatchOwnerTunnel3", playersState::PlayerColor::YELLOW, 0, 60, 7, 9, nullptr);
    std::vector<TunnelInfo> tunnelInfos = {
        Tunnel::genData(batch_stationA, batch_stationB, 201, batch_owner1, RoadColor::PINK, 4),
        Tunnel::genData(batch_stationB, batch_stationC, 202, batch_owner2, RoadColor::ORANGE, 6),
        Tunnel::genData(batch_stationA, batch_stationC, 203, batch_owner3, RoadColor::BLUE, 5),
    };
    std::vector<std::shared_ptr<mapState::Tunnel>> tunnels = mapState::Tunnel::BatchConstructor(tunnelInfos, test_graph);
    CHECK_EQ(tunnels.size(), 3);
    for (int i = 0; i < static_cast<int>(tunnels.size()); i++)
    {
      CHECK_EQ(tunnels[i]->id, std::get<0>(std::get<1>(tunnelInfos[i])));
      CHECK_EQ(tunnels[i]->owner->name, std::get<1>(std::get<1>(tunnelInfos[i]))->name);
      CHECK_EQ(tunnels[i]->stationA, std::get<0>(std::get<0>(tunnelInfos[i])));
      CHECK_EQ(tunnels[i]->stationB, std::get<1>(std::get<0>(tunnelInfos[i])));
      CHECK_EQ(tunnels[i]->color, std::get<2>(std::get<1>(tunnelInfos[i])));
      CHECK_EQ(tunnels[i]->length, std::get<3>(std::get<1>(tunnelInfos[i])));
    }
    ANN_END("BatchConstructor")
  }
  {
    ANN_START("BatchConstructor no game graph")
    std::vector<TunnelInfo> tunnelInfos = {
        Tunnel::genData(test_stationA, test_stationB, 301, test_owner, RoadColor::GREEN, 5),
        Tunnel::genData(test_stationB, test_stationA, 302, test_owner, RoadColor::RED, 4),
    };
    // check if exception is thrown if no graph is provided
    CHECK_THROW(mapState::Tunnel::BatchConstructor(tunnelInfos, nullptr), std::invalid_argument);
    ANN_END("BatchConstructor no game graph") 
  }
}

SUITE_START(DataGenerators)

TEST(genData)
{
  ANN_START("genData")
  TunnelInfo info = Tunnel::genData(test_stationA, test_stationB, test_tunnel_id, test_owner, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_tunnel_id);
  CHECK_EQ(std::get<1>(info.second)->name, test_owner->name);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("genData")
}

TEST(initData)
{
  ANN_START("initData")
  TunnelInfo info = Tunnel::initData(test_stationA, test_stationB, test_tunnel_id, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_tunnel_id);
  CHECK_EQ(std::get<1>(info.second), nullptr);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("initData")
}

TEST(genDataByName)
{
  ANN_START("genDataByName")
  std::vector<std::shared_ptr<Station>> stations = {test_stationA, test_stationB};
  TunnelInfo info = Tunnel::genDataByName(stations, "StationA", "StationB", test_tunnel_id, test_owner, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_tunnel_id);
  CHECK_EQ(std::get<1>(info.second)->name, test_owner->name);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("genDataByName")
}

TEST(initDataByName)
{
  ANN_START("initDataByName")
  std::vector<std::shared_ptr<Station>> stations = {test_stationA, test_stationB};
  TunnelInfo info = Tunnel::initDataByName(stations, "StationA", "StationB", test_tunnel_id, test_color, test_length);
  CHECK_EQ(info.first.first, test_stationA);
  CHECK_EQ(info.first.second, test_stationB);
  CHECK_EQ(std::get<0>(info.second), test_tunnel_id);
  CHECK_EQ(std::get<1>(info.second), nullptr);
  CHECK_EQ(std::get<2>(info.second), test_color);
  CHECK_EQ(std::get<3>(info.second), test_length);
  ANN_END("initDataByName")
}

SUITE_END()

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)
mapState::Tunnel test_tunnel_getters_and_setters = mapState::Tunnel(test_tunnel_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
SUITE_START(Getters)

TEST(getId)
{
  ANN_START("getId")
  CHECK_EQ(test_tunnel_getters_and_setters.getId(), test_tunnel_id);
  ANN_END("getId")
}

TEST(getStationA)
{
  ANN_START("getStationA")
  CHECK_EQ(test_tunnel_getters_and_setters.getStationA()->name, test_stationA->name);
  ANN_END("getStationA")
}

TEST(getStationB)
{
  ANN_START("getStationB")
  CHECK_EQ(test_tunnel_getters_and_setters.getStationB()->name, test_stationB->name);
  ANN_END("getStationB")
}

TEST(getLength)
{
  ANN_START("getLength")
  CHECK_EQ(test_tunnel_getters_and_setters.getLength(), test_length);
  ANN_END("getLength")
}

TEST(getEdge)
{
  ANN_START("getEdge")
  CHECK_EQ(*test_tunnel_getters_and_setters.getEdge(), *test_edge);
  ANN_END("getEdge")
}

TEST(getColor)
{
  ANN_START("getColor")
  CHECK_EQ(test_tunnel_getters_and_setters.getColor(), test_color);
  ANN_END("getColor")
}

TEST(getOwner)
{
  ANN_START("getOwner")
  CHECK_EQ(test_tunnel_getters_and_setters.getOwner()->name, test_owner->name);
  ANN_END("getOwner")
}

TEST(getVertexA)
{
  ANN_START("getVertexA")
  CHECK_EQ(*test_tunnel_getters_and_setters.getVertexA(), *(test_stationA->vertex));
  ANN_END("getVertexA")
}

TEST(getVertexB)
{
  ANN_START("getVertexB")
  CHECK_EQ(*test_tunnel_getters_and_setters.getVertexB(), *(test_stationB->vertex));
  ANN_END("getVertexB")
}

SUITE_END() // Getters
SUITE_START(Setters)
std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>("NewOwner", playersState::PlayerColor::BLACK, 0, 50, 5, 7, nullptr);
TEST(setOwner)
{
  ANN_START("setOwner")
  test_tunnel_getters_and_setters.setOwner(test_set_owner);
  CHECK_EQ(test_tunnel_getters_and_setters.owner->name, test_set_owner->name);
  ANN_END("setOwner")
}

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)

SUITE_END() // Interactions

SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
