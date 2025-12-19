
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"

#include "../../src/shared/cardsState/SharedDeck.h"
#include "../../src/shared/cardsState/WagonCard.h"
#include "../../src/shared/cardsState/DestinationCard.h"
#include "../../src/shared/cardsState/PlayerCards.h"

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

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_CASE(TurnCardUp)
{
    std::cout << "TurnCardUp Test Started ..." << std::endl;
    std::vector<std::shared_ptr<WagonCard>> trashV;
    std::vector<std::shared_ptr<WagonCard>> faceUpV;
    std::vector<std::shared_ptr<WagonCard>> faceDownV = {std::make_shared<WagonCard>(ColorCard::RED)};

    SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 0);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 1);

    deck.turnCardUp();

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 1);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 0);
    std::cout << "TurnCardUp Test Finished !\n" << std::endl;
}
BOOST_AUTO_TEST_CASE(TrashCard)
{
    std::cout << "TrashCard Test Started ..." << std::endl;
    std::vector<std::shared_ptr<WagonCard>> trashV;
    std::vector<std::shared_ptr<WagonCard>> faceUpV;
    std::vector<std::shared_ptr<WagonCard>> faceDownV;

    faceUpV.emplace_back(std::make_shared<WagonCard>(ColorCard::RED));
    cardsState::SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 1);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 0);
    std::shared_ptr<WagonCard> cardToTrash = deck.faceUpCards->cards[0];
    deck.trashCard(cardToTrash);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 0);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 1);
    BOOST_CHECK_EQUAL(deck.trash->cards[0]->getColor(), ColorCard::RED);
    BOOST_CHECK(cardToTrash != nullptr);
    BOOST_CHECK_EQUAL(cardToTrash->getColor(), ColorCard::RED);
    std::cout << "TrashCard Test Finished !\n" << std::endl;
}
BOOST_AUTO_TEST_CASE(RefillMainDeck)
{
    std::cout << "RefillMainDeck Test Started ..." << std::endl;
    std::vector<std::shared_ptr<WagonCard>> trash = {std::make_shared<WagonCard>(ColorCard::RED), std::make_shared<WagonCard>(ColorCard::BLUE)};
    std::vector<std::shared_ptr<WagonCard>> fUp;
    std::vector<std::shared_ptr<WagonCard>> fDown;

    SharedDeck<WagonCard> deck(trash, fUp, fDown);

    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 0);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 2);

    deck.refillMainDeck();

    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 2);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 0);
    std::cout << "RefillMainDeck Test Finished !\n" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_CASE(DrawCardFromFaceUpWagon)
{
    
    std::cout << "DrawCardFromFaceUpWagon Test Started ..." << std::endl;
    std::vector<std::shared_ptr<WagonCard>> trashV;
    std::vector<std::shared_ptr<WagonCard>> faceUpV;
    std::vector<std::shared_ptr<WagonCard>> faceDownV;

    faceUpV.emplace_back(std::make_shared<WagonCard>(ColorCard::RED));
    faceUpV.emplace_back(std::make_shared<WagonCard>(ColorCard::LOCOMOTIVE));

    faceDownV.emplace_back(std::make_shared<WagonCard>(ColorCard::BLUE));
    faceDownV.emplace_back(std::make_shared<WagonCard>(ColorCard::GREEN));
    faceDownV.emplace_back(std::make_shared<WagonCard>(ColorCard::YELLOW));
    cardsState::SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);
    std::vector<std::shared_ptr<DestinationCard>> playerDest;
    std::vector<std::shared_ptr<WagonCard>> playerWagon;
    PlayerCards player(playerDest, playerWagon);

    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 2);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 3);
    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 0);

    auto cardToDraw = deck.faceUpCards->cards[0];

    // deck.drawCard(cardToDraw, &player);

    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 1);
    BOOST_CHECK_EQUAL(player.wagonCards->cards[0]->getColor(), ColorCard::RED);
    BOOST_CHECK_EQUAL(deck.faceUpCards->countCards(), 2);
    BOOST_CHECK_EQUAL(deck.faceUpCards->cards.back()->getColor(), ColorCard::YELLOW);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 2);
    std::cout << "DrawCardFromFaceUpWagon Test Finished !\n" << std::endl;
}
BOOST_AUTO_TEST_CASE(DrawCardFaceDownCorrectBehavior)
{
    std::cout << "DrawCardFaceDownCorrectBehavior Test Started ..." << std::endl;
    std::vector<std::shared_ptr<WagonCard>> trashV;
    std::vector<std::shared_ptr<WagonCard>> faceUpV;
    std::vector<std::shared_ptr<WagonCard>> faceDownV = {
        std::make_shared<WagonCard>(ColorCard::RED),
        std::make_shared<WagonCard>(ColorCard::BLUE)};

    trashV.emplace_back(std::make_shared<WagonCard>(ColorCard::GREEN));
    trashV.emplace_back(std::make_shared<WagonCard>(ColorCard::YELLOW));
    trashV.emplace_back(std::make_shared<WagonCard>(ColorCard::BLACK));

    cardsState::SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);

    std::vector<std::shared_ptr<DestinationCard>> playerDest;
    std::vector<std::shared_ptr<WagonCard>> playerWagon;
    PlayerCards player(playerDest, playerWagon);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 2);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 3);
    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 0);

    // deck.drawCard(10, &player);

    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 5);
    BOOST_CHECK_EQUAL(deck.faceDownCards->countCards(), 0);
    BOOST_CHECK_EQUAL(deck.trash->countCards(), 0);
    std::cout << "DrawCardFaceDownCorrectBehavior Test Finished !\n" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()


/* vim: set sw=2 sts=2 et : */
