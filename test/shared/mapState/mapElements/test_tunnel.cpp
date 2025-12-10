
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Tunnel.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"
#include <boost/graph/adjacency_list.hpp>

using namespace ::mapState;

using StationPair = std::pair<Station *, Station *>;
using TunnelDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, bool>;
using TunnelInfo = std::pair<StationPair, TunnelDetail>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

boost::adjacency_list<> test_graph = boost::adjacency_list<>();
mapState::Station * test_stationA = new mapState::Station("StationA", new playersState::Player(1, "OwnerA", cardsState::ColorCard::RED, 0, 30, 2, 4, nullptr), false, boost::add_vertex(test_graph));
mapState::Station * test_stationB = new mapState::Station("StationB", new playersState::Player(2, "OwnerB", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr), false, boost::add_vertex(test_graph));
int test_tunnel_id = 101;
bool test_is_blocked = false;
cardsState::ColorCard test_color = cardsState::ColorCard::GREEN;
int test_length = 5;
playersState::Player test_owner(3, "TestOwner", cardsState::ColorCard::YELLOW, 0, 40, 4, 6, nullptr);
boost::adjacency_list<>::edge_descriptor test_edge = boost::add_edge(*(test_stationA->getVertex()), *(test_stationB->getVertex()), test_graph).first;

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Basic){
  mapState::Tunnel test_tunnel = mapState::Tunnel(test_tunnel_id, &test_owner, test_stationA, test_stationB, test_color, test_length, test_is_blocked,  test_edge);
  BOOST_CHECK_EQUAL(test_tunnel.id, test_tunnel_id);
  BOOST_CHECK_EQUAL(test_tunnel.owner, &test_owner);
  BOOST_CHECK_EQUAL(test_tunnel.stationA, test_stationA);
  BOOST_CHECK_EQUAL(test_tunnel.stationB, test_stationB);
  BOOST_CHECK_EQUAL(test_tunnel.color, test_color);
  BOOST_CHECK_EQUAL(test_tunnel.length, test_length);
  BOOST_CHECK_EQUAL(test_tunnel.isBlocked, test_is_blocked);
  BOOST_CHECK_EQUAL(test_tunnel.edge, test_edge);
}

BOOST_AUTO_TEST_CASE(BatchConstructor){
  std::string stationA_name = "BatchStationA";
  std::string stationB_name = "BatchStationB";
  std::string stationC_name = "BatchStationC";
  playersState::Player stationA_owner(3, "BatchOwnerStationA", cardsState::ColorCard::RED, 0, 30, 2, 4, nullptr);
  playersState::Player stationB_owner(4, "BatchOwnerStationB", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr);
  playersState::Player stationC_owner(5, "BatchOwnerStationC", cardsState::ColorCard::GREEN, 0, 50, 4, 6, nullptr);
  mapState::Station * batch_stationA = new mapState::Station(stationA_name, &stationA_owner, false, boost::add_vertex(test_graph));
  mapState::Station * batch_stationB = new mapState::Station(stationB_name, &stationB_owner, false, boost::add_vertex(test_graph));
  mapState::Station * batch_stationC = new mapState::Station(stationC_name, &stationC_owner, false, boost::add_vertex(test_graph));
  playersState::Player batch_owner1(4, "BatchOwnerTunnel1", cardsState::ColorCard::WHITE, 0, 50, 5, 7, nullptr);
  playersState::Player batch_owner2(5, "BatchOwnerTunnel2", cardsState::ColorCard::BLACK, 0, 55, 6, 8, nullptr);
  playersState::Player batch_owner3(6, "BatchOwnerTunnel3", cardsState::ColorCard::YELLOW, 0, 60, 7, 9, nullptr);
  std::vector<TunnelInfo> tunnelInfos = {
      Tunnel::genData(batch_stationA, batch_stationB, 201, &batch_owner1, cardsState::ColorCard::PINK, 4, false),
      Tunnel::genData(batch_stationB, batch_stationC, 202, &batch_owner2, cardsState::ColorCard::ORANGE, 6, true),
      Tunnel::genData(batch_stationA, batch_stationC, 203, &batch_owner3, cardsState::ColorCard::BLUE, 5, false),
  };
  std::vector<mapState::Tunnel> tunnels = mapState::Tunnel::BatchConstructor(tunnelInfos, &test_graph);
  BOOST_CHECK_EQUAL(tunnels.size(), 3);
  for(int i = 0; i < static_cast<int>(tunnels.size()); i++) {
    BOOST_CHECK_EQUAL(tunnels[i].id, std::get<0>(std::get<1>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i].owner, std::get<1>(std::get<1>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i].stationA, std::get<0>(std::get<0>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i].stationB, std::get<1>(std::get<0>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i].color, std::get<2>(std::get<1>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i].length, std::get<3>(std::get<1>(tunnelInfos[i])));
    BOOST_CHECK_EQUAL(tunnels[i].isBlocked, std::get<4>(std::get<1>(tunnelInfos[i])));
  }
}

BOOST_AUTO_TEST_CASE(GenData){
  TunnelInfo info = Tunnel::genData(test_stationA, test_stationB, test_tunnel_id, &test_owner, test_color, test_length, test_is_blocked);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_tunnel_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second), &test_owner);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  BOOST_CHECK_EQUAL(std::get<4>(info.second), test_is_blocked);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
mapState::Tunnel test_tunnel_getters_and_setters = mapState::Tunnel(test_tunnel_id, &test_owner, test_stationA, test_stationB, test_color, test_length, test_is_blocked,  test_edge);
BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetId)
{
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getId(), test_tunnel_id);
}

BOOST_AUTO_TEST_CASE(GetStationA)
{
  BOOST_CHECK_EQUAL((*test_tunnel_getters_and_setters.getStationA()).name, test_stationA->name);
}

BOOST_AUTO_TEST_CASE(GetStationB)
{
  BOOST_CHECK_EQUAL((*test_tunnel_getters_and_setters.getStationB()).name, test_stationB->name);
}

BOOST_AUTO_TEST_CASE(GetLength)
{
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getLength(), test_length);
}

BOOST_AUTO_TEST_CASE(GetEdge)
{
  BOOST_CHECK_EQUAL(*test_tunnel_getters_and_setters.getEdge(), test_edge);
}

BOOST_AUTO_TEST_CASE(GetColor)
{
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getColor(), test_color);
}

BOOST_AUTO_TEST_CASE(GetOwner)
{
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getOwner(), &test_owner);
}

BOOST_AUTO_TEST_CASE(GetBlockStatus)
{
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getBlockStatus(), test_is_blocked);
}

BOOST_AUTO_TEST_CASE(GetVertexA)
{
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getVertexA(), &(test_stationA->vertex));
}


BOOST_AUTO_TEST_CASE(GetVertexB)
{
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.getVertexB(), &(test_stationB->vertex));
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)
playersState::Player test_set_owner = playersState::Player(4, "NewOwner", cardsState::ColorCard::BLACK, 0, 50, 5, 7, nullptr);
BOOST_AUTO_TEST_CASE(SetOwner)
{
  test_tunnel_getters_and_setters.setOwner(&test_set_owner);
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.owner, &test_set_owner);
}
bool new_block_status = true;
BOOST_AUTO_TEST_CASE(SetBlockStatus)
{
  test_tunnel_getters_and_setters.setBlockStatus(new_block_status);
  BOOST_CHECK_EQUAL(test_tunnel_getters_and_setters.isBlocked, new_block_status);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
