
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/playersState.h"

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
#define CHECK_NTHROW(...) BOOST_CHECK_NO_THROW(__VA_ARGS__)
#define CHECK_THROW(...) BOOST_CHECK_THROW(__VA_ARGS__)
#define CHECK(x) BOOST_CHECK(x)

using namespace ::cardsState;

using playersInitInfos = std::tuple<std::string, playersState::PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

TEST(TestStaticAssert)
{
  CHECK(1);
}

SUITE_START(Constructors)
TEST(Default)
{
  ANN_START("Default Constructor")
  CardsState state;

  CHECK(state.gameDestinationCards == nullptr);
  CHECK(state.gameWagonCards == nullptr);
  CHECK(state.playersCards.empty());
  ANN_END("Default Constructor")
}

TEST(Europe)
{
  ANN_START("Europe Constructor")
  std::vector<std::shared_ptr<playersState::Player>> players;
  std::vector<playersInitInfos> playersInfos = {
      std::make_tuple("Alice", playersState::PlayerColor::RED, nullptr),
      std::make_tuple("Bob", playersState::PlayerColor::BLUE, nullptr),
      std::make_tuple("Charlie", playersState::PlayerColor::GREEN, nullptr)};
  playersState::PlayersState playersState = playersState::PlayersState::InitFromInfos(playersInfos);

  mapState::MapState mapState = mapState::MapState::Europe();
  mapState.display();
  const std::vector<std::shared_ptr<mapState::Station>> stations = mapState.getStations();
  CardsState newState = CardsState::Europe(stations);

  CHECK(newState.gameDestinationCards);
  CHECK(!newState.gameDestinationCards->faceDownCards->cards.empty());
  CHECK_EQ(newState.gameDestinationCards->faceDownCards->getCards().size(), 46 - (3 * 3 + 6));
  CHECK(!newState.gameDestinationCards->faceUpCards);

  CHECK(!newState.gameDestinationCards->trash);
  CHECK(newState.gameWagonCards);
  CHECK_EQ(newState.gameWagonCards->faceDownCards->getCards().size(), 8 * WagonCard::otherInitCardsCount + WagonCard::locomotiveInitCardsCount - (3 * 4 + 5));
  CHECK_EQ(newState.gameWagonCards->faceUpCards->getCards().size(), 5);
  CHECK(!newState.gameWagonCards->trash);
  CHECK(newState.outOfGameCards != nullptr);
  CHECK_EQ(newState.outOfGameCards->cards.size(), 6 - 3);
  CHECK(!newState.playersCards.empty());
  for (const auto &playerCards : newState.playersCards)
  {
    CHECK_EQ(playerCards->destinationCards->countCards(), 4);
    CHECK_EQ(playerCards->wagonCards->countCards(), 4);
  }
  ANN_END("Europe Constructor")
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
// TODO: Add tests for display
TEST(display)
{
  {
    ANN_START("display empty CardsState")
    CardsState state = CardsState();

    CHECK_NTHROW(state.display());
    CHECK_NTHROW(state.display(1));
    ANN_END("display empty CardsState")
  }
  {
    ANN_START("display Europe CardsState")
    std::vector<std::shared_ptr<playersState::Player>> players;
    std::vector<playersInitInfos> playersInfos = {
        std::make_tuple("Alice", playersState::PlayerColor::RED, nullptr),
        std::make_tuple("Bob", playersState::PlayerColor::BLUE, nullptr),
        std::make_tuple("Charlie", playersState::PlayerColor::GREEN, nullptr)};
    playersState::PlayersState playersState = playersState::PlayersState::InitFromInfos(playersInfos);

    mapState::MapState mapState = mapState::MapState::Europe();
    const std::vector<std::shared_ptr<mapState::Station>> stations = mapState.getStations();
    CardsState newState = CardsState::Europe(stations);

    CHECK_NTHROW(newState.display());
    CHECK_NTHROW(newState.display(1));
    ANN_END("display Europe CardsState")
  }
}

SUITE_END() // Interactions

SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
