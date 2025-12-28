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

using namespace ::cardsState;

// MISSING TESTS FOR TRASH, FACEUP, FACEDOWN DECKS !!!


using WagonCardInfos = cardsState::ColorCard;
using DestinationCardInfos =std::tuple<std::shared_ptr<mapState::Station>,std::shared_ptr<mapState::Station>, int>;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

// ADD STRUCTURE (std::cout) TO ALL TESTS
BOOST_AUTO_TEST_SUITE(Constructors)
std::vector<WagonCardInfos> test_colors = {ColorCard::RED, ColorCard::BLUE};
// ADD TESTS FOR OTHER CONSTRUCTORS
BOOST_AUTO_TEST_CASE(Default)
{
    {
        std::cout << "Deck Default Constructor<WagonCard> Test Started ..." << std::endl;
        WagonCard card1(test_colors[0]);
        WagonCard card2(test_colors[1]);
        std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
        Deck<WagonCard> deck(cardsVec);
                                        BOOST_CHECK_EQUAL(deck.countCards(),test_colors.size());
                for (unsigned int i = 0; i < static_cast<unsigned int>(test_colors.size()); i++)
        {
            BOOST_CHECK_EQUAL(deck.cards[i]->color, test_colors[i]);
        }
        std::cout << "Deck Default Constructor<WagonCard> Test Finished !\n"
                  << std::endl;
    }
    // ADD TEST FOR DESTINATION CARD DECK
}

BOOST_AUTO_TEST_CASE(BatchWagonDeck)
{
    std::vector<WagonCardInfos> cardArgs = {ColorCard::RED, ColorCard::BLUE, ColorCard::GREEN};
    Deck<WagonCard> deck(
        cardArgs);

    BOOST_CHECK_EQUAL(deck.countCards(), 3);

    for (size_t i = 0; i < 3; i++)
    {
        BOOST_REQUIRE(deck.cards[i]);
        BOOST_CHECK_EQUAL(deck.cards[i]->color, cardArgs[i]);
    }
}
BOOST_AUTO_TEST_CASE(BatchDestinationDeck)
{
    using namespace mapState;

    // ---- Graph ----
    auto graph = std::make_shared<boost::adjacency_list<>>();

    auto v1 = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));
    auto v2 = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));
    auto v3 = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));
    auto v4 = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
        boost::add_vertex(*graph));

    // ---- Owner ----
    auto owner = std::make_shared<playersState::Player>(
        1, "TestPlayer", ColorCard::RED, 0, 45, 3, 5, nullptr);

    bool blocked = false;

    // ---- Stations ----
    auto paris   = std::make_shared<Station>("Paris", owner, blocked, v1);
    auto lyon    = std::make_shared<Station>("Lyon", owner, blocked, v2);
    auto berlin  = std::make_shared<Station>("Berlin", owner, blocked, v3);
    auto munich  = std::make_shared<Station>("Munich", owner, blocked, v4);

    // ---- Destination cards infos ----
    std::vector<DestinationCardInfos> cardArgs = {
        {paris, lyon, 5},
        {berlin, munich, 7}
    };

    Deck<DestinationCard> deck(cardArgs);

    BOOST_CHECK_EQUAL(deck.countCards(), 2);

    BOOST_REQUIRE(deck.cards[0]);
    BOOST_REQUIRE(deck.cards[1]);

    BOOST_CHECK_EQUAL(deck.cards[0]->getstationA()->getName(), "Paris");
    BOOST_CHECK_EQUAL(deck.cards[0]->getstationB()->getName(), "Lyon");
    BOOST_CHECK_EQUAL(deck.cards[0]->getPoints(), 5);

    BOOST_CHECK_EQUAL(deck.cards[1]->getstationA()->getName(), "Berlin");
    BOOST_CHECK_EQUAL(deck.cards[1]->getstationB()->getName(), "Munich");
    BOOST_CHECK_EQUAL(deck.cards[1]->getPoints(), 7);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)
BOOST_AUTO_TEST_CASE(getCards)
{
    ColorCard cardArgs[] = {ColorCard::RED, ColorCard::BLUE};
    WagonCard card1(cardArgs[0]);
    WagonCard card2(cardArgs[1]);
    std::vector<std::shared_ptr<WagonCard>> cardsVec = std::vector<std::shared_ptr<WagonCard>>{std::make_shared<WagonCard>(card1), std::make_shared<WagonCard>(card2)};
    Deck<WagonCard> deck(cardsVec);
    auto returnedCards = deck.getCards();

    BOOST_CHECK_EQUAL(returnedCards.size(), 2);

    BOOST_CHECK_EQUAL(returnedCards[0]->color, cardArgs[0]);
    BOOST_CHECK_EQUAL(returnedCards[1]->color, cardArgs[1]);
}

// ADD TESTS FOR GetCardByTypeName, countCards
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

// ADD TESTS FOR addCard, removeCard, putCardBack, takeLastCard

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

// ADD TESTS FOR moveCardTo

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */