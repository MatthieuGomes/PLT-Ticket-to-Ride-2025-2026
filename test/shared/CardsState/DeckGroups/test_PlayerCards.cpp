#include <boost/test/unit_test.hpp>
#include <memory>
#include <vector>

#include "../../src/shared/cardsState/PlayerCards.h"
#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/cardsState/DestinationCard.h"
#include "../../src/shared/cardsState/WagonCard.h"
#include "../../src/shared/playersState/PlayersState.h"

using namespace ::cardsState;

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
#define CHECK_NTHROW(x) BOOST_CHECK_NO_THROW(x)
#define CHECK_THROW(x) BOOST_CHECK_THROW(x)
#define CHECK(x) BOOST_CHECK(x)

using playersInfos = std::tuple<std::string, playersState::PlayerColor, int, int, int, int, std::shared_ptr<cardsState::PlayerCards>>;
using playersInitInfos = std::tuple<std::string, playersState::PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

TEST(TestStaticAssert)
{
    CHECK(1);
}

SUITE_START(Constructors)
std::shared_ptr<boost::adjacency_list<>> gameGraph = std::make_shared<boost::adjacency_list<>>(boost::adjacency_list<>());
std::string test_stationNameA = "paris";
std::string test_stationNameB = "lyon";
std::shared_ptr<mapState::Station> test_stationA = std::make_shared<mapState::Station>(mapState::Station::Init(test_stationNameA, gameGraph));
std::shared_ptr<mapState::Station> test_stationB = std::make_shared<mapState::Station>(mapState::Station::Init(test_stationNameB, gameGraph));
int test_points = 5;
cardsState::ColorCard test_color = cardsState::ColorCard::BLUE;

TEST(Basic)
{
    ANN_START("Basic Constructor")
    std::vector<std::shared_ptr<DestinationCard>> destinationCards;
    std::vector<std::shared_ptr<WagonCard>> wagonCards;

    auto d1 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points, false);
    auto w1 = std::make_shared<WagonCard>(test_color);

    destinationCards.push_back(d1);
    wagonCards.push_back(w1);

    PlayerCards player(destinationCards, wagonCards);

    CHECK(player.destinationCards);
    CHECK(player.wagonCards);
    CHECK_EQ(player.destinationCards->countCards(), destinationCards.size());
    CHECK_EQ(player.wagonCards->countCards(), wagonCards.size());
    ANN_END("Basic Constructor")
}

TEST(Default)
{
    ANN_START("Default Constructor")
    PlayerCards player;
    CHECK(player.destinationCards == nullptr);
    CHECK(player.wagonCards == nullptr);
    ANN_END("Default Constructor")
}

TEST(StartHand)
{
    ANN_START("StartHand Constructor")
    std::vector<std::shared_ptr<DestinationCard>> destinationCards;
    std::vector<std::shared_ptr<WagonCard>> wagonCards;

    auto d1 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points, false);
    auto d2 = std::make_shared<DestinationCard>(test_stationB, test_stationA, test_points + 2, true);
    auto d3 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points + 4, false);
    auto d4 = std::make_shared<DestinationCard>(test_stationB, test_stationA, test_points + 6, false);
    auto w1 = std::make_shared<WagonCard>(test_color);
    auto w2 = std::make_shared<WagonCard>(cardsState::ColorCard::RED);
    auto w3 = std::make_shared<WagonCard>(cardsState::ColorCard::GREEN);
    auto w4 = std::make_shared<WagonCard>(cardsState::ColorCard::YELLOW);
    destinationCards.push_back(d1);
    destinationCards.push_back(d2);
    destinationCards.push_back(d3);
    destinationCards.push_back(d4);
    wagonCards.push_back(w1);
    wagonCards.push_back(w2);
    wagonCards.push_back(w3);
    wagonCards.push_back(w4);

    auto sharedDestinationDeck = std::make_shared<SharedDeck<DestinationCard>>(destinationCards);
    auto sharedWagonDeck = std::make_shared<SharedDeck<WagonCard>>(wagonCards);

    PlayerCards player = PlayerCards::StartHand(sharedDestinationDeck, sharedWagonDeck);

    CHECK(player.destinationCards);
    CHECK(player.wagonCards);
    CHECK_EQ(player.destinationCards->countCards(), 4);
    CHECK_EQ(player.wagonCards->countCards(), 4);

    ANN_END("StartHand Constructor")
}

