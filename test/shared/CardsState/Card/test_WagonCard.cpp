
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"

using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructor)

BOOST_AUTO_TEST_CASE(Constructor)
{
  {
    WagonCard card(ColorCard::RED);
    BOOST_CHECK_EQUAL(card.color, ColorCard::RED);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetColor)
{
  {
    WagonCard card(ColorCard::RED);
    BOOST_CHECK_EQUAL(card.getColor(), ColorCard::RED);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_CASE(SetColor)
{
  {
    WagonCard card(ColorCard::RED);
    card.setColor(ColorCard::BLUE);
    BOOST_CHECK_NE(card.getColor(), ColorCard::RED);
    BOOST_CHECK_EQUAL(card.getColor(), ColorCard::BLUE);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
