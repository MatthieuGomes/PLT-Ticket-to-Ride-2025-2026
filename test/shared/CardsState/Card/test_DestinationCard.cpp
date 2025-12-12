
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#include "cardsState/DestinationCard.h"
using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructor)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetStationA)
{
  {
    DestinationCard card("paris","rome",12);
    BOOST_CHECK_EQUAL(card.getstationA(), "paris");
  }
}
BOOST_AUTO_TEST_CASE(GetStationB)
{
  {
    DestinationCard card("paris","rome",12);
    BOOST_CHECK_EQUAL(card.getstationB(), "rome");
  }
}
BOOST_AUTO_TEST_CASE(GetPoints)
{
  {
    DestinationCard card("paris","rome",12);
    BOOST_CHECK_EQUAL(card.getPoints(), 12);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
