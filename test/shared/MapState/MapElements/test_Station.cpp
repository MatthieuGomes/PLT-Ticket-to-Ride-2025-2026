
#include <boost/test/unit_test.hpp>

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

using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

std::string test_station_name = "TestStation";
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("TestPlayer", playersState::PlayerColor::RED, 0, 45, 3, 5, nullptr);
std::shared_ptr<boost::adjacency_list<>> test_graph = std::make_shared<boost::adjacency_list<>>();
std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> test_vertex = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph));

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Empty)
{
  std::cout << "Empty Test Started ..." << std::endl;
  Station station = Station();
  BOOST_CHECK(station.name.empty());
  BOOST_CHECK_EQUAL(station.owner, nullptr);
  BOOST_CHECK_EQUAL(station.vertex, nullptr);
  std::cout << "Empty Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(Basic)
{
  std::cout << "Default Constructor Test Started ..." << std::endl;
  Station station(test_station_name, test_owner, test_vertex);
  BOOST_CHECK_EQUAL(station.owner->name, test_owner->name);
  BOOST_CHECK_EQUAL(station.name, test_station_name);
  BOOST_CHECK_EQUAL(*station.vertex, *test_vertex);
  std::cout << "Default Constructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructor)
{
  std::cout << "BatchConstructor Test Started ..." << std::endl;
  std::string test_station_name1 = "BatchStation1";
  std::shared_ptr<playersState::Player> test_owner1 = std::make_shared<playersState::Player>("BatchPlayer1", playersState::PlayerColor::RED, 0, 45, 3, 5, nullptr);
  std::string test_station_name2 = "BatchStation2";
  std::shared_ptr<playersState::Player> test_owner2 = std::make_shared<playersState::Player>("BatchPlayer2", playersState::PlayerColor::BLUE, 0, 42, 1, 3, nullptr);

  std::vector<StationInfo> stationInfos = {
      Station::genData(test_owner1, test_station_name1),
      Station::genData(test_owner2, test_station_name2),
  };
  std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationInfos, test_graph);
  BOOST_CHECK_EQUAL(stations.size(), 2);
  BOOST_CHECK_EQUAL(stations[0]->name, test_station_name1);
  BOOST_CHECK_EQUAL(stations[0]->owner->name, test_owner1->name);
  BOOST_CHECK_EQUAL(stations[1]->name, test_station_name2);
  BOOST_CHECK_EQUAL(stations[1]->owner->name, test_owner2->name);
  std::cout << "BatchConstructor Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(BatchConstructorRequiresGraph)
{
  std::vector<StationInfo> stationInfos = {
      Station::genData(test_owner, "NoGraphStation")};
  // check if exception is thrown if no graph is provided
  BOOST_CHECK_THROW(Station::BatchConstructor(stationInfos, nullptr), std::invalid_argument);

  std::shared_ptr<boost::adjacency_list<>> graph = std::make_shared<boost::adjacency_list<>>();
  std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationInfos, graph);
  BOOST_CHECK_EQUAL(stations.size(), 1);
}
BOOST_AUTO_TEST_SUITE()

BOOST_AUTO_TEST_CASE(GenData)
{
  std::cout << "GenData Test Started ..." << std::endl;
  StationInfo info = Station::genData(test_owner, test_station_name);
  BOOST_CHECK_EQUAL(std::get<0>(info)->name, test_owner->name);
  BOOST_CHECK_EQUAL(std::get<1>(info), test_station_name);
  std::cout << "GenData Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(InitData)
{
  std::cout << "InitData Test Started ..." << std::endl;
  StationInfo info = Station::initData(test_station_name);
  BOOST_CHECK_EQUAL(std::get<0>(info), nullptr);
  BOOST_CHECK_EQUAL(std::get<1>(info), test_station_name);
  std::cout << "InitData Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // DataGenerators

BOOST_AUTO_TEST_SUITE_END() // Constructors

BOOST_AUTO_TEST_SUITE(GettersAndSetters)
Station station(test_station_name, test_owner, test_vertex);
BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetName)
{
  std::cout << "GetName Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(station.getName(), test_station_name);
  std::cout << "GetName Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetOwner)
{
  std::cout << "GetOwner Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(station.getOwner()->name, test_owner->name);
  std::cout << "GetOwner Test Finished !\n"
            << std::endl;
}
using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;
using StationPair = std::pair<Station *, Station *>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int>;
using RoadInfo = std::pair<StationPair, RoadDetail>;
using TunnelDetail = RoadDetail;
using TunnelInfo = RoadInfo;
using FerryDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, int>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

BOOST_AUTO_TEST_CASE(GetVertex)
{
  std::cout << "GetVertex Test Started ..." << std::endl;
  BOOST_CHECK_EQUAL(*station.getVertex(), *test_vertex);
  std::cout << "GetVertex Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationByName)
{
  std::cout << "GetStationByName Test Started ..." << std::endl;
  std::vector<std::shared_ptr<Station>> stations = {
      std::make_shared<Station>("Station1", nullptr, test_vertex),
      std::make_shared<Station>("Station2", nullptr, test_vertex),
      std::make_shared<Station>("Station3", nullptr, test_vertex),
  };
  {
    std::cout << "found case test started ..." << std::endl;
    std::shared_ptr<Station> found_station = Station::getStationByName(stations, "Station2");
    BOOST_CHECK_NE(found_station, nullptr);
    BOOST_CHECK_EQUAL(found_station, stations[1]);
    std::cout << "found case test finished !" << std::endl;
  }
  {
    std::cout << "not found case test started ..." << std::endl;
    std::shared_ptr<Station> not_found_station = Station::getStationByName(stations, "NonExistentStation");
    BOOST_CHECK_EQUAL(not_found_station, nullptr);
    std::cout << "not found case test finished !" << std::endl;
  }
  std::cout << "GetStationByName Test Finished !\n"
            << std::endl;
}
BOOST_AUTO_TEST_CASE(getAdjacentStations)
{
  std::cout << "getAdjacentStations Test Started ..." << std::endl;
  std::shared_ptr<Station> stationA = std::make_shared<Station>("StationA", nullptr, test_vertex);
  std::shared_ptr<Station> stationB = std::make_shared<Station>("StationB", nullptr, test_vertex);
  std::shared_ptr<Station> stationC = std::make_shared<Station>("StationC", nullptr, test_vertex);
  std::shared_ptr<Road> roadAB = std::make_shared<Road>(101, nullptr, stationA, stationB, cardsState::ColorCard::RED, 3, nullptr);
  std::shared_ptr<Road> roadAC = std::make_shared<Road>(102, nullptr, stationA, stationC, cardsState::ColorCard::BLUE, 4, nullptr);
  std::vector<std::shared_ptr<Road>> roads = {roadAB, roadAC};
  {
    std::cout << "adjacent station to A case tests started ..." << std::endl;
    std::vector<std::shared_ptr<Station>> adjacent_to_A = stationA->getAdjacentStations(roads);
    BOOST_CHECK_EQUAL(adjacent_to_A.size(), 2);
    BOOST_CHECK((adjacent_to_A[0] == stationB && adjacent_to_A[1] == stationC) ||
                (adjacent_to_A[0] == stationC && adjacent_to_A[1] == stationB));
    std::cout << "adjacent stations case tests finished !" << std::endl;
  }
  {
    std::cout << "no adjacent to B case test started ..." << std::endl;
    std::vector<std::shared_ptr<Station>> adjacent_to_B = stationB->getAdjacentStations(roads);
    BOOST_CHECK_EQUAL(adjacent_to_B.size(), 1);
    BOOST_CHECK_EQUAL(adjacent_to_B[0], stationA);
    std::cout << "no adjacent to B case test finished !" << std::endl;
  }
  std::cout << "getAdjacentStations Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // Getters
BOOST_AUTO_TEST_SUITE(Setters)

std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>("NewOwner", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr);
BOOST_AUTO_TEST_CASE(SetOwner)
{
  std::cout << "SetOwner Test Started ..." << std::endl;
  station.setOwner(test_set_owner);
  BOOST_CHECK_EQUAL(station.owner->name, test_set_owner->name);
  std::cout << "SetOwner Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // Setters

BOOST_AUTO_TEST_SUITE_END() // GettersAndSetters

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE_END() // Internal

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_CASE(isClaimable)
{
  std::cout << "isClaimable Test Started ..." << std::endl;
  {
    std::cout << "not owned case test started ..." << std::endl;
    Station unowned_station("UnownedStation", nullptr, test_vertex);
    BOOST_CHECK_EQUAL(unowned_station.isClaimable(), true);
    std::cout << "not owned case test finished !" << std::endl;
  }
  {
    std::cout << "owned case test started ..." << std::endl;
    Station owned_station("OwnedStation", test_owner, test_vertex);
    BOOST_CHECK_EQUAL(owned_station.isClaimable(), false);
    std::cout << "owned case test finished !" << std::endl;
  }
  std::cout << "isClaimable Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(_display)
{
  std::cout << "_display Test Started ..." << std::endl;
  {
    std::cout << "\tnull owner case started ...  " << std::endl;
    Station station("NullOwnerStation", nullptr, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station._display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Name : NullOwnerStation") != std::string::npos);
    BOOST_CHECK(out.find("Owner : None") != std::string::npos);
    std::cout << "\tnull owner case finished !\n"
              << std::endl;
  }
  {
    std::cout << "\twith owner case started ...  " << std::endl;
    std::shared_ptr<playersState::Player> display_owner = std::make_shared<playersState::Player>("DisplayOwner", playersState::PlayerColor::GREEN, 0, 50, 4, 6, nullptr);
    Station station("DisplayStation", display_owner, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station._display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Name : DisplayStation") != std::string::npos);
    BOOST_CHECK(out.find("Owner : DisplayOwner") != std::string::npos);
    std::cout << "\twith owner case finished !\n"
              << std::endl;
  }
  {
    std::cout << "\tblocked case started ...  " << std::endl;
    Station station("BlockedStation", nullptr, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station._display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Name : BlockedStation") != std::string::npos);
    std::cout << "\tblocked status case finished !\n"
              << std::endl;
  }
  {
    std::cout << "\tnot blocked case started ...  " << std::endl;
    Station station("NotBlockedStation", nullptr, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station._display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Name : NotBlockedStation") != std::string::npos);
    std::cout << "\tnot blocked case finished !\n"
              << std::endl;
  }

  std::cout << "_display Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_CASE(display)
{
  std::cout << "display Test Started ..." << std::endl;
  {
    std::cout << "\tnormal case test started ...  " << std::endl;
    Station station("DisplayTestStation", test_owner, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station.display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Station Details:") != std::string::npos);
    BOOST_CHECK(out.find("Name : DisplayTestStation") != std::string::npos);
    BOOST_CHECK(out.find("Owner : TestPlayer") != std::string::npos);
  }
  {
    std::cout << "\tempty owner case test started ...  " << std::endl;
    Station station("DisplayTestStationNoOwner", nullptr, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station.display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Station Details:") != std::string::npos);
    BOOST_CHECK(out.find("Name : DisplayTestStationNoOwner") != std::string::npos);
    BOOST_CHECK(out.find("Owner : None") != std::string::npos);
  }
  {
    std::cout << "\tempty owner & not blocked case test started ...  " << std::endl;
    Station station("DisplayTestStationNoOwnerNotBlocked", nullptr, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station.display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Station Details:") != std::string::npos);
    BOOST_CHECK(out.find("Name : DisplayTestStationNoOwnerNotBlocked") != std::string::npos);
    BOOST_CHECK(out.find("Owner : None") != std::string::npos);
  }
  {
    std::cout << "\twith owner & blocked case test started ...  " << std::endl;
    Station station("DisplayTestStationWithOwnerBlocked", test_owner, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station.display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Station Details:") != std::string::npos);
    BOOST_CHECK(out.find("Name : DisplayTestStationWithOwnerBlocked") != std::string::npos);
    BOOST_CHECK(out.find("Owner : TestPlayer") != std::string::npos);
  }
  {
    std::cout << "\twith owner & not blocked case test started ...  " << std::endl;
    Station station("DisplayTestStationWithOwnerNotBlocked", test_owner, test_vertex);
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    station.display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    BOOST_CHECK(out.find("Station Details:") != std::string::npos);
    BOOST_CHECK(out.find("Name : DisplayTestStationWithOwnerNotBlocked") != std::string::npos);
    BOOST_CHECK(out.find("Owner : TestPlayer") != std::string::npos);
  }
  std::cout << "display Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // Interactions

BOOST_AUTO_TEST_SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
