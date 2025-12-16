
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"

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
ColorCard test_color = ColorCard::RED;
BOOST_AUTO_TEST_CASE(Constructor)
{
  {
    std::cout << "WagonCard Constructor Test Started ..." << std::endl;
    WagonCard card(test_color);
    BOOST_CHECK_EQUAL(card.color, test_color);
    std::cout << "WagonCard Constructor Test Finished !\n" << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)



BOOST_AUTO_TEST_SUITE(Getters)
ColorCard test_color = ColorCard::RED;

BOOST_AUTO_TEST_CASE(GetColor)
{
  {
    std::cout << "Get Color Test Started ..." << std::endl;
    WagonCard card(test_color);
    BOOST_CHECK_EQUAL(card.getColor(), test_color);
    std::cout << "Get Color Test Finished !\n" << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

ColorCard constructor_test_color = ColorCard::RED;
ColorCard new_test_color = ColorCard::BLUE;

BOOST_AUTO_TEST_CASE(SetColor)
{
  {
    std::cout << "Set Color Test Started ..." << std::endl;
    WagonCard card(constructor_test_color);
    card.setColor(new_test_color);
    BOOST_CHECK_NE(card.getColor(), constructor_test_color);
    BOOST_CHECK_EQUAL(card.getColor(), new_test_color);
    std::cout << "Set Color Test Finished !\n" << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
