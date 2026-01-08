#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/cardsState/Deck.h"

#define DEBUG_MODE true
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
#define CHECK_NTHROW(...) BOOST_CHECK_NO_THROW(__VA_ARGS__)
#define CHECK_THROW(...) BOOST_CHECK_THROW(__VA_ARGS__)
#define CHECK(x) BOOST_CHECK(x)

using namespace ::cardsState;


using WagonCardInfos = cardsState::ColorCard;
using DestinationCardInfos = std::tuple<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>, int, bool>;

TEST(TestStaticAssert)
{
    CHECK(1);
}

SUITE_START(Constructors)
std::vector<WagonCardInfos> test_colors = {ColorCard::RED, ColorCard::BLUE};

TEST(Default)
{
    {
        ANN_START("Default Constructor<WagonCard>")
        Deck<WagonCard> deck;
        CHECK_EQ(deck.countCards(), 0);
        CHECK(deck.cards.empty());
        ANN_END("Default Constructor<WagonCard>")
    }
    {
        ANN_START("Default Constructor<DestinationCard>")
        Deck<DestinationCard> deck;
        CHECK_EQ(deck.countCards(), 0);
        CHECK(deck.cards.empty());
        ANN_END("Default Constructor<DestinationCard>")
    }
}

TEST(Basic)
{
    {
        ANN_START("Basic Constructor<WagonCard>")
        WagonCard card1(test_colors[0]);
        WagonCard card2(test_colors[1]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
        Deck<WagonCard> deck(cardsVec);
        CHECK_EQ(deck.countCards(), test_colors.size());
        for (unsigned int i = 0; i < static_cast<unsigned int>(test_colors.size()); i++)
        {
            CHECK_EQ(deck.cards[i]->color, test_colors[i]);
        }
        ANN_END("Basic Constructor<WagonCard>")
    }
    {
        ANN_START("Basic Constructor<DestinationCard>")
        DestinationCard cardA(nullptr, nullptr, 5, false);
        DestinationCard cardB(nullptr, nullptr, 10, true);
        std::vector<std::shared_ptr<DestinationCard>> cardsVec = std::vector<std::shared_ptr<DestinationCard>>{std::make_shared<DestinationCard>(cardA), std::make_shared<DestinationCard>(cardB)};
        Deck<DestinationCard> deck(cardsVec);
        CHECK_EQ(deck.countCards(), 2);
        CHECK_EQ(deck.cards[0]->points, 5);
        CHECK_EQ(deck.cards[1]->points, 10);
        ANN_END("Basic Constructor<DestinationCard>")
    }
    {
        ANN_START("Basic Constructor<WagonCard> empty vector")
        std::vector<std::shared_ptr<WagonCard>> emptyVec;
        Deck<WagonCard> deck(emptyVec);
        CHECK_EQ(deck.countCards(), 0);
        CHECK(deck.cards.empty());
        ANN_END("Basic Constructor<WagonCard> empty vector")
    }
}

TEST(BasicNoPointers)
{
    {
        ANN_START("Basic Constructor<WagonCard> no pointers")
        WagonCard card1(test_colors[0]);
        WagonCard card2(test_colors[1]);
        std::vector<WagonCard> cardsVec = std::vector<WagonCard>{card1, card2};
        Deck<WagonCard> deck(cardsVec);
        CHECK_EQ(deck.countCards(), test_colors.size());
        for (unsigned int i = 0; i < static_cast<unsigned int>(test_colors.size()); i++)
        {
            CHECK_EQ(deck.cards[i]->color, test_colors[i]);
        }
        ANN_END("Basic Constructor<WagonCard> no pointers")
    }
    {
        ANN_START("Basic Constructor<DestinationCard> no pointers")
        DestinationCard cardA(nullptr, nullptr, 5, false);
        DestinationCard cardB(nullptr, nullptr, 10, true);
        std::vector<DestinationCard> cardsVec = std::vector<DestinationCard>{cardA, cardB};
        Deck<DestinationCard> deck(cardsVec);
        CHECK_EQ(deck.countCards(), 2);
        CHECK_EQ(deck.cards[0]->points, 5);
        CHECK_EQ(deck.cards[1]->points, 10);
        ANN_END("Basic Constructor<DestinationCard> no pointers")
    }
    {
        ANN_START("Basic Constructor<WagonCard> empty vector no pointers")
        std::vector<WagonCard> emptyVec;
        Deck<WagonCard> deck(emptyVec);
        CHECK_EQ(deck.countCards(), 0);
        CHECK(deck.cards.empty());
        ANN_END("Basic Constructor<WagonCard> empty vector no pointers")
    }
}

TEST(BatchDeck)
{
    {
        ANN_START("Batch Constructor<WagonCard>")
        std::vector<WagonCardInfos> cardArgs = {ColorCard::RED, ColorCard::BLUE, ColorCard::GREEN};
        Deck<WagonCard> deck(cardArgs);

        CHECK_EQ(deck.countCards(), 3);

        for (size_t i = 0; i < 3; i++)
        {
            BOOST_REQUIRE(deck.cards[i]);
            CHECK_EQ(deck.cards[i]->color, cardArgs[i]);
        }
        ANN_END("Batch Constructor<WagonCard>")
    }
    {
        ANN_START("Batch Constructor<DestinationCard>")
        using namespace mapState;

        // ---- Graph ----
        auto graph = std::make_shared<boost::adjacency_list<>>();

        auto v1 = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
            boost::add_vertex(*graph));
        auto v2 = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
            boost::add_vertex(*graph));

        // ---- Owner ----
        auto owner = std::make_shared<playersState::Player>("TestPlayer", playersState::PlayerColor::RED, 0, 45, 3, 5, nullptr);

        // ---- Stations ----
        auto stationA = std::make_shared<Station>("StationA", owner, v1);
        auto stationB = std::make_shared<Station>("StationB", owner, v2);

        // ---- Destination cards infos ----
        std::vector<DestinationCardInfos> cardArgs = {
            {stationA, stationB, 5, false},
            {stationB, stationA, 7, true}};

        Deck<DestinationCard> deck(cardArgs);

        CHECK_EQ(deck.countCards(), 2);

        BOOST_REQUIRE(deck.cards[0]);
        BOOST_REQUIRE(deck.cards[1]);

        CHECK_EQ(deck.cards[0]->getstationA()->getName(), "StationA");
        CHECK_EQ(deck.cards[0]->getstationB()->getName(), "StationB");
        CHECK_EQ(deck.cards[0]->getPoints(), 5);

        CHECK_EQ(deck.cards[1]->getstationA()->getName(), "StationB");
        CHECK_EQ(deck.cards[1]->getstationB()->getName(), "StationA");
        CHECK_EQ(deck.cards[1]->getPoints(), 7);
        ANN_END("Batch Constructor<DestinationCard>")
    }
}

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)

