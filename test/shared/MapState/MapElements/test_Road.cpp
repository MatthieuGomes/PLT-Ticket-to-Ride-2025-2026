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

using namespace ::mapState;

using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, RoadColor, int>;
using RoadInfo = std::pair<StationPair, RoadDetail>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
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

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Empty)
{
  std::cout << "Empty Player Constructor Test Started ..." << std::endl;
      Road road;
      BOOST_CHECK_EQUAL(road.id, -1);
      BOOST_CHECK_EQUAL(road.owner, nullptr);
      BOOST_CHECK_EQUAL(road.stationA, nullptr);
      BOOST_CHECK_EQUAL(road.stationB, nullptr);
      BOOST_CHECK_EQUAL(road.color, RoadColor::UNKNOWN);
      BOOST_CHECK_EQUAL(road.length, -1);
      BOOST_CHECK_EQUAL(road.edge, nullptr);
      std::cout << "Empty Player Constructor Test Finished !\n"
                << std::endl;
}

BOOST_AUTO_TEST_CASE(Basic)
{
  std::cout << "Default Constructor Test Started ..." << std::endl;
  mapState::Road test_road = mapState::Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
  BOOST_CHECK_EQUAL(test_road.id, test_road_id);
  BOOST_CHECK_EQUAL(test_road.owner->name, test_owner->name);
  BOOST_CHECK_EQUAL(test_road.stationA, test_stationA);
  BOOST_CHECK_EQUAL(test_road.stationB, test_stationB);
  BOOST_CHECK_EQUAL(test_road.color, test_color);
  BOOST_CHECK_EQUAL(test_road.length, test_length);
  BOOST_CHECK_EQUAL(*test_road.edge, *test_edge);
  std::cout << "Default Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(Init)
{
  std::cout << "Init Constructor Test Started ..." << std::endl;
  mapState::Road test_road = mapState::Road::Init(test_road_id, test_stationA, test_stationB, test_color, test_length, test_graph);
  BOOST_CHECK_EQUAL(test_road.id, test_road_id);
  BOOST_CHECK_EQUAL(test_road.owner, nullptr);
  BOOST_CHECK_EQUAL(test_road.stationA, test_stationA);
  BOOST_CHECK_EQUAL(test_road.stationB, test_stationB);
  BOOST_CHECK_EQUAL(test_road.color, test_color);
  BOOST_CHECK_EQUAL(test_road.length, test_length);
  BOOST_CHECK(test_road.edge != nullptr);
  std::cout << "Init Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructor)
{
  std::cout << "BatchConstructor Test Started ..." << std::endl;
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
  BOOST_CHECK_EQUAL(roads.size(), 3);
  for (int i = 0; i < 3; i++)
  {
    BOOST_CHECK_EQUAL(roads[i]->id, std::get<0>(std::get<1>(roadInfos[i])));
    BOOST_CHECK_EQUAL(roads[i]->owner->name, std::get<1>(std::get<1>(roadInfos[i]))->name);
    BOOST_CHECK_EQUAL(roads[i]->stationA, std::get<0>(std::get<0>(roadInfos[i])));
    BOOST_CHECK_EQUAL(roads[i]->stationB, std::get<1>(std::get<0>(roadInfos[i])));
    BOOST_CHECK_EQUAL(roads[i]->color, std::get<2>(std::get<1>(roadInfos[i])));
    BOOST_CHECK_EQUAL(roads[i]->length, std::get<3>(std::get<1>(roadInfos[i])));
  }
  std::cout << "BatchConstructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructorRequiresGraph)
{
  std::vector<RoadInfo> roadInfos = {
      Road::genData(test_stationA, test_stationB, 301, test_owner, RoadColor::GREEN, 5)};
  // check if exception is thrown if no graph is provided
  BOOST_CHECK_THROW(Road::BatchConstructor(roadInfos, nullptr), std::invalid_argument);

  std::vector<std::shared_ptr<mapState::Road>> roads = mapState::Road::BatchConstructor(roadInfos, test_graph);
  BOOST_CHECK_EQUAL(roads.size(), 1);
  BOOST_CHECK_EQUAL(roads[0]->id, 301);
}


BOOST_AUTO_TEST_SUITE(DataGenerators)
BOOST_AUTO_TEST_CASE(GenData)
{
  std::cout << "GenData Test Started ..." << std::endl;
  RoadInfo info = Road::genData(test_stationA, test_stationB, test_road_id, test_owner, test_color, test_length);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_road_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second)->name, test_owner->name);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  std::cout << "GenData Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(InitData){
  std::cout << "InitData Test Started ..." << std::endl;
  RoadInfo info = Road::initData(test_stationA, test_stationB, test_road_id, test_color, test_length);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_road_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second), nullptr);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  std::cout << "InitData Test Finished !\n"
            << std::endl;  
}

