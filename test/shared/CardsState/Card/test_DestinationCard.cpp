#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/cardsState/DestinationCard.h"
#include "../../src/shared/mapState/Station.h"
#include "../../src/shared/playersState/Player.h"

#include <boost/graph/adjacency_list.hpp>
#include <memory>

using namespace cardsState;
using namespace mapState;
#include "cardsState/DestinationCard.h"

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

using namespace ::cardsState;

TEST(TestStaticAssert)
{
    CHECK(1);
}

SUITE_START(Constructors)

TEST(Default)
{
    ANN_START("Default Constructor")
    DestinationCard card;
    CHECK_EQ(card.stationA, nullptr);
    CHECK_EQ(card.stationB, nullptr);
    CHECK_EQ(card.points, 0);
    CHECK_EQ(card.isLong, false);
    ANN_END("Default Constructor")
}

SUITE_START(BatchConstructors)

TEST(BatchConstructor)
{
    ANN_START("BatchConstructor")
    std::vector<std::shared_ptr<Road>> borrowedRoads;
    std::shared_ptr<playersState::Player> test_owner =
        std::make_shared<playersState::Player>("TestPlayer",
                                               playersState::PlayerColor::RED, 0, 45, 3,borrowedRoads , nullptr);

    std::shared_ptr<boost::adjacency_list<>> graph =
        std::make_shared<boost::adjacency_list<>>();

    std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));

    std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));

    std::shared_ptr<Station> stationA =
        std::make_shared<Station>("StationA", test_owner, vertexA);

    std::shared_ptr<Station> stationB =
        std::make_shared<Station>("StationB", test_owner, vertexB);

    std::vector<std::tuple<std::shared_ptr<Station>, std::shared_ptr<Station>, int, bool>> infos = {
        std::make_tuple(stationA, stationB, 10, false),
        std::make_tuple(stationB, stationA, 15, true)};

    std::vector<std::shared_ptr<DestinationCard>> destinationCards = DestinationCard::BatchConstructor(infos);
    CHECK_EQ(destinationCards.size(), 2);
    ANN_END("BatchConstructor")
}

TEST(BatchConstructorByName)
{
    ANN_START("BatchConstructorByName")
    std::vector<std::shared_ptr<Road>> borrowedRoads;
    std::shared_ptr<playersState::Player> test_owner =
        std::make_shared<playersState::Player>("TestPlayer",
                                               playersState::PlayerColor::RED, 0, 45, 3, borrowedRoads, nullptr);

    std::shared_ptr<boost::adjacency_list<>> graph =
        std::make_shared<boost::adjacency_list<>>();

    std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));

    std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));

    std::shared_ptr<Station> stationA =
        std::make_shared<Station>("StationA", test_owner, vertexA);

    std::shared_ptr<Station> stationB =
        std::make_shared<Station>("StationB", test_owner, vertexB);

    std::vector<std::shared_ptr<Station>> stations = {stationA, stationB};

    std::vector<std::tuple<std::string, std::string, int, bool>> infos = {
        std::make_tuple("StationA", "StationB", 10, false),
        std::make_tuple("StationB", "StationA", 15, true)};

    std::vector<std::shared_ptr<DestinationCard>> destinationCards = DestinationCard::BatchConstructorByName(infos, stations);
    CHECK_EQ(destinationCards.size(), 2);
    ANN_END("BatchConstructorByName")
}

TEST(Europe)
{
    ANN_START("Europe Constructor")
    mapState::MapState map = mapState::MapState::NamedMapState("europe");
    std::vector<std::shared_ptr<mapState::Station>> stations = map.getStations();
    std::vector<std::shared_ptr<DestinationCard>> destinationCards = DestinationCard::Europe(stations);
    CHECK_EQ(destinationCards.size(), 46);
    ANN_END("Europe Constructor")
}

SUITE_END() // BatchConstructors

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)

std::string test_stationA_name = "Paris";
std::string test_stationB_name = "Rome";
int test_points = 12;
std::vector<std::shared_ptr<Road>> borrowedRoads;

std::shared_ptr<playersState::Player> test_owner =
    std::make_shared<playersState::Player>("TestPlayer",
                                           playersState::PlayerColor::RED, 0, 45, 3, borrowedRoads, nullptr);

std::shared_ptr<boost::adjacency_list<>> graph =
    std::make_shared<boost::adjacency_list<>>();

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));

std::shared_ptr<Station> stationA =
    std::make_shared<Station>(test_stationA_name, test_owner, vertexA);

std::shared_ptr<Station> stationB =
    std::make_shared<Station>(test_stationB_name, test_owner, vertexB);

SUITE_START(Getters)

TEST(getStationA)
{
    ANN_START("getStationA")

    DestinationCard card(stationA, stationB, test_points, false);
    CHECK_EQ(card.getstationA()->getName(), test_stationA_name);

    ANN_END("getStationA")
}

TEST(getStationB)
{
    ANN_START("getStationB")

    DestinationCard card(stationA, stationB, test_points, false);
    CHECK_EQ(card.getstationB()->getName(), test_stationB_name);

    ANN_END("getStationB")
}

TEST(getPoints)
{
    ANN_START("getPoints")

    DestinationCard card(stationA, stationB, test_points, false);
    CHECK_EQ(card.getPoints(), test_points);

    ANN_END("getPoints")
}

SUITE_END() // Getters

SUITE_START(Setters)

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)
// TODO : add display and _display tests
TEST(Display)
{
    ANN_START("display");

    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

    std::shared_ptr<Station> stationA =
        std::make_shared<Station>("StationA", nullptr, nullptr);
    std::shared_ptr<Station> stationB =
        std::make_shared<Station>("StationB", nullptr, nullptr);
    int test_points = 10;
    DestinationCard card(stationA, stationB, test_points, false);
    card.display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    CHECK(out.find("----- DESTINATION CARD -----" )!= std::string::npos);
    CHECK(out.find("-------------------------" )!= std::string::npos);

    ANN_END("display");
}
TEST(_Display)
{
    ANN_START("_display");
    std::shared_ptr<Station> stationA =
        std::make_shared<Station>("StationA", nullptr, nullptr);
    std::shared_ptr<Station> stationB =
        std::make_shared<Station>("StationB", nullptr, nullptr);
    int test_points = 10;
    DestinationCard card(stationA, stationB, test_points, false);

    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

    card.display();
    std::cout.rdbuf(old);

    std::string out = buffer.str();
    CHECK(out.find("Station A: StationA") != std::string::npos);
    CHECK(out.find("Station B: StationB") != std::string::npos);
    CHECK(out.find("Points: 10") != std::string::npos);
    CHECK(out.find("Is Long: No") != std::string::npos);
    ANN_END("_display");

}
TEST(_Display_None)
{
    ANN_START("_display with None stations");

    std::shared_ptr<Station> stationA = nullptr;
    std::shared_ptr<Station> stationB = nullptr;

    int test_points = 5;
    DestinationCard card(stationA, stationB, test_points, true);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    card.display();
    std::cout.rdbuf(old);

    std::string out = buffer.str();

    CHECK(out.find("Station A: None") != std::string::npos);
    CHECK(out.find("Station B: None") != std::string::npos);
    CHECK(out.find("Points: 5") != std::string::npos);
    CHECK(out.find("Is Long: Yes") != std::string::npos);
    ANN_END("_display_None");
}


SUITE_END() // Interactions

SUITE_END() // Operations
