
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
BOOST_AUTO_TEST_CASE(test_drawCard_from_face_up_wagon)
{
    std::vector<WagonCard> trashV;
    std::vector<WagonCard> faceUpV;
    std::vector<WagonCard> faceDownV;

    faceUpV.emplace_back(ColorCard::RED);
    faceUpV.emplace_back(ColorCard::LOCOMOTIVE);

    faceDownV.emplace_back(ColorCard::BLUE);
    faceDownV.emplace_back(ColorCard::GREEN);
    faceDownV.emplace_back(ColorCard::YELLOW);

    cardsState::SharedDeck<WagonCard> deck(&trashV, &faceUpV, &faceDownV);
    std::vector<DestinationCard> playerDest;
    std::vector<WagonCard> playerWagon;
    PlayerCards player(&playerDest, &playerWagon);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 2);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 3);
    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 0);

    auto cardToDraw = deck.faceUpCards->cards[0];

    deck.drawCard(cardToDraw, &player);

    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 1);
    BOOST_CHECK_EQUAL(player.wagonCards->cards[0]->getColor(), ColorCard::RED);
    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 2);
    BOOST_CHECK_EQUAL(deck.faceUpCards->cards.back()->getColor(), ColorCard::YELLOW);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 2);
}
    BOOST_AUTO_TEST_CASE(test_drawCard_face_down_correct_behavior)
    {
        std::vector<WagonCard> trashV;
        std::vector<WagonCard> faceUpV;
        std::vector<WagonCard> faceDownV = {
            WagonCard(ColorCard::RED),
            WagonCard(ColorCard::BLUE)
        };

        trashV.emplace_back(ColorCard::GREEN);
        trashV.emplace_back(ColorCard::YELLOW);
        trashV.emplace_back(ColorCard::BLACK);

        cardsState::SharedDeck<WagonCard> deck(&trashV, &faceUpV, &faceDownV);

        std::vector<DestinationCard> playerDest;
        std::vector<WagonCard> playerWagon;
        PlayerCards player(&playerDest, &playerWagon);

        BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 2);
        BOOST_CHECK_EQUAL(deck.trash->countCards(), 3);
        BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 0);

        deck.drawCard(10, &player);

        BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 5);
        BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 0);
        BOOST_CHECK_EQUAL(deck.trash->countCards(), 0);
    }

/* vim: set sw=2 sts=2 et : */
