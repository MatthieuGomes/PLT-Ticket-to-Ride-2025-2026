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

using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructor)

// ADD TESTS FOR CONSTRUCTOR

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)


std::string test_stationA_name = "Paris";
std::string test_stationB_name = "Rome";
int test_points = 12;

std::shared_ptr<playersState::Player> test_owner =
    std::make_shared<playersState::Player>(1, "TestPlayer",
        cardsState::ColorCard::RED, 0, 45, 3, 5, nullptr);

bool test_is_blocked = false;


std::shared_ptr<boost::adjacency_list<>> graph =
    std::make_shared<boost::adjacency_list<>>();

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));


std::shared_ptr<Station> stationA =
    std::make_shared<Station>(test_stationA_name, test_owner, test_is_blocked, vertexA);

std::shared_ptr<Station> stationB =
    std::make_shared<Station>(test_stationB_name, test_owner, test_is_blocked, vertexB);


BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetStationA)
{
    std::cout << "Get StationA Test Started ..." << std::endl;

    DestinationCard card(stationA, stationB, test_points);
    BOOST_CHECK_EQUAL(card.getstationA()->getName(), test_stationA_name);

    std::cout << "Get StationA Test Finished !\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(GetStationB)
{
    std::cout << "Get StationB Test Started ..." << std::endl;

    DestinationCard card(stationA, stationB, test_points);
    BOOST_CHECK_EQUAL(card.getstationB()->getName(), test_stationB_name);

    std::cout << "Get StationB Test Finished !\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(GetPoints)
{
    std::cout << "Get Points Test Started ..." << std::endl;

    DestinationCard card(stationA, stationB, test_points);
    BOOST_CHECK_EQUAL(card.getPoints(), test_points);

    std::cout << "Get Points Test Finished !\n" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
