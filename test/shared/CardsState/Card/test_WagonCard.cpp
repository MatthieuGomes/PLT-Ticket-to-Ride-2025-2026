
#include <boost/test/unit_test.hpp>

#include "../../src/shared/cardsState/CardsState.h"

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
#define REQUIRE(...) BOOST_REQUIRE(__VA_ARGS__)
#define CHECK(x) BOOST_CHECK(x)

using namespace ::cardsState;

TEST(TestStaticAssert)
{
  CHECK(1);
}

SUITE_START(Constructors)
ColorCard test_color = ColorCard::RED;
TEST(Basic)
{
  ANN_START("Basic Constructor");
  WagonCard card(test_color);
  CHECK_EQ(card.color, test_color);
  ANN_END("Basic Constructor");
}

TEST(Default)
{
  ANN_START("Default Constructor");
  WagonCard card;
  CHECK_EQ(card.color, ColorCard::UNKNOWN);
  ANN_END("Default Constructor");
}

SUITE_START(BatchConstructors)

TEST(BatchConstructor)
{
  ANN_START("BatchConstructor");
  std::vector<ColorCard> infos = {ColorCard::RED, ColorCard::BLUE, ColorCard::GREEN};
  std::vector<std::shared_ptr<WagonCard>> cards = WagonCard::BatchConstructor(infos);
  CHECK_EQ(cards.size(), infos.size());
  for (size_t i = 0; i < infos.size(); ++i)
  {
    CHECK_EQ(cards[i]->getColor(), infos[i]);
  }
  ANN_END("BatchConstructor");
}

TEST(OrderedBatch)
{
  ANN_START("OrderedBatch");
  std::pair<ColorCard, int> infos[9] = {
      {ColorCard::RED, 2},
      {ColorCard::BLUE, 1},
      {ColorCard::GREEN, 3},
      {ColorCard::BLACK, 0},
      {ColorCard::YELLOW, 2},
      {ColorCard::ORANGE, 1},
      {ColorCard::PINK, 0},
      {ColorCard::WHITE, 2},
      {ColorCard::LOCOMOTIVE, 4}};
  std::vector<std::shared_ptr<WagonCard>> cards = WagonCard::OrderedBatch(infos);
  size_t expectedSize = 0;
  for (const auto &pair : infos)
  {
    expectedSize += pair.second;
  }
  CHECK_EQ(cards.size(), expectedSize);
  size_t index = 0;
  for (const auto &pair : infos)
  {
    for (int count = 0; count < pair.second; ++count)
    {
      CHECK_EQ(cards[index]->getColor(), pair.first);
      ++index;
    }
  }
  ANN_END("OrderedBatch");
}

TEST(StartCards)
{
  ANN_START("StartCards");
  std::vector<std::shared_ptr<WagonCard>> cards = WagonCard::StartCards();
  size_t expectedSize = 8 * WagonCard::otherInitCardsCount + WagonCard::locomotiveInitCardsCount;
  CHECK_EQ(cards.size(), expectedSize);
  std::unordered_map<ColorCard, int> colorCounts;
  for (const auto &card : cards)
  {
    colorCounts[card->getColor()]++;
  }
  for (const auto &pair : colorCounts)
  {
    std::cout << "Color: " << WagonCard::ColorsNames[pair.first] << ", Count: " << pair.second << std::endl;
    CHECK_EQ(pair.second,
             (pair.first == ColorCard::LOCOMOTIVE ? WagonCard::locomotiveInitCardsCount : WagonCard::otherInitCardsCount));
  }
  ANN_END("StartCards");
}

SUITE_END() // BatchConstructor

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)

SUITE_START(Getters)
ColorCard test_color = ColorCard::RED;

TEST(getColor)
{
  ANN_START("getColor");
  WagonCard card(test_color);
  CHECK_EQ(card.getColor(), test_color);
  ANN_END("getColor");
}

SUITE_END() // Getters

SUITE_START(Setters)

ColorCard constructor_test_color = ColorCard::RED;
ColorCard new_test_color = ColorCard::BLUE;

TEST(setColor)
{
  ANN_START("setColor");
  WagonCard card(constructor_test_color);
  card.setColor(new_test_color);
  CHECK_NE(card.getColor(), constructor_test_color);
  CHECK_EQ(card.getColor(), new_test_color);
  ANN_END("setColor");
}

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_END() // Internal

SUITE_START(Interactions)
// TODO: Add tests for display and _display
TEST(display)
{
  ANN_START("display");
  WagonCard card(ColorCard::GREEN);
  CHECK_NTHROW(card.display());
  CHECK_NTHROW(card.display(1));
  ANN_END("display");
}
TEST(_Display)
{
  ANN_START("_display");
  WagonCard card(ColorCard::YELLOW);
  CHECK_NTHROW(card._display());
  CHECK_NTHROW(card._display(1));
  ANN_END("_display");
}

SUITE_END() // Interactions

SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
