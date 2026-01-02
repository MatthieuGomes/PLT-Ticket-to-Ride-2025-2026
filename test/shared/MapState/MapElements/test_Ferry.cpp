
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

using namespace ::mapState;

using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, int, bool>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

std::shared_ptr<boost::adjacency_list<>> test_graph = std::make_shared<boost::adjacency_list<>>();
std::shared_ptr<mapState::Station> test_stationA = std::make_shared<mapState::Station>("StationA", std::make_shared<playersState::Player>("OwnerA", playersState::PlayerColor::RED, 0, 30, 2, 4, nullptr), false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
std::shared_ptr<mapState::Station> test_stationB = std::make_shared<mapState::Station>("StationB", std::make_shared<playersState::Player>("OwnerB", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr), false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
int test_ferry_id = 101;
bool test_is_blocked = false;
cardsState::ColorCard test_color = cardsState::ColorCard::GREEN;
int test_length = 5;
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("TestOwner", playersState::PlayerColor::YELLOW, 0, 40, 4, 6, nullptr);
std::shared_ptr<boost::adjacency_list<>::edge_descriptor> test_edge = std::make_shared<boost::adjacency_list<>::edge_descriptor>(
    boost::add_edge(*(test_stationA->getVertex().get()), *(test_stationB->getVertex().get()), *test_graph).first);
int test_locomotives = 2;

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Default)
{
  std::cout << "Default Constructor Test Started ..." << std::endl;
  mapState::Ferry test_ferry = mapState::Ferry(test_ferry_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_locomotives, test_is_blocked, test_edge);
  BOOST_CHECK_EQUAL(test_ferry.id, test_ferry_id);
  BOOST_CHECK_EQUAL(test_ferry.owner->name, test_owner->name);
  BOOST_CHECK_EQUAL(test_ferry.stationA, test_stationA);
  BOOST_CHECK_EQUAL(test_ferry.stationB, test_stationB);
  BOOST_CHECK_EQUAL(test_ferry.color, test_color);
  BOOST_CHECK_EQUAL(test_ferry.length, test_length);
  BOOST_CHECK_EQUAL(test_ferry.isBlocked, test_is_blocked);
  BOOST_CHECK_EQUAL(*test_ferry.edge, *test_edge);
  BOOST_CHECK_EQUAL(test_ferry.locomotives, test_locomotives);
  std::cout << "Default Constructor Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructor)
{
  std::cout << "BatchConstructor Test Started ..." << std::endl;
  std::string stationA_name = "BatchStationA";
  std::string stationB_name = "BatchStationB";
  std::string stationC_name = "BatchStationC";
  std::shared_ptr<playersState::Player> stationA_owner = std::make_shared<playersState::Player>  ("BatchOwnerStationA", playersState::PlayerColor::RED, 0, 30, 2, 4, nullptr);
  std::shared_ptr<playersState::Player> stationB_owner = std::make_shared<playersState::Player>("BatchOwnerStationB", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr);
  std::shared_ptr<playersState::Player> stationC_owner = std::make_shared<playersState::Player>("BatchOwnerStationC", playersState::PlayerColor::GREEN, 0, 50, 4, 6, nullptr);
  std::shared_ptr<mapState::Station> batch_stationA = std::make_shared<mapState::Station>(stationA_name, stationA_owner, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
  std::shared_ptr<mapState::Station> batch_stationB = std::make_shared<mapState::Station>(stationB_name, stationB_owner, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
  std::shared_ptr<mapState::Station> batch_stationC = std::make_shared<mapState::Station>(stationC_name, stationC_owner, false, std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph)));
  std::shared_ptr<playersState::Player> batch_owner1 = std::make_shared<playersState::Player>("BatchOwnerFerry1", playersState::PlayerColor::RED, 0, 50, 5, 7, nullptr);
  std::shared_ptr<playersState::Player> batch_owner2 = std::make_shared<playersState::Player>("BatchOwnerFerry2", playersState::PlayerColor::BLACK, 0, 55, 6, 8, nullptr);
  std::shared_ptr<playersState::Player> batch_owner3 = std::make_shared<playersState::Player>("BatchOwnerFerry3", playersState::PlayerColor::YELLOW, 0, 60, 7, 9, nullptr);
  std::vector<FerryInfo> ferryInfos = {
      Ferry::genData(batch_stationA, batch_stationB, 201, batch_owner1, cardsState::ColorCard::PINK, 3, 1, false),
      Ferry::genData(batch_stationB, batch_stationC, 202, batch_owner2, cardsState::ColorCard::ORANGE, 4, 2, true),
      Ferry::genData(batch_stationA, batch_stationC, 203, batch_owner3, cardsState::ColorCard::BLUE, 5, 3, false),
  };
  std::vector<std::shared_ptr<Ferry>> ferries = Ferry::BatchConstructor(ferryInfos, test_graph);
  BOOST_CHECK_EQUAL(ferries.size(), 3);
  for(int i = 0; i < static_cast<int>(ferries.size()); i++) {
    BOOST_CHECK_EQUAL(ferries[i]->id, std::get<0>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i]->owner->name, std::get<1>(std::get<1>(ferryInfos[i]))->name);
    BOOST_CHECK_EQUAL(ferries[i]->stationA, std::get<0>(std::get<0>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i]->stationB, std::get<1>(std::get<0>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i]->color, std::get<2>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i]->length, std::get<3>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i]->locomotives, std::get<4>(std::get<1>(ferryInfos[i])));
    BOOST_CHECK_EQUAL(ferries[i]->isBlocked, std::get<5>(std::get<1>(ferryInfos[i])));
  }
  std::cout << "BatchConstructor Test Finished !\n"<< std::endl;
} 

