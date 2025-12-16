
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"
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

using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, bool, std::string>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

std::string test_station_name = "TestStation";
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>(1, "TestPlayer", cardsState::ColorCard::RED, 0, 45, 3, 5, nullptr);
bool test_is_blocked = false;
boost::adjacency_list<> test_graph = boost::adjacency_list<>();
boost::adjacency_list<>::vertex_descriptor test_vertex = boost::add_vertex(test_graph);

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Basic)
{
  std::cout << "Default Constructor Test Started ..." << std::endl;
  Station station(test_station_name, test_owner, test_is_blocked, test_vertex);
  BOOST_CHECK_EQUAL(station.owner->name, test_owner->name);
  BOOST_CHECK_EQUAL(station.isBlocked, test_is_blocked);
  BOOST_CHECK_EQUAL(station.name, test_station_name);
  BOOST_CHECK_EQUAL(station.vertex, test_vertex);
  std::cout << "Default Constructor Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructor)
{
  std::cout << "BatchConstructor Test Started ..." << std::endl;
  std::string test_station_name1 = "BatchStation1";
  std::shared_ptr<playersState::Player> test_owner1 = std::make_shared<playersState::Player>(1, "BatchPlayer1", cardsState::ColorCard::RED, 0, 45, 3, 5, nullptr);
  bool test_is_blocked1 = false;
  std::string test_station_name2 = "BatchStation2";
  std::shared_ptr<playersState::Player> test_owner2 = std::make_shared<playersState::Player>(1, "BatchPlayer2", cardsState::ColorCard::BLUE, 0, 42, 1, 3, nullptr);
  bool test_is_blocked2 = false;

  std::vector<StationInfo> stationInfos = {
      Station::genData(test_owner1, test_is_blocked1, test_station_name1),
      Station::genData(test_owner2, test_is_blocked2, test_station_name2),
  };
  std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationInfos, &test_graph);
  BOOST_CHECK_EQUAL(stations.size(), 2);
  BOOST_CHECK_EQUAL(stations[0]->name, test_station_name1);
  BOOST_CHECK_EQUAL(stations[0]->owner->name, test_owner1->name);
  BOOST_CHECK_EQUAL(stations[0]->isBlocked, test_is_blocked1);
  BOOST_CHECK_EQUAL(stations[1]->name, test_station_name2);
  BOOST_CHECK_EQUAL(stations[1]->owner->name, test_owner2->name);
  BOOST_CHECK_EQUAL(stations[1]->isBlocked, test_is_blocked2);
  std::cout << "BatchConstructor Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GenData)
{
  std::cout << "GenData Test Started ..." << std::endl;
  StationInfo info = Station::genData(test_owner, test_is_blocked, test_station_name);
  BOOST_CHECK_EQUAL(std::get<0>(info)->name, test_owner->name);
  BOOST_CHECK_EQUAL(std::get<1>(info), test_is_blocked);
  BOOST_CHECK_EQUAL(std::get<2>(info), test_station_name);
  std::cout << "GenData Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
Station station(test_station_name, test_owner, test_is_blocked, test_vertex);
BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetName)
{
  std::cout << "GetName Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(station.getName(), test_station_name);
  std::cout << "GetName Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetOwner)
{
  std::cout << "GetOwner Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(station.getOwner()->name, test_owner->name);
  std::cout << "GetOwner Test Finished !\n"<< std::endl;
}
using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, bool, std::string>;
using StationPair = std::pair<Station *, Station *>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>;
using RoadInfo = std::pair<StationPair, RoadDetail>;
using TunnelDetail = RoadDetail;
using TunnelInfo = RoadInfo;
using FerryDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, int, bool>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

BOOST_AUTO_TEST_CASE(GetBlockStatus)
{
  std::cout << "GetBlockStatus Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(station.getBlockStatus(), test_is_blocked);
  std::cout << "GetBlockStatus Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetVertex)
{
  std::cout << "GetVertex Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*station.getVertex(), test_vertex);
  std::cout << "GetVertex Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)

std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>(2, "NewOwner", cardsState::ColorCard::BLUE, 0, 45, 3, 5, nullptr);
BOOST_AUTO_TEST_CASE(SetOwner)
{
  std::cout << "SetOwner Test Started ..." << std::endl;
  station.setOwner(test_set_owner);
  BOOST_CHECK_EQUAL(station.owner->name, test_set_owner->name);
  std::cout << "SetOwner Test Finished !\n"<< std::endl;
}
bool test_set_is_blocked = true;
BOOST_AUTO_TEST_CASE(SetBlockStatus)
{
  std::cout << "SetBlockStatus Test Started ..." << std::endl;
  station.setBlockStatus(test_set_is_blocked);
  BOOST_CHECK_EQUAL(station.isBlocked, test_set_is_blocked);
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
