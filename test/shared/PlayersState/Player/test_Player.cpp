
#include <boost/test/unit_test.hpp>

#include "../../src/shared/playersState/Player.h"

using namespace ::playersState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}


BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Constructor) {

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
    BOOST_CHECK_EQUAL(player.nbWagons,21);
    BOOST_CHECK_EQUAL(player.nbStations,10);
    BOOST_CHECK_EQUAL(player.nbRoads,5);
    BOOST_CHECK_EQUAL(player.hand,hand);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(getId)
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

BOOST_AUTO_TEST_CASE(getName)
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


BOOST_AUTO_TEST_CASE(getColor){

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

BOOST_AUTO_TEST_CASE(getScore){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getScore(),23);
    }

}
BOOST_AUTO_TEST_CASE(getNumTrains){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbWagons(),21);
    }

}

BOOST_AUTO_TEST_CASE(getNbStations){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbStations(),10);
    }

}

BOOST_AUTO_TEST_CASE(getNbRoads){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbRoads(),5);
    }

}


BOOST_AUTO_TEST_CASE(getHand){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getHand(),hand);
    }

}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_CASE(setId)
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

BOOST_AUTO_TEST_CASE(setName)
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


BOOST_AUTO_TEST_CASE(setColor){

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

BOOST_AUTO_TEST_CASE(setScore){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player = Player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setScore(40);
        BOOST_CHECK_EQUAL(player.score,40);
    }

}

BOOST_AUTO_TEST_CASE(setNumTrains){

    {   cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setNbWagons(20);
        BOOST_CHECK_EQUAL(player.nbWagons,20);
    }

}

BOOST_AUTO_TEST_CASE(setNbStations){

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

BOOST_AUTO_TEST_CASE(setNbRoads){

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

BOOST_AUTO_TEST_CASE(setHand){

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
/*
BOOST_AUTO_TEST_CASE(AddRoadAndStation) {

    cardsState::DestinationCard dest("Paris", "Marseille", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);
    std::vector<cardsState::DestinationCard> destCards = {dest};
    std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
    Player player(1, "Alice", cardsState::ColorCard::RED, 0, 10, 1, 0, hand);

    player.addRoad();
    BOOST_CHECK_EQUAL(player.getNbRoads(), 1);

    player.addStation();
    BOOST_CHECK_EQUAL(player.getNbStations(), 2);
}

BOOST_AUTO_TEST_CASE(AddScore) {
    {
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "Alice", cardsState::ColorCard::RED, 10, 5, 1, 0, hand);

        player.addScore(15);
        BOOST_CHECK_EQUAL(player.getScore(), 25);
    }
}

BOOST_AUTO_TEST_CASE(RemoveTrain)
{  {
    cardsState::DestinationCard dest("Paris", "Marseille", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);
    std::vector<cardsState::DestinationCard> destCards = {dest};
    std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
    Player player(1, "Alice", cardsState::ColorCard::RED, 0, 5, 1, 0, hand);

    player.removeTrain(2);
    BOOST_CHECK_EQUAL(player.getNbWagons(), 3);

    player.removeTrain(10);
    BOOST_CHECK_EQUAL(player.getNbWagons(), 0);
}
}

BOOST_AUTO_TEST_CASE(AddAndRemoveWagonCard) {

    cardsState::DestinationCard dest("Strasbourg", "Lyon", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);
    std::vector<cardsState::DestinationCard> destCards = {dest};
    std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
    Player player(2, "Alexa", cardsState::ColorCard::RED, 0, 5, 1, 0, hand);

    auto* card = new cardsState::WagonCard(cardsState::ColorCard::BLUE);
    player.addWagonCard(card);
    BOOST_CHECK_EQUAL(player.getHand()->wagonCards->cards.size(), 2);


    std::vector<cardsState::WagonCard*> cardsToRemove = {card};
    player.removeWagonCard(cardsToRemove);
    BOOST_CHECK_EQUAL(player.getHand()->wagonCards->cards.size(), 1);


}

BOOST_AUTO_TEST_CASE(AddDestinationCard) {

    cardsState::DestinationCard dest("Strasbourg", "Lyon", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);
    std::vector<cardsState::DestinationCard> destCards = {dest};
    std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
    Player player(4, "Amina", cardsState::ColorCard::RED, 0, 5, 1, 0, hand);

    auto* destCard = new cardsState::DestinationCard("Paris", "Marseille", 12);
    player.addDestinationCard(destCard);
    BOOST_CHECK_EQUAL(player.getHand()->destinationCards->cards.size(), 2);
}

BOOST_AUTO_TEST_CASE(DisplayHand) {
    cardsState::DestinationCard dest("Strasbourg", "Lyon", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);
    std::vector<cardsState::DestinationCard> destCards = {dest};
    std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
    Player player(1, "Alice", cardsState::ColorCard::RED, 0, 5, 1, 0, hand);

    BOOST_CHECK_NO_THROW(player.displayHand());
}

 *
 *
 *
 *
 */

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
