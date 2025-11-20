
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"


using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestWagonCard)
{
  {
    WagonCard card(ColorCard::RED);
    BOOST_CHECK_EQUAL(card.color, ColorCard::RED);

  }
  {
    WagonCard card(ColorCard::BLUE);
    BOOST_CHECK_EQUAL(card.getColor(), ColorCard::BLUE);
  }

  {
    WagonCard card(ColorCard::RED);
    card.setColor(ColorCard::BLUE);
    BOOST_CHECK_NE(card.getColor(), ColorCard::RED);
    BOOST_CHECK_EQUAL(card.getColor(), ColorCard::BLUE);

  }
}

/* vim: set sw=2 sts=2 et : */
