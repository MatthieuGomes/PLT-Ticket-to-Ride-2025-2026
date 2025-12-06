
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#include "cardsState/DestinationCard.h"
using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestDestinationCard) {
  {
    DestinationCard card("paris","rome",12);

    BOOST_CHECK_EQUAL(card.getstationA(), "paris");
  }

  {

    DestinationCard card("paris","rome",12);
    BOOST_CHECK_EQUAL(card.getPoints(), 12);

  }


  {
    DestinationCard card("paris","rome",12);
    BOOST_CHECK_EQUAL(card.getstationB(), "rome");


  }
}

/* vim: set sw=2 sts=2 et : */
