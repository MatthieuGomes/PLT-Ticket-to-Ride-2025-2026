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

#define TEST(x) BOOST_AUTO_TEST_CASE(x)
#define SUITE_START(x) BOOST_AUTO_TEST_SUITE(x)
#define SUITE_END() BOOST_AUTO_TEST_SUITE_END()
#define ANN_START(x) std::cout << "Starting " << x << " test..." << std::endl;
#define ANN_END(x) std::cout << x << " test finished!" << std::endl;
#define CHECK_EQ(a, b) BOOST_CHECK_EQUAL(a, b)
#define CHECK_NE(a, b) BOOST_CHECK_NE(a, b)
#define CHECK_NTHROW(...) BOOST_CHECK_NO_THROW(__VA_ARGS__)
#define CHECK_THROW(...) BOOST_CHECK_THROW(__VA_ARGS__)
#define CHECK(x) BOOST_CHECK(x)

using namespace ::mapState;

using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;

TEST(TestStaticAssert)
{
  BOOST_CHECK(1);
}

std::string test_station_name = "TestStation";
std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("TestPlayer", playersState::PlayerColor::RED, 0, 45, 3, 5, nullptr);
std::shared_ptr<boost::adjacency_list<>> test_graph = std::make_shared<boost::adjacency_list<>>();
std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> test_vertex = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(boost::add_vertex(*test_graph));

SUITE_START(Constructors)

TEST(Default)
{
  ANN_START("Default")
  Station station = Station();
  BOOST_CHECK(station.name.empty());
  CHECK_EQ(station.owner, nullptr);
  CHECK_EQ(station.vertex, nullptr);
  ANN_END("Default")
}

TEST(Basic)
{
  ANN_START("Basic")
  Station station(test_station_name, test_owner, test_vertex);
  CHECK_EQ(station.owner->name, test_owner->name);
  CHECK_EQ(station.name, test_station_name);
  CHECK_EQ(*station.vertex, *test_vertex);
  ANN_END("Basic")
}

TEST(BatchConstructor)
{
  {
    ANN_START("BatchConstructor")
    std::string test_station_name1 = "BatchStation1";
    std::shared_ptr<playersState::Player> test_owner1 = std::make_shared<playersState::Player>("BatchPlayer1", playersState::PlayerColor::RED, 0, 45, 3, 5, nullptr);
    std::string test_station_name2 = "BatchStation2";
    std::shared_ptr<playersState::Player> test_owner2 = std::make_shared<playersState::Player>("BatchPlayer2", playersState::PlayerColor::BLUE, 0, 42, 1, 3, nullptr);

    std::vector<StationInfo> stationInfos = {
        Station::genData(test_owner1, test_station_name1),
        Station::genData(test_owner2, test_station_name2),
    };
    std::vector<std::shared_ptr<Station>> stations = Station::BatchConstructor(stationInfos, test_graph);
    CHECK_EQ(stations.size(), 2);
    CHECK_EQ(stations[0]->name, test_station_name1);
    CHECK_EQ(stations[0]->owner->name, test_owner1->name);
    CHECK_EQ(stations[1]->name, test_station_name2);
    CHECK_EQ(stations[1]->owner->name, test_owner2->name);
    ANN_END("BatchConstructor")
  }
  {
    ANN_START("BatchConstructor with no gameGraph")
    std::string test_station_name1 = "BatchStation1_NoGraph";
    std::shared_ptr<playersState::Player> test_owner1 = std::make_shared<playersState::Player>("BatchPlayer1_NoGraph", playersState::PlayerColor::RED, 0, 45, 3, 5, nullptr);
    std::vector<StationInfo> stationInfos = {
        Station::genData(test_owner1, test_station_name1),
    };
    CHECK_THROW(Station::BatchConstructor(stationInfos, nullptr), std::invalid_argument);
    ANN_END("BatchConstructor with no gameGraph")
  }
}

SUITE_START()

TEST(genData)
{
  ANN_START("genData")
  StationInfo info = Station::genData(test_owner, test_station_name);
  CHECK_EQ(std::get<0>(info)->name, test_owner->name);
  CHECK_EQ(std::get<1>(info), test_station_name);
  ANN_END("genData")
}

TEST(initData)
{
  ANN_START("initData")
  StationInfo info = Station::initData(test_station_name);
  CHECK_EQ(std::get<0>(info), nullptr);
  CHECK_EQ(std::get<1>(info), test_station_name);
  ANN_END("initData")
}

