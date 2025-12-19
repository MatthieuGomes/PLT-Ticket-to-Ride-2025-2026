
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Tunnel.h"
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

using namespace ::mapState;

using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using TunnelDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>;
using TunnelInfo = std::pair<StationPair, TunnelDetail>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

std::shared_ptr<boost::adjacency_list<>> test_graph = std::make_shared<boost::adjacency_list<>>();
std::shared_ptr<mapState::Station> test_stationA = std::make_shared<mapState::Station>("StationA", std::make_shared<playersState::Player>(1, "OwnerA", cardsState::ColorCard::RED, 0, 30, 2, 4, nullptr), false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
std::shared_ptr<mapState::Station> test_stationB = std::make_shared<mapState::Station>("StationB", std::make_shared<playersState::Player>(2, "OwnerB", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr), false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
int test_tunnel_id = 101;
bool test_is_blocked = false;
cardsState::ColorCard test_color = cardsState::ColorCard::GREEN;
int test_length = 5;
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>(3, "TestOwner", cardsState::ColorCard::YELLOW, 0, 40, 4, 6, nullptr);
std::shared_ptr<boost::adjacency_list<>::edge_descriptor> test_edge = std::make_shared<boost::adjacency_list<>::edge_descriptor>(
    boost::add_edge(*(test_stationA->getVertex().get()), *(test_stationB->getVertex().get()), *test_graph).first);

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Basic){
  std::cout << "Default Constructor Test Started ..." << std::endl;
  mapState::Tunnel test_tunnel = mapState::Tunnel(test_tunnel_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_is_blocked,  test_edge);
  BOOST_CHECK_EQUAL(test_tunnel.id, test_tunnel_id);
  BOOST_CHECK_EQUAL(test_tunnel.owner->name, test_owner->name);
  BOOST_CHECK_EQUAL(test_tunnel.stationA, test_stationA);
  BOOST_CHECK_EQUAL(test_tunnel.stationB, test_stationB);
  BOOST_CHECK_EQUAL(test_tunnel.color, test_color);
  BOOST_CHECK_EQUAL(test_tunnel.length, test_length);
  BOOST_CHECK_EQUAL(test_tunnel.isBlocked, test_is_blocked);
  BOOST_CHECK_EQUAL(*test_tunnel.edge, *test_edge);
  std::cout << "Default Constructor Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructor){
  std::cout << "BatchConstructor Test Started ..." << std::endl;
  std::string stationA_name = "BatchStationA";
  std::string stationB_name = "BatchStationB";
  std::string stationC_name = "BatchStationC";
  std::shared_ptr<playersState::Player> stationA_owner = std::make_shared<playersState::Player>(3, "BatchOwnerStationA", cardsState::ColorCard::RED, 0, 30, 2, 4, nullptr);
  std::shared_ptr<playersState::Player> stationB_owner = std::make_shared<playersState::Player>(4, "BatchOwnerStationB", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr);
  std::shared_ptr<playersState::Player> stationC_owner = std::make_shared<playersState::Player>(5, "BatchOwnerStationC", cardsState::ColorCard::GREEN, 0, 50, 4, 6, nullptr);
  std::shared_ptr<mapState::Station> batch_stationA = std::make_shared<mapState::Station>(stationA_name, stationA_owner, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
  std::shared_ptr<mapState::Station> batch_stationB = std::make_shared<mapState::Station>(stationB_name, stationB_owner, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
  std::shared_ptr<mapState::Station> batch_stationC = std::make_shared<mapState::Station>(stationC_name, stationC_owner, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
  std::shared_ptr<playersState::Player> batch_owner1 = std::make_shared<playersState::Player>(4, "BatchOwnerTunnel1", cardsState::ColorCard::WHITE, 0, 50, 5, 7, nullptr);
  std::shared_ptr<playersState::Player> batch_owner2 = std::make_shared<playersState::Player>(5, "BatchOwnerTunnel2", cardsState::ColorCard::BLACK, 0, 55, 6, 8, nullptr);
  std::shared_ptr<playersState::Player> batch_owner3 = std::make_shared<playersState::Player>(6, "BatchOwnerTunnel3", cardsState::ColorCard::YELLOW, 0, 60, 7, 9, nullptr);
  std::vector<TunnelInfo> tunnelInfos = {
      Tunnel::genData(batch_stationA, batch_stationB, 201, batch_owner1, cardsState::ColorCard::PINK, 4, false),
      Tunnel::genData(batch_stationB, batch_stationC, 202, batch_owner2, cardsState::ColorCard::ORANGE, 6, true),
      Tunnel::genData(batch_stationA, batch_stationC, 203, batch_owner3, cardsState::ColorCard::BLUE, 5, false),
  };
  std::vector<std::shared_ptr<mapState::Tunnel>> tunnels = mapState::Tunnel::BatchConstructor(tunnelInfos, test_graph);
  BOOST_CHECK_EQUAL(tunnels.size(), 3);
  for(int i = 0; i < static_cast<int>(tunnels.size()); i++) {
    BOOST_CHECK_EQUAL(tunnels[i]->id, std::get<0>(std::get<1>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i]->owner->name, std::get<1>(std::get<1>(tunnelInfos[i]))->name);
    BOOST_CHECK_EQUAL(tunnels[i]->stationA, std::get<0>(std::get<0>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i]->stationB, std::get<1>(std::get<0>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i]->color, std::get<2>(std::get<1>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i]->length, std::get<3>(std::get<1>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i]->isBlocked, std::get<4>(std::get<1>(tunnelInfos[i])));
  }
  std::cout << "BatchConstructor Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructorRequiresGraph){
  std::vector<TunnelInfo> tunnelInfos = {
      Tunnel::genData(test_stationA, test_stationB, 401, test_owner, cardsState::ColorCard::GREEN, 5, false)};
  // check if exception is thrown if no graph is provided
  BOOST_CHECK_THROW(mapState::Tunnel::BatchConstructor(tunnelInfos, nullptr), std::invalid_argument);

  std::vector<std::shared_ptr<mapState::Tunnel>> tunnels = mapState::Tunnel::BatchConstructor(tunnelInfos, test_graph);
  BOOST_CHECK_EQUAL(tunnels.size(), 1);
  BOOST_CHECK_EQUAL(tunnels[0]->id, 401);
}

BOOST_AUTO_TEST_CASE(GenData){
  std::cout << "GenData Test Started ..." << std::endl;
  TunnelInfo info = Tunnel::genData(test_stationA, test_stationB, test_tunnel_id, test_owner, test_color, test_length, test_is_blocked);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_tunnel_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second)->name, test_owner->name);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  BOOST_CHECK_EQUAL(std::get<4>(info.second), test_is_blocked);
  std::cout << "GenData Test Finished !\n"<< std::endl;
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
mapState::Tunnel test_tunnel_getters_and_setters = mapState::Tunnel(test_tunnel_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_is_blocked,  test_edge);
BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetId)
{
  std::cout << "GetId Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getId(), test_tunnel_id);
  std::cout << "GetId Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationA)
{
  std::cout << "GetStationA Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getStationA()->name, test_stationA->name);
  std::cout << "GetStationA Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationB)
{
  std::cout << "GetStationB Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getStationB()->name, test_stationB->name);
  std::cout << "GetStationB Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetLength)
{
  std::cout << "GetLength Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getLength(), test_length);
  std::cout << "GetLength Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetEdge)
{
  std::cout << "GetEdge Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_tunnel_getters_and_setters.getEdge(), *test_edge);
  std::cout << "GetEdge Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetColor)
{
  std::cout << "GetColor Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getColor(), test_color);
  std::cout << "GetColor Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetOwner)
{
  std::cout << "GetOwner Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getOwner()->name, test_owner->name);
  std::cout << "GetOwner Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetBlockStatus)
{
  std::cout << "GetBlockStatus Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getBlockStatus(), test_is_blocked);
  std::cout << "GetBlockStatus Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetVertexA)
{
  std::cout << "GetVertexA Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_tunnel_getters_and_setters.getVertexA(), *(test_stationA->vertex));
  std::cout << "GetVertexA Test Finished !\n"<< std::endl;
}


BOOST_AUTO_TEST_CASE(GetVertexB)
{
  std::cout << "GetVertexB Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_tunnel_getters_and_setters.getVertexB(), *(test_stationB->vertex));
  std::cout << "GetVertexB Test Finished !\n"<< std::endl;
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)
std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>(4, "NewOwner", cardsState::ColorCard::BLACK, 0, 50, 5, 7, nullptr);
BOOST_AUTO_TEST_CASE(SetOwner)
{
  std::cout << "SetOwner Test Started ..." << std::endl;
  test_tunnel_getters_and_setters.setOwner(test_set_owner);
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.owner->name, test_set_owner->name);
  std::cout << "SetOwner Test Finished !\n"<< std::endl;
}
bool new_block_status = true;
BOOST_AUTO_TEST_CASE(SetBlockStatus)
{
  std::cout << "SetBlockStatus Test Started ..." << std::endl;
  test_tunnel_getters_and_setters.setBlockStatus(new_block_status);
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.isBlocked, new_block_status);
  std::cout << "SetBlockStatus Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
