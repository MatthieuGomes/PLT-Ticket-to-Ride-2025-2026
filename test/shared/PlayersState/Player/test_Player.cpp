#include <boost/test/unit_test.hpp>

#include "../../src/shared/playersState/Player.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

using namespace ::playersState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Constructor)
{
    std::cout << "Player Constructor Test Started ..." << std::endl;
    cardsState::DestinationCard dest("Paris", "Marseille", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);

    std::vector<cardsState::DestinationCard> destCards = {dest};
    std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

    auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

    Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
    BOOST_CHECK_EQUAL(player.id, 1);
    BOOST_CHECK_EQUAL(player.name, "yosra");
    BOOST_CHECK_EQUAL(player.color, cardsState::ColorCard::RED);
    BOOST_CHECK_EQUAL(player.score, 23);
    BOOST_CHECK_EQUAL(player.nbWagons, 21);
    BOOST_CHECK_EQUAL(player.nbStations, 10);
    BOOST_CHECK_EQUAL(player.nbRoads, 5);
    BOOST_CHECK_EQUAL(player.hand, hand);
    std::cout << "Player Constructor Test Finished !\n"
              << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(getId)
{
    {
        std::cout << "GetId Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getId(), 1);
        std::cout << "GetId Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getName)
{
    {
        std::cout << "GetName Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getName(), "yosra");
        std::cout << "GetName Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getColor)
{

    {
        std::cout << "GetColor Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getColor(), cardsState::ColorCard::RED);
        std::cout << "GetColor Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getScore)
{

    {
        std::cout << "GetScore Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getScore(), 23);
        std::cout << "GetScore Test Finished !\n"
                  << std::endl;
    }
}
BOOST_AUTO_TEST_CASE(getNumTrains)
{

    {
        std::cout << "GetNbWagons Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbWagons(), 21);
        std::cout << "GetNbWagons Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getNbStations)
{

    {
        std::cout << "GetNbStations Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbStations(), 10);
        std::cout << "GetNbStations Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getNbRoads)
{

    {
        std::cout << "GetNbRoads Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getNbRoads(), 5);
        std::cout << "GetNbRoads Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getHand)
{

    {
        std::cout << "GetHand Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        BOOST_CHECK_EQUAL(player.getHand(), hand);
        std::cout << "GetHand Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_CASE(setId)
{
    {
        std::cout << "SetId Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setId(2);
        BOOST_CHECK_EQUAL(player.id, 2);
        std::cout << "SetId Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setName)
{
    {
        std::cout << "SetName Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);

        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};

        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setName("sara");
        BOOST_CHECK_EQUAL(player.name, "sara");
        std::cout << "SetName Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setColor)
{

    {
        std::cout << "SetColor Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setColor(cardsState::ColorCard::GREEN);
        BOOST_CHECK_EQUAL(player.color, cardsState::ColorCard::GREEN);
        std::cout << "SetColor Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setScore)
{

    {
        std::cout << "SetScore Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player = Player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setScore(40);
        BOOST_CHECK_EQUAL(player.score, 40);
        std::cout << "SetScore Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setNumTrains)
{

    {
        std::cout << "SetNbWagons Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setNbWagons(20);
        BOOST_CHECK_EQUAL(player.nbWagons, 20);
        std::cout << "SetNbWagons Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setNbStations)
{

    {
        std::cout << "SetNbStations Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setNbStations(9);
        BOOST_CHECK_EQUAL(player.nbStations, 9);
        std::cout << "SetNbStations Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setNbRoads)
{

    {
        std::cout << "SetNbRoads Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);
        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setNbRoads(6);
        BOOST_CHECK_EQUAL(player.nbRoads, 6);
        std::cout << "SetNbRoads Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setHand)
{

    {
        std::cout << "SetHand Test Started ..." << std::endl;
        cardsState::DestinationCard dest("Paris", "Marseille", 12);
        cardsState::WagonCard wagon(cardsState::ColorCard::RED);
        std::vector<cardsState::DestinationCard> destCards = {dest};
        std::vector<cardsState::WagonCard> wagonCardsVec = {wagon};
        auto *hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

        cardsState::DestinationCard dest1("Lyon", "Toulouse", 15);
        cardsState::WagonCard wagon1(cardsState::ColorCard::BLUE);
        std::vector<cardsState::DestinationCard> destCards1 = {dest1};
        std::vector<cardsState::WagonCard> wagonCardsVec1 = {wagon1};
        auto *hand1 = new cardsState::PlayerCards(&destCards1, &wagonCardsVec1);

        Player player(1, "yosra", cardsState::ColorCard::RED, 23, 21, 10, 5, hand);
        player.setHand(hand1);
        BOOST_CHECK_EQUAL(player.hand, hand1);
        std::cout << "SetHand Test Finished !\n"
                  << std::endl;
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(addRoad)
{
    cardsState::DestinationCard dest("Paris", "Bruxelle", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);

    std::vector<cardsState::DestinationCard> destCards = { dest };
    std::vector<cardsState::WagonCard> wagonCardsVec = { wagon };
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

    Player player(1, "yosra", cardsState::ColorCard::RED, 0 , 10, 2, 3, hand);

    BOOST_CHECK_EQUAL(player.getNbRoads(), 3);
    player.addRoad();
    BOOST_CHECK_EQUAL(player.getNbRoads(), 4);
}

BOOST_AUTO_TEST_CASE(addStation)
{
    cardsState::DestinationCard dest("Paris", "Bruxelle", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);

    std::vector<cardsState::DestinationCard> destCards = { dest };
    std::vector<cardsState::WagonCard> wagonCardsVec = { wagon };
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);


    Player player(1, "yosra", cardsState::ColorCard::RED, 0, 10, 2, 3, hand);


    BOOST_CHECK_EQUAL(player.getNbStations(), 2);


    player.addStation();
    BOOST_CHECK_EQUAL(player.getNbStations(), 3);


}

BOOST_AUTO_TEST_CASE(addScore)
{
    cardsState::DestinationCard dest("Paris", "Marseille", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);

    std::vector<cardsState::DestinationCard> destCards = { dest };
    std::vector<cardsState::WagonCard> wagonCardsVec = { wagon };
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

    Player player(1, "yosra", cardsState::ColorCard::RED, 0 , 10, 2, 3, hand);

    BOOST_CHECK_EQUAL(player.getScore(), 0);

    player.addScore(10);
    BOOST_CHECK_EQUAL(player.getScore(), 10);
}

BOOST_AUTO_TEST_CASE(removeTrain)
{
    cardsState::DestinationCard dest("Paris", "Marseille", 12);
    cardsState::WagonCard wagon(cardsState::ColorCard::RED);
    std::vector<cardsState::DestinationCard> destCards = { dest };
    std::vector<cardsState::WagonCard> wagonCardsVec = { wagon };
    auto* hand = new cardsState::PlayerCards(&destCards, &wagonCardsVec);

    Player player(1, "yosra", cardsState::ColorCard::RED, 0, 10, 2, 3, hand);

    BOOST_CHECK_EQUAL(player.getNbWagons(), 10);

    player.removeTrain(3);
    BOOST_CHECK_EQUAL(player.getNbWagons(), 7);
}

BOOST_AUTO_TEST_CASE(calculateDestinationPoints)
{
    using namespace cardsState;

    DestinationCard* d1 = new DestinationCard("A", "B", 10);
    DestinationCard* d2 = new DestinationCard("C", "D", 8);

    std::vector<DestinationCard> destCardsVec;
    std::vector<WagonCard> wagonCardsVec;
    auto* hand = new PlayerCards(&destCardsVec, &wagonCardsVec);

    playersState::Player player(1, "tester", ColorCard::BLUE, 5, 10, 2, 3, hand);
    BOOST_CHECK_EQUAL(player.getScore(), 5);

    player.completedDestinations.clear();
    player.completedDestinations.push_back(d1);
    player.completedDestinations.push_back(d2);

    int gained = player.calculateDestinationPoints();
    BOOST_CHECK_EQUAL(gained, 18);
    BOOST_CHECK_EQUAL(player.getScore(), 23);

    delete d1;
    delete d2;
    delete hand;
}

BOOST_AUTO_TEST_CASE(canBuildRoad_success)
{

    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);
    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED),
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);

    playersState::Player player(1, "p1", cardsState::ColorCard::RED, 0, 2, 0, 0, hand);

    BOOST_CHECK(player.canBuildRoad(&map, paris, berlin));

    delete hand;
}

BOOST_AUTO_TEST_CASE(canBuildRoad_notEnoughWagons)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);


    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED),
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);

    playersState::Player player(1, "p2", cardsState::ColorCard::RED, 0, 1, 0, 0, hand);

    BOOST_CHECK_MESSAGE(!player.canBuildRoad(&map, paris, berlin), "Should be false: not enough wagons");

    delete hand;
}

BOOST_AUTO_TEST_CASE(canBuildRoad_notEnoughCards)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);


    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);

    playersState::Player player(1, "p3", cardsState::ColorCard::RED, 0, 2, 0, 0, hand);

    BOOST_CHECK_MESSAGE(!player.canBuildRoad(&map, paris, berlin), "Should be false: not enough wagon cards of required color");

    delete hand;
}

BOOST_AUTO_TEST_CASE(canBuildRoad_blocked)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);

    mapState::Road* road = map.getRoadBetweenStations(paris, berlin);
    BOOST_REQUIRE(road);


    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED),
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);
    playersState::Player player(1, "p4", cardsState::ColorCard::RED, 0,  2, 0, 0, hand);


    road->setBlockStatus(true);
    BOOST_CHECK_MESSAGE(!player.canBuildRoad(&map, paris, berlin), "Should be false: road is blocked");


    delete hand;
}

BOOST_AUTO_TEST_CASE(canBuildRoad_alreadyOwned)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);

    mapState::Road* road = map.getRoadBetweenStations(paris, berlin);
    BOOST_REQUIRE(road);


    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED),
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);
    playersState::Player player(1, "p5", cardsState::ColorCard::RED, 0,  2, 0, 0, hand);


    std::vector<cardsState::DestinationCard> destVec2;
    std::vector<cardsState::WagonCard> wagonVec2;
    auto* hand2 = new cardsState::PlayerCards(&destVec2, &wagonVec2);
    playersState::Player owner(2, "owner", cardsState::ColorCard::GREEN, 0, 10, 0, 0, hand2);

    road->setOwner(&owner);
    BOOST_CHECK_MESSAGE(!player.canBuildRoad(&map, paris, berlin), "Should be false: route already owned");

    delete hand;
    delete hand2;
}


#include <algorithm>

BOOST_AUTO_TEST_CASE(getClaimableRoads_success)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);

    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED),
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);
    playersState::Player player(1, "p_ok", cardsState::ColorCard::RED, 0,  2, 0, 0, hand);

    auto claimable = player.getClaimableRoads(&map);
    mapState::Road* road = map.getRoadBetweenStations(paris, berlin);
    BOOST_REQUIRE(road);

    BOOST_CHECK(std::find(claimable.begin(), claimable.end(), road) != claimable.end());

    delete hand;
}

BOOST_AUTO_TEST_CASE(getClaimableRoads_insufficient_wagons)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);

    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED),
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);
    playersState::Player player(1, "p_no_wagons", cardsState::ColorCard::RED, 0, 1, 0, 0, hand);

    auto claimable = player.getClaimableRoads(&map);
    mapState::Road* road = map.getRoadBetweenStations(paris, berlin);
    BOOST_REQUIRE(road);

    BOOST_CHECK(std::find(claimable.begin(), claimable.end(), road) == claimable.end());

    delete hand;
}

BOOST_AUTO_TEST_CASE(getClaimableRoads_insufficient_cards)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);

    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);
    playersState::Player player(1, "p_no_cards", cardsState::ColorCard::RED, 0, 2, 0, 0, hand);

    auto claimable = player.getClaimableRoads(&map);
    mapState::Road* road = map.getRoadBetweenStations(paris, berlin);
    BOOST_REQUIRE(road);
    BOOST_CHECK(std::find(claimable.begin(), claimable.end(), road) == claimable.end());

    delete hand;
}

