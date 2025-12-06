
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#

#include "../../src/shared/cardsState/SharedDeck.h"
#include "../../src/shared/cardsState/WagonCard.h"
#include "../../src/shared/cardsState/DestinationCard.h"
#include "../../src/shared/cardsState/PlayerCards.h"

using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(test_turn_card_up)
{

    std::vector<WagonCard> trashV;
    std::vector<WagonCard> faceUpV;
    std::vector faceDownV = { WagonCard(ColorCard::RED) };

    SharedDeck<WagonCard> deck(&trashV, &faceUpV, &faceDownV);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 0);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 1);

    deck.turnCardUp();

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 1);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 0);
}
/* vim: set sw=2 sts=2 et : */