SUITE_END() // DataGenerators

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)
Station station(test_station_name, test_owner, test_vertex);
SUITE_START(Getters)

TEST(getName)
{
  ANN_START("getName")
  CHECK_EQ(station.getName(), test_station_name);
  ANN_END("getName")
}

TEST(getOwner)
{
  ANN_START("getOwner")
  CHECK_EQ(station.getOwner()->name, test_owner->name);
  ANN_END("getOwner")
}
using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;
using StationPair = std::pair<Station *, Station *>;
using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, RoadColor, int>;
using RoadInfo = std::pair<StationPair, RoadDetail>;
using TunnelDetail = RoadDetail;
using TunnelInfo = RoadInfo;
using FerryDetail = std::tuple<int, playersState::Player *, RoadColor, int, int>;
using FerryInfo = std::pair<StationPair, FerryDetail>;

TEST(getVertex)
{
  ANN_START("getVertex")
  CHECK_EQ(*station.getVertex(), *test_vertex);
  ANN_END("getVertex")
}

TEST(getStationByName)
{
  ANN_START("getStationByName")
  std::vector<std::shared_ptr<Station>> stations = {
      std::make_shared<Station>("Station1", nullptr, test_vertex),
      std::make_shared<Station>("Station2", nullptr, test_vertex),
      std::make_shared<Station>("Station3", nullptr, test_vertex),
  };
  {
    ANN_START("found case")
    std::shared_ptr<Station> found_station = Station::getStationByName(stations, "Station2");
    CHECK_NE(found_station, nullptr);
    CHECK_EQ(found_station, stations[1]);
    ANN_END("found case")
  }
  {
    ANN_START("not found case")
    std::shared_ptr<Station> not_found_station = Station::getStationByName(stations, "NonExistentStation");
    CHECK_EQ(not_found_station, nullptr);
    ANN_END("not found case")
  }
  ANN_END("getStationByName")
}

TEST(getAdjacentStations)
{
  ANN_START("getAdjacentStations")
  std::shared_ptr<Station> stationA = std::make_shared<Station>("StationA", nullptr, test_vertex);
  std::shared_ptr<Station> stationB = std::make_shared<Station>("StationB", nullptr, test_vertex);
  std::shared_ptr<Station> stationC = std::make_shared<Station>("StationC", nullptr, test_vertex);
  std::shared_ptr<Road> roadAB = std::make_shared<Road>(101, nullptr, stationA, stationB, RoadColor::RED, 3, nullptr);
  std::shared_ptr<Road> roadAC = std::make_shared<Road>(102, nullptr, stationA, stationC, RoadColor::BLUE, 4, nullptr);
  std::vector<std::shared_ptr<Road>> roads = {roadAB, roadAC};
  {
    ANN_START("adjacent stations to A case")
    std::vector<std::shared_ptr<Station>> adjacent_to_A = stationA->getAdjacentStations(roads);
    CHECK_EQ(adjacent_to_A.size(), 2);
    BOOST_CHECK((adjacent_to_A[0] == stationB && adjacent_to_A[1] == stationC) ||
                (adjacent_to_A[0] == stationC && adjacent_to_A[1] == stationB));
    ANN_END("adjacent stations to A case")
  }
  {
    ANN_START("adjacent to B case")
    std::vector<std::shared_ptr<Station>> adjacent_to_B = stationB->getAdjacentStations(roads);
    CHECK_EQ(adjacent_to_B.size(), 1);
    CHECK_EQ(adjacent_to_B[0], stationA);
    ANN_END("adjacent to B case")
  }
  ANN_END("getAdjacentStations")
}

SUITE_END() // Getters
SUITE_START(Setters)

std::shared_ptr<playersState::Player> test_set_owner = std::make_shared<playersState::Player>("NewOwner", playersState::PlayerColor::BLUE, 0, 45, 3, 5, nullptr);
TEST(setOwner)
{
  ANN_START("setOwner")
  station.setOwner(test_set_owner);
  CHECK_EQ(station.owner->name, test_set_owner->name);
  ANN_END("setOwner")
}

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)

TEST(isClaimable){
    ANN_START("isClaimable"){
        ANN_START("not owned case")
            Station unowned_station("UnownedStation", nullptr, test_vertex);
CHECK_EQ(unowned_station.isClaimable(), true);
ANN_END("not owned case")
}
{
  ANN_START("owned case")
  Station owned_station("OwnedStation", test_owner, test_vertex);
  CHECK_EQ(owned_station.isClaimable(), false);
  ANN_END("owned case")
}
ANN_END("isClaimable")
}