BOOST_AUTO_TEST_CASE(getClaimableRoads_blocked_and_owned)
{
    mapState::MapState map;
    mapState::Station* paris = map.getStationByName("paris");
    mapState::Station* berlin = map.getStationByName("berlin");
    BOOST_REQUIRE(paris);
    BOOST_REQUIRE(berlin);

    mapState::Road* road = map.getRoadBetweenStations(paris, berlin);
    BOOST_REQUIRE(road);

    std::vector<cardsState::DestinationCard> destVec;
    std::vector<cardsState::WagonCard> wagonVec = {
        cardsState::WagonCard(cardsState::ColorCard::RED),
        cardsState::WagonCard(cardsState::ColorCard::RED)
    };
    auto* hand = new cardsState::PlayerCards(&destVec, &wagonVec);
    playersState::Player player(1, "p_test", cardsState::ColorCard::RED, 0, 2, 0, 0, hand);

    road->setBlockStatus(true);
    auto claimable_blocked = player.getClaimableRoads(&map);
    BOOST_CHECK(std::find(claimable_blocked.begin(), claimable_blocked.end(), road) == claimable_blocked.end());
    road->setBlockStatus(false);

    std::vector<cardsState::DestinationCard> destVec2;
    std::vector<cardsState::WagonCard> wagonVec2;
    auto* hand2 = new cardsState::PlayerCards(&destVec2, &wagonVec2);
    playersState::Player owner(2, "owner", cardsState::ColorCard::GREEN, 0, 10, 0, 0, hand2);
    road->setOwner(&owner);

    auto claimable_owned = player.getClaimableRoads(&map);
    BOOST_CHECK(std::find(claimable_owned.begin(), claimable_owned.end(), road) == claimable_owned.end());


    road->setOwner(nullptr);
    delete hand;
    delete hand2;
}
}

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

/* vim: set sw=2 sts=2 et : */
