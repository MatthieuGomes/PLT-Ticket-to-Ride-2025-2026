#include <boost/test/unit_test.hpp>
#include <memory>
#include <vector>

#include "../../src/shared/cardsState/PlayerCards.h"
#include "../../src/shared/cardsState/CardsState.h"
#include "../../src/shared/cardsState/DestinationCard.h"
#include "../../src/shared/cardsState/WagonCard.h"

using namespace ::cardsState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructors)

std::string test_stationNameA = "paris";
std::string test_stationNameB = "lyon";
int test_points = 5;
cardsState::ColorCard test_color = cardsState::ColorCard::BLUE;

BOOST_AUTO_TEST_CASE(Default)
{
    std::cout << "PlayerCards Default Constructor Test Started ..." << std::endl;
    std::vector<std::shared_ptr<DestinationCard>> destinationCards;
    std::vector<std::shared_ptr<WagonCard>> wagonCards;

    auto d1 = std::make_shared<DestinationCard>(test_stationNameA, test_stationNameB, test_points);
    auto w1 = std::make_shared<WagonCard>(test_color);

    destinationCards.push_back(d1);
    wagonCards.push_back(w1);

    PlayerCards player(destinationCards, wagonCards);

    BOOST_CHECK(player.destinationCards);
    BOOST_CHECK(player.wagonCards);
    BOOST_CHECK_EQUAL(player.destinationCards->countCards(), destinationCards.size());
    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), wagonCards.size());
    std::cout << "PlayerCards Default Constructor Test Finished !\n" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)
// ADD TESTS FOR _takeCard and takeCard
BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
