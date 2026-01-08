
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"

#include "../../src/shared/cardsState/SharedDeck.h"
#include "../../src/shared/cardsState/WagonCard.h"
#include "../../src/shared/cardsState/DestinationCard.h"
#include "../../src/shared/cardsState/PlayerCards.h"
#include "../../src/shared/playersState/PlayersState.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

#define TEST(x) BOOST_AUTO_TEST_CASE(x)
#define SUITE_START(x) BOOST_AUTO_TEST_SUITE(x)
#define SUITE_END() BOOST_AUTO_TEST_SUITE_END()
#define ANN_START(x) std::cout << "Starting " << x << " test..." << std::endl;
#define ANN_END(x) std::cout << x << " test finished!" << std::endl;
#define CHECK_EQ(a, b) BOOST_CHECK_EQUAL(a, b)
#define CHECK_NE(a, b) BOOST_CHECK_NE(a, b)
#define CHECK_NTHROW(x) BOOST_CHECK_NO_THROW(x)
#define CHECK_THROW(x) BOOST_CHECK_THROW(x)
#define CHECK(x) BOOST_CHECK(x)

using namespace ::cardsState;

using playersInfos = std::tuple<std::string, playersState::PlayerColor, int, int, int, int, std::shared_ptr<cardsState::PlayerCards>>;
using playersInitInfos = std::tuple<std::string, playersState::PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

TEST(TestStaticAssert)
{
    CHECK(1);
}

SUITE_START(Constructors)

TEST(Default)
{
    ANN_START("Default Constructor")
    SharedDeck<WagonCard> decks;
    CHECK(decks.trash == nullptr);
    CHECK(decks.faceUpCards == nullptr);
    CHECK(decks.faceDownCards == nullptr);
    ANN_END("Default Constructor")
}

TEST(Basic)
{
    ANN_START("Basic Constructor")
    std::vector<std::shared_ptr<WagonCard>> trashV;
    std::vector<std::shared_ptr<WagonCard>> faceUpV;
    std::vector<std::shared_ptr<WagonCard>> faceDownV;

    trashV.emplace_back(std::make_shared<WagonCard>(ColorCard::RED));
    faceUpV.emplace_back(std::make_shared<WagonCard>(ColorCard::BLUE));
    faceDownV.emplace_back(std::make_shared<WagonCard>(ColorCard::GREEN));

    SharedDeck<WagonCard> decks(trashV, faceUpV, faceDownV);

    CHECK_EQ(decks.trash->countCards(), 1);
    CHECK_EQ(decks.faceUpCards->countCards(), 1);
    CHECK_EQ(decks.faceDownCards->countCards(), 1);
    ANN_END("Basic Constructor")
}

TEST(BasicShort)
{
    ANN_START("Basic Short Constructor")
    std::vector<std::shared_ptr<WagonCard>> faceDownV;
    faceDownV.emplace_back(std::make_shared<WagonCard>(ColorCard::GREEN));
    SharedDeck<WagonCard> decks(faceDownV);
    CHECK(decks.trash == nullptr);
    CHECK(decks.faceUpCards == nullptr);
    CHECK_EQ(decks.faceDownCards->countCards(), 1);
    ANN_END("Basic Short Constructor")
}

TEST(Init)
{
    ANN_START("Init Constructor")
    SharedDeck<WagonCard> decks = SharedDeck<WagonCard>::Init();
    CHECK(decks.trash == nullptr);
    CHECK(decks.faceUpCards == nullptr);
    CHECK_EQ(decks.faceDownCards->countCards(), WagonCard::locomotiveInitCardsCount + 8 * WagonCard::otherInitCardsCount);
    ANN_END("Init Constructor")
}

TEST(Europe)
{
    ANN_START("Europe Constructor")
    mapState::MapState europeMap = mapState::MapState::Europe();
    auto stations = europeMap.getStations();
    std::vector<std::shared_ptr<DestinationCard>> expectedCards = DestinationCard::Europe(stations);
    SharedDeck<DestinationCard> decks = SharedDeck<DestinationCard>::Europe(stations);
    CHECK(decks.trash == nullptr);
    CHECK(decks.faceUpCards == nullptr);
    CHECK_EQ(decks.faceDownCards->countCards(), expectedCards.size());
    ANN_END("Europe Constructor")
}

SUITE_END()

SUITE_START(GettersAndSetters)

SUITE_START(Getters)

SUITE_END()

SUITE_START(Setters)

SUITE_END()

SUITE_END()

SUITE_START(Operations)

SUITE_START(Internal)

TEST(turnCardUp)
{
    {
        ANN_START("turnCardUp")
        std::vector<std::shared_ptr<WagonCard>> trashV;
        std::vector<std::shared_ptr<WagonCard>> faceUpV;
        std::vector<std::shared_ptr<WagonCard>> faceDownV = {std::make_shared<WagonCard>(ColorCard::RED)};

        SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);

        CHECK_EQ(deck.faceUpCards->countCards(), 0);
        CHECK_EQ(deck.faceDownCards->countCards(), 1);

        deck.turnCardUp();

        CHECK_EQ(deck.faceUpCards->countCards(), 1);
        CHECK_EQ(deck.faceDownCards->countCards(), 0);
        ANN_END("turnCardUp")
    }
    {
        ANN_START("turnCardUp with empty faceDownCards")
        std::vector<std::shared_ptr<WagonCard>> trashV;
        std::vector<std::shared_ptr<WagonCard>> faceUpV;
        std::vector<std::shared_ptr<WagonCard>> faceDownV;

        SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);

        CHECK_EQ(deck.faceUpCards->countCards(), 0);
        CHECK_EQ(deck.faceDownCards->countCards(), 0);

        deck.turnCardUp();

        CHECK_EQ(deck.faceUpCards->countCards(), 0);
        CHECK_EQ(deck.faceDownCards->countCards(), 0);
        ANN_END("turnCardUp with empty faceDownCards")
    }
}
TEST(trashCard)
{
    ANN_START("trashCard")
    std::vector<std::shared_ptr<WagonCard>> trashV;
    std::vector<std::shared_ptr<WagonCard>> faceUpV;
    std::vector<std::shared_ptr<WagonCard>> faceDownV;

    faceUpV.emplace_back(std::make_shared<WagonCard>(ColorCard::RED));
    cardsState::SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);

    CHECK_EQ(deck.faceUpCards->countCards(), 1);
    CHECK_EQ(deck.trash->countCards(), 0);
    std::shared_ptr<WagonCard> cardToTrash = deck.faceUpCards->cards[0];
    deck.trashCard(cardToTrash);

    CHECK_EQ(deck.faceUpCards->countCards(), 0);
    CHECK_EQ(deck.trash->countCards(), 1);
    CHECK_EQ(deck.trash->cards[0]->getColor(), ColorCard::RED);
    CHECK(cardToTrash != nullptr);
    CHECK_EQ(cardToTrash->getColor(), ColorCard::RED);
    ANN_END("trashCard")
}
TEST(refillMainDeck)
{
    {
        ANN_START("refillMainDeck")
        std::vector<std::shared_ptr<WagonCard>> trash = {std::make_shared<WagonCard>(ColorCard::RED), std::make_shared<WagonCard>(ColorCard::BLUE)};
        std::vector<std::shared_ptr<WagonCard>> fUp;
        std::vector<std::shared_ptr<WagonCard>> fDown;

        SharedDeck<WagonCard> deck(trash, fUp, fDown);

        CHECK_EQ(deck.faceDownCards->countCards(), 0);
        CHECK_EQ(deck.trash->countCards(), 2);

        deck.refillMainDeck();

        CHECK_EQ(deck.faceDownCards->countCards(), 2);
        CHECK_EQ(deck.trash->countCards(), 0);
        ANN_END("refillMainDeck")
    }
    {
        ANN_START("refillMainDeck with empty trash")
        std::vector<std::shared_ptr<WagonCard>> trash;
        std::vector<std::shared_ptr<WagonCard>> fUp;
        std::vector<std::shared_ptr<WagonCard>> fDown;

        SharedDeck<WagonCard> deck(trash, fUp, fDown);

        CHECK_EQ(deck.faceDownCards->countCards(), 0);
        CHECK_EQ(deck.trash->countCards(), 0);

        deck.refillMainDeck();

        CHECK_EQ(deck.faceDownCards->countCards(), 0);
        CHECK_EQ(deck.trash->countCards(), 0);
        ANN_END("refillMainDeck with empty trash")
    }
    {
        ANN_START("refillMainDeck with non-empty faceDownCards")
        std::vector<std::shared_ptr<WagonCard>> trash = {std::make_shared<WagonCard>(ColorCard::RED)};
        std::vector<std::shared_ptr<WagonCard>> fUp;
        std::vector<std::shared_ptr<WagonCard>> fDown = {std::make_shared<WagonCard>(ColorCard::BLUE)};
        SharedDeck<WagonCard> deck(trash, fUp, fDown);
        CHECK_EQ(deck.faceDownCards->countCards(), 1);
        CHECK_EQ(deck.trash->countCards(), 1);
        deck.refillMainDeck();
        CHECK_EQ(deck.faceDownCards->countCards(), 1);
        CHECK_EQ(deck.trash->countCards(), 1);
        ANN_END("refillMainDeck with non-empty faceDownCards")
    }
}

SUITE_END() // Internal

SUITE_START(Interactions)