BOOST_AUTO_TEST_CASE(GenDataByName){
  std::cout << "GenDataByName Test Started ..." << std::endl;
  RoadInfo info = Road::genDataByName({test_stationA, test_stationB}, test_stationA->name, test_stationB->name, test_road_id, test_owner, test_color, test_length);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_road_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second)->name, test_owner->name);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  std::cout << "GenDataByName Test Finished !\n"
            << std::endl;  
}
BOOST_AUTO_TEST_CASE(InitDataByName){
  std::cout << "InitDataByName Test Started ..." << std::endl;
  RoadInfo info = Road::initDataByName({test_stationA, test_stationB}, test_stationA->name, test_stationB->name, test_road_id, test_color, test_length);
  BOOST_CHECK_EQUAL(info.first.first, test_stationA);
  BOOST_CHECK_EQUAL(info.first.second, test_stationB);
  BOOST_CHECK_EQUAL(std::get<0>(info.second), test_road_id);
  BOOST_CHECK_EQUAL(std::get<1>(info.second), nullptr);
  BOOST_CHECK_EQUAL(std::get<2>(info.second), test_color);
  BOOST_CHECK_EQUAL(std::get<3>(info.second), test_length);
  std::cout << "InitDataByName Test Finished !\n"
            << std::endl;  
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
mapState::Road test_road_getters_and_setters = mapState::Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetId)
{
  std::cout << "GetId Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_road_getters_and_setters.getId(), test_road_id);
  std::cout << "GetId Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationA)
{
  std::cout << "GetStationA Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_road_getters_and_setters.getStationA()->name, test_stationA->name);
  std::cout << "GetStationA Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationB)
{
  std::cout << "GetStationB Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_road_getters_and_setters.getStationB()->name, test_stationB->name);
  std::cout << "GetStationB Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetLength)
{
  std::cout << "GetLength Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_road_getters_and_setters.getLength(), test_length);
  std::cout << "GetLength Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetEdge)
{
  std::cout << "GetEdge Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_road_getters_and_setters.getEdge(), *test_edge);
  std::cout << "GetEdge Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetColor)
{
  std::cout << "GetColor Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_road_getters_and_setters.getColor(), test_color);
  std::cout << "GetColor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetOwner)
{
  std::cout << "GetOwner Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(test_road_getters_and_setters.getOwner()->name, test_owner->name);
  std::cout << "GetOwner Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetVertexA)
{
  std::cout << "GetVertexA Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_road_getters_and_setters.getVertexA(), *(test_stationA->vertex));
  std::cout << "GetVertexA Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetVertexB)
{
  std::cout << "GetVertexB Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*test_road_getters_and_setters.getVertexB(), *(test_stationB->vertex));
  std::cout << "GetVertexB Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)
std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>("NewOwner", playersState::PlayerColor::BLACK, 0, 50, 5, 7, nullptr);
BOOST_AUTO_TEST_CASE(SetOwner)
{
  std::cout << "SetOwner Test Started ..." << std::endl;
  test_road_getters_and_setters.setOwner(test_set_owner);
  BOOST_CHECK_EQUAL(test_road_getters_and_setters.owner->name, test_set_owner->name);
  std::cout << "SetOwner Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_CASE(isClaimable)
{
  std::cout << "isClaimable Test Started ..." << std::endl;
  int smallPartySize = 3;
  int largePartySize = 4;
  {
    std::cout << "owned road case test started ..." << std::endl;
    Road ownedRoad = Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
    std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad)};
    BOOST_CHECK_EQUAL(ownedRoad.isClaimable(roads, smallPartySize, test_owner), false);
    std::cout << "owned road case test finished !" << std::endl;
  }
  {
    std::cout << "normal claimable road case test started ..." << std::endl;
    Road claimableRoad = Road(test_road_id, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
    std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(claimableRoad)};
    BOOST_CHECK_EQUAL(claimableRoad.isClaimable(roads, smallPartySize, test_owner), true);
    std::cout << "normal claimable road case test finished !" << std::endl;
  }
  {
    std::cout << "double road case tests started ..." << std::endl;
    {
      std::cout << "owned by himself case tests started ..." << std::endl;
      {
        std::cout << "\tsmall party size case test started ..." << std::endl;
        Road ownedRoad = Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
        Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
        std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
        BOOST_CHECK_EQUAL(otherRoad.isClaimable(roads, smallPartySize, test_owner), false);
        std::cout << "\tsmall party size case tests finished !" << std::endl;
      }
      {
        std::cout << "\tlarge party size case test started ..." << std::endl;
        Road ownedRoad = Road(test_road_id, test_owner, test_stationA, test_stationB, test_color, test_length, test_edge);
        Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
        std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
        BOOST_CHECK_EQUAL(otherRoad.isClaimable(roads, largePartySize, test_owner), false);
        std::cout << "\tlarge party size case tests finished !" << std::endl;
      }
      std::cout << "owned by himself case tests finished !" << std::endl;
    }
    {
      std::cout << "\towned by another case tests started ..." << std::endl;
      {
        std::cout << "\tsmall party size case test started ..." << std::endl;
        Road ownedRoad = Road(test_road_id, std::make_shared<playersState::Player>("AnotherOwner", playersState::PlayerColor::GREEN, 0, 35, 2, 4, nullptr), test_stationA, test_stationB, test_color, test_length, test_edge);
        Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
        std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
        BOOST_CHECK_EQUAL(otherRoad.isClaimable(roads, smallPartySize, test_owner), false);
        std::cout << "\tsmall party size case tests finished !" << std::endl;
      }
      {
        std::cout << "\tlarge party size case test started ..." << std::endl;
        Road ownedRoad = Road(test_road_id, std::make_shared<playersState::Player>("AnotherOwner", playersState::PlayerColor::GREEN, 0, 35, 2, 4, nullptr), test_stationA, test_stationB, test_color, test_length, test_edge);
        Road otherRoad = Road(test_road_id + 1, nullptr, test_stationA, test_stationB, test_color, test_length, test_edge);
        std::vector<std::shared_ptr<Road>> roads = {std::make_shared<Road>(ownedRoad), std::make_shared<Road>(otherRoad)};
        BOOST_CHECK_EQUAL(otherRoad.isClaimable(roads, largePartySize, test_owner), true);
        std::cout << "\tlarge party size case tests finished !" << std::endl;
      }
      std::cout << "\towned by another case tests finished !" << std::endl;
    }
    std::cout << "double road case tests finished !" << std::endl;
  }
  std::cout << "isClaimable Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(getClaimableRoads)
{
  std::cout << "getClaimableRoads Test Started ..." << std::endl;
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
    std::cout << "\tsmall party size case test started ..." << std::endl;
    std::vector<std::shared_ptr<mapState::Road>> claimableRoads = mapState::Road::getClaimableRoads(roads, smallPartySize, test_owner);
    BOOST_CHECK_EQUAL(claimableRoads.size(), 2);
    std::cout << "\tsmall party size case test finished !" << std::endl;
  }
  {
    std::cout << "\tlarge party size case test started ..." << std::endl;
    std::vector<std::shared_ptr<mapState::Road>> claimableRoads = mapState::Road::getClaimableRoads(roads, largePartySize, test_owner);
    BOOST_CHECK_EQUAL(claimableRoads.size(), 2);
    std::cout << "\tlarge party size case tests finished !" << std::endl;
  }
  std::cout << "getClaimableRoads Test Finished !\n"
            << std::endl;
}
// TODO : Add tests for display and _display methods
BOOST_AUTO_TEST_CASE(_display)
{
  std::cout << "_display Test Started ..." << std::endl;
  std::cout << "_display Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(display)
{
  std::cout << "display Test Started ..." << std::endl;
  std::cout << "display Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
