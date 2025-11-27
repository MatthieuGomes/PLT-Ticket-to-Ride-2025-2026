
#include <boost/test/unit_test.hpp>

#include "../../src/shared/playersState/Player.h"

using namespace ::playersState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestPLayerConstructor) {

    cardsState::DestinationCard dest("Paris", "Marseille", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);

    std::vector<cardsState::DestinationCard> destCards = {dest};
    std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

    Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
    BOOST_CHECK_EQUAL(player.id,1);
    BOOST_CHECK_EQUAL(player.name,"yosra");
    BOOST_CHECK_EQUAL(player.color,cardsState::ColorCard::RED);
    BOOST_CHECK_EQUAL(player.score,23);
    BOOST_CHECK_EQUAL(player.numTrains,21);
    BOOST_CHECK_EQUAL(player.nbStations,10);
    BOOST_CHECK_EQUAL(player.nbRoads,5);
    BOOST_CHECK_EQUAL(player.hand,hand);

}

BOOST_AUTO_TEST_CASE(TestPLayergetId)
{
    {
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getId(),1);
    }
}

BOOST_AUTO_TEST_CASE(TestPLayergetName)
{
    {
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getName(), "yosra");


    }
}


BOOST_AUTO_TEST_CASE(TestPlayergetColor){

    {
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getColor(),cardsState::ColorCard::RED);
    }

}

BOOST_AUTO_TEST_CASE(TestPlayergetScore){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getScore(),23);
    }

}

BOOST_AUTO_TEST_CASE(TestPlayergetNumTrains){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNumTrains(),21);
    }

}

BOOST_AUTO_TEST_CASE(TestPlayergetNbStations){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbStations(),10);
    }

}

BOOST_AUTO_TEST_CASE(TestPlayergetNbRoads){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbRoads(),5);
    }

}


BOOST_AUTO_TEST_CASE(TestPlayergetHand){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getHand(),hand);
    }

}

BOOST_AUTO_TEST_CASE(TestPLayersetId)
{
    {
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setId(2);
        BOOST_CHECK_EQUAL(player.id,2);


    }
}

BOOST_AUTO_TEST_CASE(TestPLayersetName)
{
    {
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setName("sara");
        BOOST_CHECK_EQUAL(player.name,"sara");


    }
}


BOOST_AUTO_TEST_CASE(TestPlayersetColor){

    {
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setColor(cardsState::ColorCard::GREEN);
        BOOST_CHECK_EQUAL(player.color,cardsState::ColorCard::GREEN);
    }

}

BOOST_AUTO_TEST_CASE(TestPlayersetScore){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player = Player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        printf("player score :", player.score);
        player.setScore(40);
        BOOST_CHECK_EQUAL(player.score,40);
    }

}

BOOST_AUTO_TEST_CASE(TestPlayersetNumTrains){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setNumTrains(20);
        BOOST_CHECK_EQUAL(player.numTrains,20);
    }

}

BOOST_AUTO_TEST_CASE(TestPlayersetNbStations){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setNbStations(9);
        BOOST_CHECK_EQUAL(player.nbStations,9);

    }

}


BOOST_AUTO_TEST_CASE(TestPlayersetNbRoads){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setNbRoads(6);
        BOOST_CHECK_EQUAL(player.nbRoads,6);

    }

}


BOOST_AUTO_TEST_CASE(TestPlayersetHand){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        cardsState::DestinationCard dest1("Lyon", "Toulouse", 15);
        cardsState::WagonCard wagon1(cardsState::ColorCard::BLUE);
        std::vector<cardsState::DestinationCard> destCards1 = {dest1};
        std::vector<cardsState::WagonCard> wagonCardsVec1 = {wagon1};
        auto* hand1 = new cardsState::PlayerCards(&destCards1, &wagonCardsVec1);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setHand(hand1);
        BOOST_CHECK_EQUAL(player.hand,hand1);
    }

}













/* vim: set sw=2 sts=2 et : */