TEST(BatchStartHand)
{
    ANN_START("BatchStartHand Constructor")

    std::vector<playersInitInfos> infos = {
        std::make_tuple("Player1", playersState::PlayerColor::RED, nullptr),
        std::make_tuple("Player2", playersState::PlayerColor::BLUE, nullptr)};

    playersState::PlayersState playersState = playersState::PlayersState::InitFromInfos(infos);
    std::vector<std::shared_ptr<DestinationCard>> destinationCards;
    std::vector<std::shared_ptr<WagonCard>> wagonCards;

    auto d1 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points, false);
    auto d2 = std::make_shared<DestinationCard>(test_stationB, test_stationA, test_points + 2, true);
    auto d3 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points + 4, false);
    auto d4 = std::make_shared<DestinationCard>(test_stationB, test_stationA, test_points + 6, false);
    auto d5 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points + 8, true);
    auto d6 = std::make_shared<DestinationCard>(test_stationB, test_stationA, test_points + 10, false);
    auto d7 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points + 12, false);
    auto d8 = std::make_shared<DestinationCard>(test_stationB, test_stationA, test_points + 14, false);
    auto w1 = std::make_shared<WagonCard>(test_color);
    auto w2 = std::make_shared<WagonCard>(cardsState::ColorCard::RED);
    auto w3 = std::make_shared<WagonCard>(cardsState::ColorCard::GREEN);
    auto w4 = std::make_shared<WagonCard>(cardsState::ColorCard::YELLOW);
    auto w5 = std::make_shared<WagonCard>(cardsState::ColorCard::BLACK);
    auto w6 = std::make_shared<WagonCard>(cardsState::ColorCard::ORANGE);
    auto w7 = std::make_shared<WagonCard>(cardsState::ColorCard::PINK);
    auto w8 = std::make_shared<WagonCard>(cardsState::ColorCard::WHITE);
    destinationCards.push_back(d1);
    destinationCards.push_back(d2);
    destinationCards.push_back(d3);
    destinationCards.push_back(d4);
    destinationCards.push_back(d5);
    destinationCards.push_back(d6);
    destinationCards.push_back(d7);
    destinationCards.push_back(d8);
    wagonCards.push_back(w1);
    wagonCards.push_back(w2);
    wagonCards.push_back(w3);
    wagonCards.push_back(w4);
    wagonCards.push_back(w5);
    wagonCards.push_back(w6);
    wagonCards.push_back(w7);
    wagonCards.push_back(w8);

    auto sharedDestinationDeck = std::make_shared<SharedDeck<DestinationCard>>(destinationCards);
    auto sharedWagonDeck = std::make_shared<SharedDeck<WagonCard>>(wagonCards);

    auto players = PlayerCards::BatchStartHand(sharedDestinationDeck, sharedWagonDeck);

    CHECK_EQ(players.size(), 2);
    for (const auto &player : players)
    {
        CHECK(player);
        CHECK(player->destinationCards);
        CHECK(player->wagonCards);
        CHECK_EQ(player->destinationCards->countCards(), 4);
        CHECK_EQ(player->wagonCards->countCards(), 4);
    }
    ANN_END("BatchStartHand Constructor")
}

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)
SUITE_START(Getters)
SUITE_END() // Getters
SUITE_START(Setters)
SUITE_END() // Setters
SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)
// TODO : _takeCard and takeCard

// TODO : add tests for display
TEST(Display)
{
    {
        ANN_START("Display PlayerCards")
        std::shared_ptr<boost::adjacency_list<>> gameGraph = std::make_shared<boost::adjacency_list<>>(boost::adjacency_list<>());
        std::string test_stationNameA = "paris";
        std::string test_stationNameB = "lyon";
        std::shared_ptr<mapState::Station> test_stationA = std::make_shared<mapState::Station>(mapState::Station::Init(test_stationNameA, gameGraph));
        std::shared_ptr<mapState::Station> test_stationB = std::make_shared<mapState::Station>(mapState::Station::Init(test_stationNameB, gameGraph));
        int test_points = 5;
        cardsState::ColorCard test_color = cardsState::ColorCard::BLUE;
        std::vector<std::shared_ptr<DestinationCard>> destinationCards;
        std::vector<std::shared_ptr<WagonCard>> wagonCards;

        auto d1 = std::make_shared<DestinationCard>(test_stationA, test_stationB, test_points, false);
        auto w1 = std::make_shared<WagonCard>(test_color);

        destinationCards.push_back(d1);
        wagonCards.push_back(w1);

        PlayerCards player(destinationCards, wagonCards);

        CHECK_NTHROW(player.display());
        CHECK_NTHROW(player.display(1));

        ANN_END("Display PlayerCards")
    }
    {
        ANN_START("Display PlayerCards with empty decks")
        PlayerCards player;

        CHECK_NTHROW(player.display());
        CHECK_NTHROW(player.display(1));

        ANN_END("Display PlayerCards with empty decks")
    }
}

SUITE_END() // Interactions

SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
