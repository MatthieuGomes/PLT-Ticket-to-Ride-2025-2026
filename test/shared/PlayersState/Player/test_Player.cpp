#include <boost/test/unit_test.hpp>

#include "../../src/shared/playersState/Player.h"

#include <algorithm>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

// FIXME : needs new constructors for mapstate to be able to test more correctly

using namespace ::playersState;

std::string test_init_stationA_name = "paris";
std::string test_init_stationB_name = "rome";
int test_init_destination_points = 12;
cardsState::ColorCard test_init_wagon_color = cardsState::ColorCard::RED;
std::shared_ptr<mapState::Station> test_init_stationA = std::make_shared<mapState::Station>(test_init_stationA_name, nullptr, false, nullptr);
std::shared_ptr<mapState::Station> test_init_stationB = std::make_shared<mapState::Station>(test_init_stationB_name, nullptr, false, nullptr);

std::string test_init_player_name = "yosra";
PlayerColor test_init_player_color = PlayerColor::RED;
int test_init_player_score = 23;
int test_init_player_nbWagons = 21;
int test_init_player_nbStations = 10;
int test_init_player_nbRoads = 5;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructors)

cardsState::DestinationCard test_constr_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points);
cardsState::WagonCard test_constr_wagon_card(test_init_wagon_color);

std::vector<std::shared_ptr<cardsState::DestinationCard>> test_constr_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_constr_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_constr_hand = std::make_shared<cardsState::PlayerCards>(test_constr_dest_cards, test_constr_wagon_cards);

BOOST_AUTO_TEST_CASE(Constructor)
{
    {
        std::cout << "Player Constructor Test Started ..." << std::endl;
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_constr_hand);
        BOOST_CHECK_EQUAL(player.name, test_init_player_name);
        BOOST_CHECK_EQUAL(player.color, test_init_player_color);
        BOOST_CHECK_EQUAL(player.score, test_init_player_score);
        BOOST_CHECK_EQUAL(player.nbWagons, test_init_player_nbWagons);
        BOOST_CHECK_EQUAL(player.nbStations, test_init_player_nbStations);
        BOOST_CHECK_EQUAL(player.nbRoads, test_init_player_nbRoads);
        BOOST_CHECK_EQUAL(player.hand, test_constr_hand);
        std::cout << "Player Constructor Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END() // Constructors

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

cardsState::DestinationCard test_getset_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points);
cardsState::WagonCard test_getset_wagon_card(cardsState::ColorCard::RED);

std::vector<std::shared_ptr<cardsState::DestinationCard>> test_getset_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_getset_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_getset_hand = std::make_shared<cardsState::PlayerCards>(test_getset_dest_cards, test_getset_wagon_cards);

BOOST_AUTO_TEST_SUITE(Getters)

Player test_getter_player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_getset_hand);