SUITE_START(Getters)
TEST(getCards)
{
    ANN_START("getCards")
    ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
    WagonCard card1(cardArgs[0]);
    WagonCard card2(cardArgs[1]);
    std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
    Deck<WagonCard> deck(cardsVec);
    auto returnedCards = deck.getCards();

    CHECK_EQ(returnedCards.size(), 2);

    CHECK_EQ(returnedCards[0]->color, cardArgs[0]);
    CHECK_EQ(returnedCards[1]->color, cardArgs[1]);
    ANN_END("getCards")
}
TEST(countCards)
{
    ANN_START("countCards")
    ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE, ColorCard::GREEN};
    WagonCard card1(cardArgs[0]);
    WagonCard card2(cardArgs[1]);
    WagonCard card3(cardArgs[2]);
    std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2), std::make_shared<WagonCard>(card3)};
    Deck<WagonCard> deck(cardsVec);
    CHECK_EQ(deck.countCards(), 3);
    ANN_END("countCards")
}

SUITE_END() // Getters

SUITE_START(Setters)

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

TEST(shuffle)
{
    {
        ANN_START("shuffle")
        ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE, ColorCard::GREEN, ColorCard::YELLOW, ColorCard::BLACK};
        std::vector<std::shared_ptr<WagonCard>> cardsVec;
        for (ColorCard color : cardArgs)
        {
            WagonCard card(color);
            cardsVec.push_back(std::make_shared<WagonCard>(card));
        }
        Deck<WagonCard> deck(cardsVec);

        std::vector<ColorCard> originalOrder;
        for (const auto &card : deck.getCards())
        {
            originalOrder.push_back(card->getColor());
        }

        deck.shuffle();

        std::vector<ColorCard> shuffledOrder;
        for (const auto &card : deck.getCards())
        {
            shuffledOrder.push_back(card->getColor());
        }

        bool isDifferent = false;
        for (size_t i = 0; i < originalOrder.size(); ++i)
        {
            if (originalOrder[i] != shuffledOrder[i])
            {
                isDifferent = true;
                break;
            }
        }

        CHECK(isDifferent);
        ANN_END("shuffle")
    }
    {
        ANN_START("shuffle static")
        ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE, ColorCard::GREEN, ColorCard::YELLOW, ColorCard::BLACK};
        std::vector<std::shared_ptr<WagonCard>> cardsVec;
        for (ColorCard color : cardArgs)
        {
            WagonCard card(color);
            cardsVec.push_back(std::make_shared<WagonCard>(card));
        }

        std::vector<ColorCard> originalOrder;
        for (const auto &card : cardsVec)
        {
            originalOrder.push_back(card->getColor());
        }

        std::vector<std::shared_ptr<WagonCard>> shuffledCardsVec = Deck<WagonCard>::shuffle(cardsVec);

        std::vector<ColorCard> shuffledOrder;
        for (const auto &card : shuffledCardsVec)
        {
            shuffledOrder.push_back(card->getColor());
        }

        bool isDifferent = false;
        for (size_t i = 0; i < originalOrder.size(); ++i)
        {
            if (originalOrder[i] != shuffledOrder[i])
            {
                isDifferent = true;
                break;
            }
        }

        CHECK(isDifferent);
        ANN_END("shuffle static")
    }
}