BOOST_AUTO_TEST_CASE(BatchConstructorRequiresGraph)
{
  std::vector<FerryInfo> ferryInfos = {
      Ferry::genData(test_stationA, test_stationB, 501, test_owner, cardsState::ColorCard::GREEN, test_length, test_locomotives, false)};
  // check if exception is thrown if no graph is provided
  BOOST_CHECK_THROW(Ferry::BatchConstructor(ferryInfos, nullptr), std::invalid_argument);

  std::vector<std::shared_ptr<Ferry>> ferries = Ferry::BatchConstructor(ferryInfos, test_graph);
  BOOST_CHECK_EQUAL(ferries.size(), 1);
  BOOST_CHECK_EQUAL(ferries[0]->id, 501);
}

BOOST_AUTO_TEST_CASE(GenData)
{
  std::cout << "GenData Test Started ..." << std::endl;
  FerryInfo info = Ferry::genData(test_stationA, test_stationB, test_ferry_id, test_owner, test_color, test_length, test_locomotives, test_is_blocked);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_ferry_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second)->name, test_owner->name);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  BOOST_CHECK_EQUAL(std::get<4>(info.second), test_locomotives);
  BOOST_CHECK_EQUAL(std::get<5>(info.second), test_is_blocked); 
  std::cout << "GenData Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
mapState::Ferry test_ferry_getters_and_setters = mapState::Ferry(test_ferry_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_locomotives, test_is_blocked, test_edge);
BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetId)
{
  std::cout << "GetId Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getId(), test_ferry_id);
  std::cout << "GetId Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationA)
{
  std::cout << "GetStationA Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getStationA(), test_stationA);
  std::cout << "GetStationA Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationB)
{
  std::cout << "GetStationB Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getStationB(), test_stationB);
  std::cout << "GetStationB Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetLength)
{
  std::cout << "GetLength Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getLength(), test_length);
  std::cout << "GetLength Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetEdge)
{
  std::cout << "GetEdge Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_ferry_getters_and_setters.getEdge(), *test_edge);
  std::cout << "GetEdge Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetColor)
{
  std::cout << "GetColor Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getColor(), test_color);
  std::cout << "GetColor Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetOwner)
{
  std::cout << "GetOwner Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getOwner()->name, test_owner->name);
  std::cout << "GetOwner Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetBlockStatus)
{
  std::cout << "GetBlockStatus Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getBlockStatus(), test_is_blocked);
  std::cout << "GetBlockStatus Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetVertexA)
{
  std::cout << "GetVertexA Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_ferry_getters_and_setters.getVertexA(), *(test_stationA->vertex));
  std::cout << "GetVertexA Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetVertexB)
{
  std::cout << "GetVertexB Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_ferry_getters_and_setters.getVertexB(), *(test_stationB->vertex));
  std::cout << "GetVertexB Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_CASE(GetLocomotives)
{
  std::cout << "GetLocomotives Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.getLocomotives(), test_locomotives);
  std::cout << "GetLocomotives Test Finished !\n"<< std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)
std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>("NewOwner", playersState::PlayerColor::BLACK, 0, 50, 5, 7, nullptr);
BOOST_AUTO_TEST_CASE(SetOwner)
{
  std::cout << "SetOwner Test Started ..." << std::endl;
  test_ferry_getters_and_setters.setOwner(test_set_owner);
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.owner->name, test_set_owner->name);
  std::cout << "SetOwner Test Finished !\n"<< std::endl;
}
bool new_block_status = true;
BOOST_AUTO_TEST_CASE(SetBlockStatus)
{
  std::cout << "SetBlockStatus Test Started ..." << std::endl;
  test_ferry_getters_and_setters.setBlockStatus(new_block_status);
  BOOST_CHECK_EQUAL(test_ferry_getters_and_setters.isBlocked, new_block_status);
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
