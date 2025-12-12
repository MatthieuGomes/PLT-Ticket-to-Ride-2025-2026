
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Ferry.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"
#include <boost/graph/adjacency_list.hpp>

using namespace ::mapState;

using StationPair = std::pair<Station *, Station *>;
using FerryDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, int, bool>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

boost::adjacency_list<> test_graph = boost::adjacency_list<>();
mapState::Station *test_stationA = new mapState::Station("StationA", new playersState::Player(1, "OwnerA", cardsState::ColorCard::RED, 0, 30, 2, 4, nullptr), false, boost::add_vertex(test_graph));
mapState::Station *test_stationB = new mapState::Station("StationB", new playersState::Player(2, "OwnerB", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr), false, boost::add_vertex(test_graph));
int test_ferry_id = 101;
bool test_is_blocked = false;
cardsState::ColorCard test_color = cardsState::ColorCard::GREEN;
int test_length = 5;
playersState::Player test_owner(3, "TestOwner", cardsState::ColorCard::YELLOW, 0, 40, 4, 6, nullptr);
boost::adjacency_list<>::edge_descriptor test_edge = boost::add_edge(*(test_stationA->getVertex()), *(test_stationB->getVertex()), test_graph).first;
int test_locomotives = 2;

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Constructor)
{
  mapState::Ferry test_ferry = mapState::Ferry(test_ferry_id, &test_owner, test_stationA, test_stationB, test_color, test_length, test_locomotives, test_is_blocked, test_edge);
  BOOST_CHECK_EQUAL(test_ferry.id, test_ferry_id);
  BOOST_CHECK_EQUAL(test_ferry.owner, &test_owner);
  BOOST_CHECK_EQUAL(test_ferry.stationA, test_stationA);
  BOOST_CHECK_EQUAL(test_ferry.stationB, test_stationB);
  BOOST_CHECK_EQUAL(test_ferry.color, test_color);
  BOOST_CHECK_EQUAL(test_ferry.length, test_length);
  BOOST_CHECK_EQUAL(test_ferry.isBlocked, test_is_blocked);
  BOOST_CHECK_EQUAL(test_ferry.edge, test_edge);
  BOOST_CHECK_EQUAL(test_ferry.locomotives, test_locomotives);
}

BOOST_AUTO_TEST_CASE(BatchConstructor)
{
  std::string stationA_name = "BatchStationA";
  std::string stationB_name = "BatchStationB";
  std::string stationC_name = "BatchStationC";
  playersState::Player stationA_owner(3, "BatchOwnerStationA", cardsState::ColorCard::RED, 0, 30, 2, 4, nullptr);
  playersState::Player stationB_owner(4, "BatchOwnerStationB", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr);
  playersState::Player stationC_owner(5, "BatchOwnerStationC", cardsState::ColorCard::GREEN, 0, 50, 4, 6, nullptr);
  mapState::Station *batch_stationA = new mapState::Station(stationA_name, &stationA_owner, false, boost::add_vertex(test_graph));
  mapState::Station *batch_stationB = new mapState::Station(stationB_name, &stationB_owner, false, boost::add_vertex(test_graph));
  mapState::Station *batch_stationC = new mapState::Station(stationC_name, &stationC_owner, false, boost::add_vertex(test_graph));
  playersState::Player batch_owner1(4, "BatchOwnerFerry1", cardsState::ColorCard::WHITE, 0, 50, 5, 7, nullptr);
  playersState::Player batch_owner2(5, "BatchOwnerFerry2", cardsState::ColorCard::BLACK, 0, 55, 6, 8, nullptr);
  playersState::Player batch_owner3(6, "BatchOwnerFerry3", cardsState::ColorCard::YELLOW, 0, 60, 7, 9, nullptr);
  std::vector<FerryInfo> ferryInfos = {
      Ferry::genData(batch_stationA, batch_stationB, 201, &batch_owner1, cardsState::ColorCard::PINK, 3, 1, false),
      Ferry::genData(batch_stationB, batch_stationC, 202, &batch_owner2, cardsState::ColorCard::ORANGE, 4, 2, true),
      Ferry::genData(batch_stationA, batch_stationC, 203, &batch_owner3, cardsState::ColorCard::BLUE, 5, 3, false),
  };
  std::vector<Ferry> ferries = Ferry::BatchConstructor(ferryInfos, &test_graph);
  BOOST_CHECK_EQUAL(ferries.size(), 3);
  for(int i = 0; i < static_cast<int>(ferries.size()); i++) {
    BOOST_CHECK_EQUAL(ferries[i].id, std::get<0>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i].owner, std::get<1>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i].stationA, std::get<0>(std::get<0>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i].stationB, std::get<1>(std::get<0>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i].color, std::get<2>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i].length, std::get<3>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i].locomotives, std::get<4>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i].isBlocked, std::get<5>(std::get<1>(ferryInfos[i])));
  }
} 

BOOST_AUTO_TEST_CASE(GenData)
{
  FerryInfo info = Ferry::genData(test_stationA, test_stationB, test_ferry_id, &test_owner, test_color, test_length, test_locomotives, test_is_blocked);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_ferry_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second), &test_owner);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  BOOST_CHECK_EQUAL(std::get<4>(info.second), test_locomotives);
  BOOST_CHECK_EQUAL(std::get<5>(info.second), test_is_blocked); 
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
mapState::Ferry test_ferry_getters_and_setters = mapState::Ferry(test_ferry_id, &test_owner, test_stationA, test_stationB, test_color, test_length, test_locomotives, test_is_blocked, test_edge);
BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetId)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getId(), test_ferry_id);
}

BOOST_AUTO_TEST_CASE(GetStationA)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getStationA(), test_stationA);
}

BOOST_AUTO_TEST_CASE(GetStationB)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getStationB(), test_stationB);
}

BOOST_AUTO_TEST_CASE(GetLength)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getLength(), test_length);
}

BOOST_AUTO_TEST_CASE(GetEdge)
{
  BOOST_CHECK_EQUAL(*test_ferry_getters_and_setters.getEdge(), test_edge);
}

BOOST_AUTO_TEST_CASE(GetColor)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getColor(), test_color);
}

BOOST_AUTO_TEST_CASE(GetOwner)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getOwner(), &test_owner);
}

BOOST_AUTO_TEST_CASE(GetBlockStatus)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getBlockStatus(), test_is_blocked);
}

BOOST_AUTO_TEST_CASE(GetVertexA)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getVertexA(), &(test_stationA->vertex));
}

BOOST_AUTO_TEST_CASE(GetVertexB)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getVertexB(), &(test_stationB->vertex));
}

BOOST_AUTO_TEST_CASE(GetLocomotives)
{
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getLocomotives(), test_locomotives);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)
playersState::Player test_set_owner = playersState::Player(4, "NewOwner", cardsState::ColorCard::BLACK, 0, 50, 5, 7, nullptr);
BOOST_AUTO_TEST_CASE(SetOwner)
{
  test_ferry_getters_and_setters.setOwner(&test_set_owner);
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.owner, &test_set_owner);
}
bool new_block_status = true;
BOOST_AUTO_TEST_CASE(SetBlockStatus)
{
  test_ferry_getters_and_setters.setBlockStatus(new_block_status);
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.isBlocked, new_block_status);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