TEST(addCard)
{
    ANN_START("addCard")
    Deck<WagonCard> deck;
    WagonCard card(ColorCard::RED);
    auto cardPtr = std::make_shared<WagonCard>(card);
    deck.addCard(cardPtr);
    CHECK_EQ(deck.countCards(), 1);
    CHECK_EQ(deck.cards[0]->color, ColorCard::RED);
    ANN_END("addCard")
}

SUITE_END() // Internal

SUITE_START(Interactions)

TEST(moveCardTo)
{
    {
        ANN_START("moveCardTo")
        ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
        WagonCard card1(cardArgs[0]);
        WagonCard card2(cardArgs[1]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
        auto originDeck = std::make_shared<Deck<WagonCard>>(cardsVec);
        auto destinationDeck = std::make_shared<Deck<WagonCard>>();

        Deck<WagonCard>::moveCardTo(originDeck, destinationDeck, 0);

        CHECK_EQ(originDeck->countCards(), 1);
        CHECK_EQ(destinationDeck->countCards(), 1);

        CHECK_EQ(originDeck->cards[0]->color, ColorCard::BLUE);
        CHECK_EQ(destinationDeck->cards[0]->color, ColorCard::RED);
        ANN_END("moveCardTo")
    }
    {
        ANN_START("moveCardTo nonexistent origin deck")
        ColorCard cardArgs[] = {ColorCard::RED};
        WagonCard card1(cardArgs[0]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1)};
        auto originDeck = std::make_shared<Deck<WagonCard>>(cardsVec);
        auto destinationDeck = std::make_shared<Deck<WagonCard>>();

        CHECK_NTHROW(Deck<WagonCard>::moveCardTo(nullptr, destinationDeck, 1)); 
        CHECK_NTHROW(Deck<WagonCard>::moveCardTo(originDeck, nullptr, 1));      
        CHECK_NTHROW(Deck<WagonCard>::moveCardTo(nullptr, nullptr, 1));         

        ANN_END("moveCardTo invalid position")
    }
}

