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

BOOST_AUTO_TEST_CASE(Constructor_WithSharedPtrVectors)
{
    std::vector<std::shared_ptr<DestinationCard>> destinationCards;
    std::vector<std::shared_ptr<WagonCard>> wagonCards;


    destinationCards.push_back(d1);
    wagonCards.push_back(w1);

    PlayerCards player(destinationCards, wagonCards);

    BOOST_CHECK(player.destinationCards);
    BOOST_CHECK(player.wagonCards);
    BOOST_CHECK_EQUAL(player.destinationCards->countCards(), 1);
    BOOST_CHECK_EQUAL(player.wagonCards->countCards(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Operations)


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)


BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
