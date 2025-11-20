
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/cardsState/Deck.h"

using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestDeck)
{


    {
       ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
        WagonCard card1(cardArgs[0]);
        WagonCard card2(cardArgs[1]);
        auto* cardsVec = new std::vector<WagonCard>{card1,card2};
      Deck<WagonCard> deck(cardsVec);
        BOOST_CHECK_EQUAL(deck.countCards(), sizeof(cardArgs)/sizeof(ColorCard));
        for (unsigned int i=0;i<static_cast<int>(sizeof(cardArgs))/sizeof(ColorCard);i++){
            BOOST_CHECK_EQUAL(deck.cards[i]->color, cardArgs[i]);
        }


    }
}

/* vim: set sw=2 sts=2 et : */