TEST(removeCard)
{
    {
        ANN_START("removeCard")
        ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
        WagonCard card1(cardArgs[0]);
        WagonCard card2(cardArgs[1]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
        Deck<WagonCard> deck(cardsVec);

        auto removedCard = deck.removeCard(0);

        CHECK_EQ(deck.countCards(), 1);
        CHECK_EQ(removedCard->color, ColorCard::RED);
        CHECK_EQ(deck.cards[0]->color, ColorCard::BLUE);
        ANN_END("removeCard")
    }
    {
        ANN_START("removeCard invalid position")
        ColorCard cardArgs[] = {ColorCard::RED};
        WagonCard card1(cardArgs[0]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1)};
        Deck<WagonCard> deck(cardsVec);
        CHECK_EQ(deck.removeCard(-1), nullptr); 
        CHECK_EQ(deck.removeCard(1), nullptr);  
        ANN_END("removeCard invalid position")
    }
}

TEST(putCardBack)
{
    {
        ANN_START("putCardBack")
        ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
        WagonCard card1(cardArgs[0]);
        WagonCard card2(cardArgs[1]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1)};
        Deck<WagonCard> deck(cardsVec);

        deck.putCardBack(card2);

        CHECK_EQ(deck.countCards(), 2);
        CHECK_EQ(deck.cards[0]->color, ColorCard::BLUE);
        CHECK_EQ(deck.cards[1]->color, ColorCard::RED);
        ANN_END("putCardBack")
    }
    {
        ANN_START("putCardBack on empty deck")
        Deck<WagonCard> deck;

        WagonCard card(ColorCard::RED);
        deck.putCardBack(card);

        CHECK_EQ(deck.countCards(), 1);
        CHECK_EQ(deck.cards[0]->color, ColorCard::RED);
        ANN_END("putCardBack on empty deck")
    }
    {
        ANN_START("putCardBack on deck with one nullptr card")
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{nullptr};
        Deck<WagonCard> deck(cardsVec);

        WagonCard card(ColorCard::RED);
        deck.putCardBack(card);

        CHECK_EQ(deck.countCards(), 1);
        CHECK_EQ(deck.cards[0]->color, ColorCard::RED);
        ANN_END("putCardBack on deck with one nullptr card")
    }
}

TEST(takeLastCard)
{
    {
        ANN_START("takeLastCard")
        ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
        WagonCard card1(cardArgs[0]);
        WagonCard card2(cardArgs[1]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
        Deck<WagonCard> deck(cardsVec);

        auto lastCard = deck.takeLastCard();

        CHECK_EQ(deck.countCards(), 1);
        CHECK_EQ(lastCard->color, ColorCard::BLUE);
        CHECK_EQ(deck.cards[0]->color, ColorCard::RED);
        ANN_END("takeLastCard")
    }
    {
        ANN_START("takeLastCard on empty deck")
        Deck<WagonCard> deck;

        auto lastCard = deck.takeLastCard();

        CHECK_EQ(deck.countCards(), 0);
        CHECK_EQ(lastCard, nullptr);
        ANN_END("takeLastCard on empty deck")
    }
}
// TODO : add tests for display and _display
TEST(display)
{
    ANN_START("display")
    ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
    WagonCard card1(cardArgs[0]);
    WagonCard card2(cardArgs[1]);
    std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
    Deck<WagonCard> deck(cardsVec);
    CHECK_NTHROW(deck.display());
    CHECK_NTHROW(deck.display(1));
    ANN_END("display")
}
TEST(_display)
{
    ANN_START("_display")
    ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
    WagonCard card1(cardArgs[0]);
    WagonCard card2(cardArgs[1]);
    std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
    Deck<WagonCard> deck(cardsVec);
    CHECK_NTHROW(deck._display());
    CHECK_NTHROW(deck._display(1));
    ANN_END("_display")
}

SUITE_END() // Interactions

SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */