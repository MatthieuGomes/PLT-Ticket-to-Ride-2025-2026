
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
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

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

std::string test_stationA = "Paris";
std::string test_stationB = "Rome";
int test_points = 12;

BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetStationA)
{
  {
    std::cout << "Get StationA Test Started ..." << std::endl;
    DestinationCard card(test_stationA,test_stationB,test_points);
    BOOST_CHECK_EQUAL(card.getstationA(), test_stationA);
    std::cout << "Get StationA Test Finished !\n" << std::endl;
  }
}
BOOST_AUTO_TEST_CASE(GetStationB)
{
  {
    std::cout << "Get StationB Test Started ..." << std::endl;
    DestinationCard card(test_stationA,test_stationB,test_points);
    BOOST_CHECK_EQUAL(card.getstationB(), test_stationB);
    std::cout << "Get StationB Test Finished !\n" << std::endl;
  }
}
BOOST_AUTO_TEST_CASE(GetPoints)
{
  {
    std::cout << "Get Points Test Started ..." << std::endl;
    DestinationCard card(test_stationA,test_stationB,test_points);
    BOOST_CHECK_EQUAL(card.getPoints(), test_points);
    std::cout << "Get Points Test Finished !\n" << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
