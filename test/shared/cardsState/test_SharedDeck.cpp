
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
BOOST_AUTO_TEST_CASE(test_trash_card)
{
    std::vector<WagonCard> trashV;
    std::vector<WagonCard> faceUpV;
    std::vector<WagonCard> faceDownV;

    faceUpV.emplace_back(ColorCard::RED);
    cardsState::SharedDeck<WagonCard> deck(&trashV, &faceUpV, &faceDownV);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 1);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 0);
    std::shared_ptr<WagonCard> cardToTrash = deck.faceUpCards->cards[0];
    deck.trashCard(cardToTrash);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 0);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 1);
    BOOST_CHECK_EQUAL(deck.trash->cards[0]->getColor(), ColorCard::RED);
    BOOST_CHECK(cardToTrash != nullptr);
    BOOST_CHECK_EQUAL(cardToTrash->getColor(), ColorCard::RED);
}

BOOST_AUTO_TEST_CASE(test_refill_main_deck)
{
    std::vector<WagonCard> trash = { WagonCard(ColorCard::RED), WagonCard(ColorCard::BLUE) };
    std::vector<WagonCard> fUp;
    std::vector<WagonCard> fDown;

    SharedDeck<WagonCard> deck(&trash, &fUp, &fDown);

    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 0);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 2);

    deck.refillMainDeck();

    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 2);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 0);
}
/* vim: set sw=2 sts=2 et : */