TEST(Setup)
{
    {
        ANN_START("Setup WagonCards")
        SharedDeck<WagonCard> deck = SharedDeck<WagonCard>::Init();
        deck.Setup();
        CHECK_EQ(deck.faceDownCards->countCards(), WagonCard::locomotiveInitCardsCount + 8 * WagonCard::otherInitCardsCount - WagonCard::faceUpCardsCount);
        CHECK_EQ(deck.faceUpCards->countCards(), WagonCard::faceUpCardsCount);
        CHECK(deck.trash == nullptr);
        ANN_END("Setup WagonCards")
    }
    {
        ANN_START("Setup DestinationCards")
        std::vector<playersInitInfos> playersInfos = {
            std::make_tuple("Player1", playersState::PlayerColor::RED, std::make_shared<PlayerCards>()),
            std::make_tuple("Player2", playersState::PlayerColor::BLUE, std::make_shared<PlayerCards>()),
        };
        playersState::PlayersState players = playersState::PlayersState::InitFromInfos(playersInfos);
        mapState::MapState europeMap = mapState::MapState::Europe();
        auto stations = europeMap.getStations();
        auto expectedCards = DestinationCard::Europe(stations);
        SharedDeck<DestinationCard> sharedDestinations = SharedDeck<DestinationCard>::Europe(stations);
        SharedDeck<WagonCard> wagonDeck = SharedDeck<WagonCard>::Init();
        std::vector<std::shared_ptr<PlayerCards>> playersCards = cardsState::PlayerCards::BatchStartHand(std::make_shared<SharedDeck<DestinationCard>>(sharedDestinations), std::make_shared<SharedDeck<WagonCard>>(wagonDeck));
        std::shared_ptr<OutOfGame<DestinationCard>> outOfGameDestinations = std::make_shared<OutOfGame<DestinationCard>>();
        sharedDestinations.Setup(outOfGameDestinations);

        CHECK_EQ(sharedDestinations.faceDownCards->countCards(), expectedCards.size() - playersState::PlayersState::nbPlayers * (cardsState::PlayerCards::startingShortDestinationCardsCount)-6);
        CHECK(sharedDestinations.faceUpCards == nullptr);
        CHECK(sharedDestinations.trash == nullptr);
        CHECK_EQ(outOfGameDestinations->countCards(), 6 - playersState::PlayersState::nbPlayers);
        ANN_END("Setup DestinationCards")
    }
}

// TODO:: add proper tests for drawCard
TEST(DrawCardFromFaceUpWagon)
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
    cardsState::SharedDeck<WagonCard> sharedDecks(trashV, faceUpV, faceDownV);
    std::vector<std::shared_ptr<DestinationCard>> playerDest;
    std::vector<std::shared_ptr<WagonCard>> playerWagon;
    std::shared_ptr<PlayerCards> playerCards = std::make_shared<PlayerCards>(playerDest, playerWagon);

    CHECK_EQ(sharedDecks.faceUpCards->countCards(), 2);
    CHECK_EQ(sharedDecks.faceDownCards->countCards(), 3);
    CHECK_EQ(playerCards->wagonCards->countCards(), 0);
    auto cardToDraw = sharedDecks.faceUpCards->cards[0];

    sharedDecks.drawCard(playerCards, cardToDraw);

    CHECK_EQ(playerCards->wagonCards->countCards(), 1);
    CHECK_EQ(playerCards->wagonCards->cards[0]->getColor(), ColorCard::RED);
    CHECK_EQ(sharedDecks.faceUpCards->countCards(), 1);
    CHECK_EQ(sharedDecks.faceUpCards->cards.back()->getColor(), ColorCard::LOCOMOTIVE);
    CHECK_EQ(sharedDecks.faceDownCards->countCards(), 3);
    std::cout << "DrawCardFromFaceUpWagon Test Finished !\n"
              << std::endl;
}
TEST(DrawCardFaceDownCorrectBehavior)
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
    std::shared_ptr<PlayerCards> player = std::make_shared<PlayerCards>(playerDest, playerWagon);
    CHECK_EQ(deck.faceDownCards->countCards(), 2);
    CHECK_EQ(deck.trash->countCards(), 3);
    CHECK_EQ(player->wagonCards->countCards(), 0);

    deck.drawCard(player, nullptr, 2);

    CHECK_EQ(player->wagonCards->countCards(), 2);
    CHECK_EQ(deck.faceDownCards->countCards(), 0);
    CHECK_EQ(deck.trash->countCards(), 3);
    std::cout << "DrawCardFaceDownCorrectBehavior Test Finished !\n"
              << std::endl;
}
// TODO : add tests for display
TEST(Display)
{
    {
        ANN_START("Display SharedDeck")
        std::vector<std::shared_ptr<WagonCard>> trashV;
        std::vector<std::shared_ptr<WagonCard>> faceUpV;
        std::vector<std::shared_ptr<WagonCard>> faceDownV;

        faceUpV.emplace_back(std::make_shared<WagonCard>(ColorCard::RED));
        faceDownV.emplace_back(std::make_shared<WagonCard>(ColorCard::BLUE));

        SharedDeck<WagonCard> deck(trashV, faceUpV, faceDownV);

        CHECK_NTHROW(deck.display());
        CHECK_NTHROW(deck.display(1));
        ANN_END("Display SharedDeck")
    }
    {
        ANN_START("Display Empty SharedDeck")
        SharedDeck<WagonCard> deck;

        CHECK_NTHROW(deck.display());
        CHECK_NTHROW(deck.display(1));
        ANN_END("Display Empty SharedDeck")
    }
}

SUITE_END() // Interactions

SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