BOOST_AUTO_TEST_CASE(getName)
{
    {
        std::cout << "GetName Test Started ..." << std::endl;

        BOOST_CHECK_EQUAL(test_getter_player.getName(), test_init_player_name);
        std::cout << "GetName Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getColor)
{

    {
        std::cout << "GetColor Test Started ..." << std::endl;

        BOOST_CHECK_EQUAL(test_getter_player.getColor(), test_init_player_color);
        std::cout << "GetColor Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getScore)
{

    {
        std::cout << "GetScore Test Started ..." << std::endl;
        BOOST_CHECK_EQUAL(test_getter_player.getScore(), test_init_player_score);
        std::cout << "GetScore Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getNumTrains)
{

    {
        std::cout << "GetNbWagons Test Started ..." << std::endl;
        BOOST_CHECK_EQUAL(test_getter_player.getNbWagons(), test_init_player_nbWagons);
        std::cout << "GetNbWagons Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getNbStations)
{

    {
        std::cout << "GetNbStations Test Started ..." << std::endl;
        BOOST_CHECK_EQUAL(test_getter_player.getNbStations(), test_init_player_nbStations);
        std::cout << "GetNbStations Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getNbRoads)
{

    {
        std::cout << "GetNbRoads Test Started ..." << std::endl;
        BOOST_CHECK_EQUAL(test_getter_player.getNbRoads(), test_init_player_nbRoads);
        std::cout << "GetNbRoads Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(getHand)
{

    {
        std::cout << "GetHand Test Started ..." << std::endl;
        BOOST_CHECK_EQUAL(test_getter_player.getHand(), test_getset_hand);
        std::cout << "GetHand Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END() // Getters

BOOST_AUTO_TEST_SUITE(Setters)

cardsState::DestinationCard dest(test_init_stationA, test_init_stationB, test_init_destination_points);
cardsState::WagonCard wagon(cardsState::ColorCard::RED);

std::vector<std::shared_ptr<cardsState::DestinationCard>> test_init_destCards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points)};
std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> hand = std::make_shared<cardsState::PlayerCards>(test_init_destCards, wagonCardsVec);

BOOST_AUTO_TEST_CASE(setName)
{
    std::string test_new_name = "sara";
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    {
        std::cout << "SetName Test Started ..." << std::endl;
        player.setName(test_new_name);
        BOOST_CHECK_EQUAL(player.name, test_new_name);
        std::cout << "SetName Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setColor)
{
    playersState::PlayerColor test_new_color = playersState::PlayerColor::GREEN;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    {
        std::cout << "SetColor Test Started ..." << std::endl;
        player.setColor(test_new_color);
        BOOST_CHECK_EQUAL(player.color, test_new_color);
        std::cout << "SetColor Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setScore)
{
    int test_new_score = 40;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    {
        std::cout << "SetScore Test Started ..." << std::endl;
        player.setScore(test_new_score);
        BOOST_CHECK_EQUAL(player.score, test_new_score);
        std::cout << "SetScore Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setNumTrains)
{
    int test_new_nbWagons = 20;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    {
        std::cout << "SetNbWagons Test Started ..." << std::endl;
        player.setNbWagons(test_new_nbWagons);
        BOOST_CHECK_EQUAL(player.nbWagons, test_new_nbWagons);
        std::cout << "SetNbWagons Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setNbStations)
{
    int test_new_nbStations = 9;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    {
        std::cout << "SetNbStations Test Started ..." << std::endl;
        player.setNbStations(test_new_nbStations);
        BOOST_CHECK_EQUAL(player.nbStations, test_new_nbStations);
        std::cout << "SetNbStations Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setNbRoads)
{
    int test_new_nbRoads = 6;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    {
        std::cout << "SetNbRoads Test Started ..." << std::endl;
        player.setNbRoads(test_new_nbRoads);
        BOOST_CHECK_EQUAL(player.nbRoads, test_new_nbRoads);
        std::cout << "SetNbRoads Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(setHand)
{
    std::string test_new_stationA_name = "berlin";
    std::string test_new_stationB_name = "rome";
    std::shared_ptr<mapState::Station> test_new_stationA = std::make_shared<mapState::Station>(test_new_stationA_name, nullptr, false, nullptr);
    std::shared_ptr<mapState::Station> test_new_stationB = std::make_shared<mapState::Station>(test_new_stationB_name, nullptr, false, nullptr);

    int test_new_destination_points = 15;
    cardsState::ColorCard test_new_wagon_color = cardsState::ColorCard::BLUE;
    std::vector<std::shared_ptr<cardsState::DestinationCard>> test_new_destCards = {std::make_shared<cardsState::DestinationCard>(test_new_stationA, test_new_stationB, test_new_destination_points)};
    std::vector<std::shared_ptr<cardsState::WagonCard>> test_new_wagonCards = {std::make_shared<cardsState::WagonCard>(test_new_wagon_color)};
    std::shared_ptr<cardsState::PlayerCards> test_new_hand = std::make_shared<cardsState::PlayerCards>(test_new_destCards, test_new_wagonCards);

    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    {
        std::cout << "SetHand Test Started ..." << std::endl;
        player.setHand(test_new_hand);
        BOOST_CHECK_EQUAL(player.hand, test_new_hand);
        std::cout << "SetHand Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END() // Setters

BOOST_AUTO_TEST_SUITE_END() // GettersAndSetters

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

cardsState::DestinationCard test_internal_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points);
cardsState::WagonCard test_internal_wagon_card(test_init_wagon_color);
std::vector<std::shared_ptr<cardsState::DestinationCard>> test_internal_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_internal_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_internal_hand = std::make_shared<cardsState::PlayerCards>(test_internal_dest_cards, test_internal_wagon_cards);

BOOST_AUTO_TEST_CASE(addScore)
{
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
    int test_new_score_to_add = 10;
    {
        std::cout << "AddScore Test Started ..." << std::endl;
        BOOST_CHECK_EQUAL(player.score, test_init_player_score);
        player.addScore(test_new_score_to_add);
        BOOST_CHECK_EQUAL(player.score, test_new_score_to_add + test_init_player_score);
        std::cout << "AddScore Test Finished !\n"
                  << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(removeTrain)
{

    std::cout << "RemoveTrain Test Started ..." << std::endl;
    {
        std::cout << "enough trains case ..." << std::endl;
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_trains_to_remove = 3;
        BOOST_CHECK_EQUAL(player.getNbWagons(), test_init_player_nbWagons);

        player.removeTrain(nb_trains_to_remove);
        BOOST_CHECK_EQUAL(player.getNbWagons(), test_init_player_nbWagons - nb_trains_to_remove);
        std::cout << "enough trains case finished !" << std::endl;
    }
    {
        std::cout << "not enough trains case ..." << std::endl;
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_trains_to_remove = 25;
        BOOST_CHECK_EQUAL(player.getNbWagons(), test_init_player_nbWagons);

        BOOST_CHECK_THROW(player.removeTrain(nb_trains_to_remove), std::invalid_argument);
        BOOST_CHECK_EQUAL(player.getNbWagons(), test_init_player_nbWagons);
        std::cout << "not enough trains case finished !" << std::endl;
    }
    std::cout << "RemoveTrain Test Finished !\n"
              << std::endl;
}

// FIXME : add method
BOOST_AUTO_TEST_CASE(RemoveStation)
{
    std::cout << "RemoveStation Test Started ..." << std::endl;
    {
        std::cout << "enough stations case ..." << std::endl;
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_stations_to_remove = 2;
        BOOST_CHECK_EQUAL(player.getNbStations(), test_init_player_nbStations);

        player.removeStation(nb_stations_to_remove);
        BOOST_CHECK_EQUAL(player.getNbStations(), test_init_player_nbStations - nb_stations_to_remove);
        std::cout << "enough stations case finished !" << std::endl;
    }
    {
        std::cout << "not enough stations case ..." << std::endl;
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_stations_to_remove = 15;
        BOOST_CHECK_EQUAL(player.getNbStations(), test_init_player_nbStations);

        BOOST_CHECK_THROW(player.removeStation(nb_stations_to_remove), std::invalid_argument);
        BOOST_CHECK_EQUAL(player.getNbStations(), test_init_player_nbStations);
        std::cout << "not enough stations case finished !" << std::endl;
    }
    std::cout << "RemoveStation Test Finished !\n"
              << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // Internal

BOOST_AUTO_TEST_SUITE(Interactions)
std::string test_interract_stationA_name = "paris";
std::string test_interract_stationB_name = "berlin";
std::shared_ptr<mapState::MapState> test_interact_map = std::make_shared<mapState::MapState>();
cardsState::DestinationCard test_interact_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points);
cardsState::WagonCard test_interact_wagon_card(test_init_wagon_color);
std::vector<std::shared_ptr<cardsState::DestinationCard>> test_interact_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_interact_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_interact_hand = std::make_shared<cardsState::PlayerCards>(test_interact_dest_cards, test_interact_wagon_cards);

BOOST_AUTO_TEST_CASE(addRoad)
{
    std::cout << "addRoad Test Started ..." << std::endl;
    playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
    BOOST_CHECK_EQUAL(player.nbRoads, test_init_player_nbRoads);
    player.addRoad();
    BOOST_CHECK_EQUAL(player.nbRoads, 6);
    std::cout << "addRoad Test Finished !\n"
              << std::endl;
}

BOOST_AUTO_TEST_CASE(addStation)
{
    std::cout << "addStation Test Started ..." << std::endl;
    playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);

    BOOST_CHECK_EQUAL(player.nbStations, test_init_player_nbStations);

    player.addStation();
    BOOST_CHECK_EQUAL(player.nbStations, 11);
    std::cout << "addStation Test Finished !\n"
              << std::endl;
}

BOOST_AUTO_TEST_CASE(calculateDestinationPoints)
{
    std::cout << "calculateDestinationPoints Test Started ..." << std::endl;
    {
        std::cout << "completed destinations not empty case ..." << std::endl;
        int d1_points = 10;
        int d2_points = 8;
        std::shared_ptr<mapState::Station> d1_stationA = std::make_shared<mapState::Station>("paris", nullptr, false, nullptr);
        std::shared_ptr<mapState::Station> d1_stationB = std::make_shared<mapState::Station>("berlin", nullptr, false, nullptr);
        std::shared_ptr<mapState::Station> d2_stationA = std::make_shared<mapState::Station>("madrid", nullptr, false, nullptr);
        std::shared_ptr<mapState::Station> d2_stationB = std::make_shared<mapState::Station>("rome", nullptr, false, nullptr);
        std::shared_ptr<cardsState::DestinationCard> d1 = std::make_shared<cardsState::DestinationCard>(d1_stationA, d1_stationB, d1_points);
        std::shared_ptr<cardsState::DestinationCard> d2 = std::make_shared<cardsState::DestinationCard>(d2_stationA, d2_stationB, d2_points);
        std::vector<std::shared_ptr<cardsState::DestinationCard>> dest_cards = {d1, d2};
        std::vector<std::shared_ptr<cardsState::WagonCard>> wagon_cards = {std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED)};
        std::shared_ptr<cardsState::PlayerCards> test_calculate_hand = std::make_shared<cardsState::PlayerCards>(dest_cards, wagon_cards);

        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_calculate_hand);
        BOOST_CHECK_EQUAL(player.score, test_init_player_score);

        player.completedDestinations.clear();
        player.completedDestinations.push_back(std::move(d1));
        player.completedDestinations.push_back(std::move(d2));

        int gained = player.calculateDestinationPoints();
        BOOST_CHECK_EQUAL(gained, 18);
        std::cout << "completed destinations not empty case finished !" << std::endl;
    }
    {
        std::cout << "completed destinations empty case ..." << std::endl;
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
        BOOST_CHECK_EQUAL(player.score, test_init_player_score);

        player.completedDestinations.clear();

        int gained = player.calculateDestinationPoints();
        BOOST_CHECK_EQUAL(gained, 0);
        std::cout << "completed destinations empty case finished !" << std::endl;
    }
    std::cout << "calculateDestinationPoints Test Finished !\n"
              << std::endl;
}

// TODO : complete tests (need mapstate methods) + restructure (testing ferries & roads => different map state)
BOOST_AUTO_TEST_CASE(isRoadBuildable)
{
    std::cout << "isRoadBuildable Test Started ..." << std::endl;
    std::shared_ptr<mapState::Station> test_stationA = test_interact_map->getStationByName(test_interract_stationA_name);
    std::shared_ptr<mapState::Station> test_stationB = test_interact_map->getStationByName(test_interract_stationB_name);
    std::vector<std::shared_ptr<mapState::Road>> roads = test_interact_map->getRoadBetweenStations(test_stationA, test_stationB);
    std::shared_ptr<mapState::Road> road = roads[0];
    BOOST_REQUIRE(test_stationA);
    BOOST_REQUIRE(test_stationB);
    int test_nb_wagons_insufficient = 1;
    {
        std::cout << "\tnot enough trains case started ...  " << std::endl;
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_nb_wagons_insufficient, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
        BOOST_CHECK_EQUAL(player.isRoadBuildable(test_interact_map, road), false);
        std::cout << "\tnot enough trains case finished !\n"
                  << std::endl;
    }

    {
        std::cout << "\tnot enough wagon cards case started ...  " << std::endl;
        std::vector<std::shared_ptr<cardsState::DestinationCard>> test_insufficient_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points)};
        std::vector<std::shared_ptr<cardsState::WagonCard>> test_insufficient_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
        std::shared_ptr<cardsState::PlayerCards> test_insufficient_hand = std::make_shared<cardsState::PlayerCards>(test_insufficient_dest_cards, test_insufficient_wagon_cards);
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_insufficient_hand);
        BOOST_CHECK_EQUAL(player.isRoadBuildable(test_interact_map, road), false);
        std::cout << "\tnot enough wagon cards case finished !\n"
                  << std::endl;
    }
    bool test_blocked_road = true;
    {
        std::cout << "\tblocked road case started ...  " << std::endl;
        BOOST_REQUIRE(road);
        road->setBlockStatus(test_blocked_road);
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
        BOOST_CHECK_EQUAL(player.isRoadBuildable(test_interact_map, road), false);
        std::cout << "\tblocked road case finished !\n"
                  << std::endl;
    }
    std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("owner", PlayerColor::GREEN, 0, 10, 0, 0, test_interact_hand);
    {
        road->setBlockStatus(false);
        std::cout << "\talready owned case started ...  " << std::endl;
        BOOST_REQUIRE(road);
        road->setOwner(test_owner);
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
        BOOST_CHECK_EQUAL(player.isRoadBuildable(test_interact_map, road), false);
        std::cout << "\talready owned case finished !\n"
                  << std::endl;
    }
    std::cout << "canBuildRoad Test Finished !\n"
              << std::endl;
}
// TODO : complete tests (need mapstate methods) + restructure (testing ferries & roads => different map state)
BOOST_AUTO_TEST_CASE(getClaimableRoads)
{
    std::cout << "getClaimableRoads Test Started ..." << std::endl;
    {
        std::cout << "sucess case started ...  " << std::endl;

        std::cout << "sucess case finished !\n"
                  << std::endl;
    }
    {
        std::cout << "insufficient wagons case started ...  " << std::endl;

        std::cout << "insufficient wagons case finished !\n"
                  << std::endl;
    }
    {
        std::cout << "insufficient cards case started ...  " << std::endl;

        std::cout << "insufficient cards case finished !\n"
                  << std::endl;
    }
    {
        std::cout << "blocked or owned case started ...  " << std::endl;

        std::cout << "blocked or owned case finished !\n"
                  << std::endl;
    }
    std::cout << "getClaimableRoads Test Finished !\n"
              << std::endl;
}

// TODO : complete tests 
BOOST_AUTO_TEST_CASE(getLongestPathLength)
{
    #if DEBUG
    std::cout << "getLongestPathLength Test Started ..." << std::endl;
    {
        std::cout << "normal case started ...  " << std::endl;
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);

        int length = player.getLongestPathLength(test_interact_map);
        std::cout << "calculated length: " << length << std::endl;

        std::cout << "normal case finished !\n"
                  << std::endl;
    }
    std::cout << "getLongestPathLength Test Finished !\n"
              << std::endl;
    #endif
}

BOOST_AUTO_TEST_SUITE_END() // Interactions

BOOST_AUTO_TEST_SUITE_END() // Operations
/* vim: set sw=2 sts=2 et : */