TEST(_display){
    ANN_START("_display"){
        ANN_START("null owner case")
            Station station("NullOwnerStation", nullptr, test_vertex);
std::stringstream buffer;
std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
station._display();
std::cout.rdbuf(old);
std::string out = buffer.str();
BOOST_CHECK(out.find("Name : NullOwnerStation") != std::string::npos);
BOOST_CHECK(out.find("Owner : None") != std::string::npos);
ANN_END("null owner case")
}
{
  ANN_START("with owner case")
  std::shared_ptr<playersState::Player> display_owner = std::make_shared<playersState::Player>("DisplayOwner", playersState::PlayerColor::GREEN, 0, 50, 4, 6, nullptr);
  Station station("DisplayStation", display_owner, test_vertex);
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  station._display();
  std::cout.rdbuf(old);
  std::string out = buffer.str();
  BOOST_CHECK(out.find("Name : DisplayStation") != std::string::npos);
  BOOST_CHECK(out.find("Owner : DisplayOwner") != std::string::npos);
  ANN_END("with owner case")
}
{
  ANN_START("blocked case")
  Station station("BlockedStation", nullptr, test_vertex);
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  station._display();
  std::cout.rdbuf(old);
  std::string out = buffer.str();
  BOOST_CHECK(out.find("Name : BlockedStation") != std::string::npos);
  ANN_END("blocked case")
}
{
  ANN_START("not blocked case")
  Station station("NotBlockedStation", nullptr, test_vertex);
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  station._display();
  std::cout.rdbuf(old);
  std::string out = buffer.str();
  BOOST_CHECK(out.find("Name : NotBlockedStation") != std::string::npos);
  ANN_END("not blocked case")
}
ANN_END("_display")
}

TEST(display){
    ANN_START("display"){
        ANN_START("normal case")
            Station station("DisplayTestStation", test_owner, test_vertex);
std::stringstream buffer;
std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
station.display();
std::cout.rdbuf(old);
std::string out = buffer.str();
BOOST_CHECK(out.find("Station Details:") != std::string::npos);
BOOST_CHECK(out.find("Name : DisplayTestStation") != std::string::npos);
BOOST_CHECK(out.find("Owner : TestPlayer") != std::string::npos);
ANN_END("normal case")
}
{
  ANN_START("empty owner case")
  Station station("DisplayTestStationNoOwner", nullptr, test_vertex);
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  station.display();
  std::cout.rdbuf(old);
  std::string out = buffer.str();
  BOOST_CHECK(out.find("Station Details:") != std::string::npos);
  BOOST_CHECK(out.find("Name : DisplayTestStationNoOwner") != std::string::npos);
  BOOST_CHECK(out.find("Owner : None") != std::string::npos);
  ANN_END("empty owner case")
}
{
  ANN_START("empty owner & not blocked case")
  Station station("DisplayTestStationNoOwnerNotBlocked", nullptr, test_vertex);
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  station.display();
  std::cout.rdbuf(old);
  std::string out = buffer.str();
  BOOST_CHECK(out.find("Station Details:") != std::string::npos);
  BOOST_CHECK(out.find("Name : DisplayTestStationNoOwnerNotBlocked") != std::string::npos);
  BOOST_CHECK(out.find("Owner : None") != std::string::npos);
  ANN_END("empty owner & not blocked case")
}
{
  ANN_START("with owner & blocked case")
  Station station("DisplayTestStationWithOwnerBlocked", test_owner, test_vertex);
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  station.display();
  std::cout.rdbuf(old);
  std::string out = buffer.str();
  BOOST_CHECK(out.find("Station Details:") != std::string::npos);
  BOOST_CHECK(out.find("Name : DisplayTestStationWithOwnerBlocked") != std::string::npos);
  BOOST_CHECK(out.find("Owner : TestPlayer") != std::string::npos);
  ANN_END("with owner & blocked case")
}
{
  ANN_START("with owner & not blocked case")
  Station station("DisplayTestStationWithOwnerNotBlocked", test_owner, test_vertex);
  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  station.display();
  std::cout.rdbuf(old);
  std::string out = buffer.str();
  BOOST_CHECK(out.find("Station Details:") != std::string::npos);
  BOOST_CHECK(out.find("Name : DisplayTestStationWithOwnerNotBlocked") != std::string::npos);
  BOOST_CHECK(out.find("Owner : TestPlayer") != std::string::npos);
  ANN_END("with owner & not blocked case")
}
ANN_END("display")
}

SUITE_END() // Interactions

SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
