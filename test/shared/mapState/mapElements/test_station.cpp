
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"
#include <boost/graph/adjacency_list.hpp>

using namespace ::mapState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

std::string test_station_name = "TestStation";
playersState::Player test_owner(1, "TestPlayer", cardsState::ColorCard::RED, 0, 45, 3, 5, nullptr);
bool test_is_blocked = false;
boost::adjacency_list<> test_graph = boost::adjacency_list<>();
boost::adjacency_list<>::vertex_descriptor test_vertex = boost::add_vertex(test_graph);

BOOST_AUTO_TEST_CASE(TestConstructor)
{

  Station station(test_station_name, &test_owner, test_is_blocked, test_vertex);
  BOOST_CHECK_EQUAL(station.owner, &test_owner);
  BOOST_CHECK_EQUAL(station.isBlocked, test_is_blocked);
  BOOST_CHECK_EQUAL(station.name, test_station_name);
  BOOST_CHECK_EQUAL(station.vertex, test_vertex);
}
BOOST_AUTO_TEST_SUITE(TestGettersAndSetters)
Station station(test_station_name, &test_owner, test_is_blocked, test_vertex);
BOOST_AUTO_TEST_SUITE(TestGetters)

BOOST_AUTO_TEST_CASE(TestGetName)
{
  BOOST_CHECK_EQUAL(station.getName(), test_station_name);
}

BOOST_AUTO_TEST_CASE(TestGetOwner)
{
  BOOST_CHECK_EQUAL(station.getOwner(), &test_owner);
}

BOOST_AUTO_TEST_CASE(TestGetBlockStatus)
{
  BOOST_CHECK_EQUAL(station.getBlockStatus(), test_is_blocked);
}

BOOST_AUTO_TEST_CASE(TestGetVertex)
{
  BOOST_CHECK_EQUAL(*station.getVertex(), test_vertex);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(TestSetters)

playersState::Player test_set_owner = playersState::Player(2, "NewOwner", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr);
BOOST_AUTO_TEST_CASE(TestSetOwner)
{
  station.setOwner(&test_set_owner);
  BOOST_CHECK_EQUAL(station.owner, &test_set_owner);
}
bool test_set_is_blocked = true;
BOOST_AUTO_TEST_CASE(TestSetBlockStatus)
{
  station.setBlockStatus(test_set_is_blocked);
  BOOST_CHECK_EQUAL(station.isBlocked, test_set_is_blocked);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
