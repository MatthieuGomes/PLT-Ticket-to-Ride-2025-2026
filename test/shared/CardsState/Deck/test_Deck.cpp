#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/cardsState/Deck.h"

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

BOOST_AUTO_TEST_SUITE(Constructors)
ColorCard test_colors[] = {ColorCard::RED, ColorCard::BLUE};


BOOST_AUTO_TEST_CASE(Default)
{
    {
        std::cout << "Deck Default Constructor<WagonCard> Test Started ..." << std::endl;
        ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
        WagonCard card1(test_colors[0]);
        WagonCard card2(test_colors[1]);
        std::vector<WagonCard> *cardsVec = new std::vector<WagonCard>{card1, card2};
        Deck<WagonCard> deck(cardsVec);
        BOOST_CHECK_EQUAL(deck.countCards(), sizeof(test_colors) / sizeof(ColorCard));
        for (unsigned int i = 0; i < static_cast<int>(sizeof(test_colors)) / sizeof(ColorCard); i++)
        {
            BOOST_CHECK_EQUAL(deck.cards[i]->color, test_colors[i]);
        }
        std::cout << "Deck Default Constructor<WagonCard> Test Finished !\n" << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)
BOOST_AUTO_TEST_CASE(getCards) {
    ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
    WagonCard card1(cardArgs[0]);
    WagonCard card2(cardArgs[1]);
    auto *cardsVec = new std::vector<WagonCard>{card1, card2};
    Deck<WagonCard> deck(cardsVec);
    auto returnedCards = deck.getCards();

    BOOST_CHECK_EQUAL(returnedCards.size(), 2);

    BOOST_CHECK_EQUAL(returnedCards[0]->color, cardArgs[0]);
    BOOST_CHECK_EQUAL(returnedCards[1]->color, cardArgs[1]);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
