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

using namespace ::playersState;

using playersInfos = std::tuple<std::string, PlayerColor, int, int, int, int, std::shared_ptr<cardsState::PlayerCards>>;
using playersInitInfos = std::tuple<std::string, PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

std::string test_init_stationA_name = "paris";
std::string test_init_stationB_name = "rome";
int test_init_destination_points = 12;
cardsState::ColorCard test_init_wagon_color = cardsState::ColorCard::RED;
std::shared_ptr<mapState::Station> test_init_stationA = std::make_shared<mapState::Station>(test_init_stationA_name, nullptr, nullptr);
std::shared_ptr<mapState::Station> test_init_stationB = std::make_shared<mapState::Station>(test_init_stationB_name, nullptr, nullptr);

std::string test_init_player_name = "yosra";
PlayerColor test_init_player_color = PlayerColor::RED;
int test_init_player_score = 23;
int test_init_player_nbWagons = 21;
int test_init_player_nbStations = 10;
int test_init_player_nbRoads = 5;

TEST(TestStaticAssert)
{
    CHECK(1);
}

SUITE_START(Constructors)

cardsState::DestinationCard test_constr_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points, false);
cardsState::WagonCard test_constr_wagon_card(test_init_wagon_color);

std::vector<std::shared_ptr<cardsState::DestinationCard>> test_constr_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points, false)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_constr_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_constr_hand = std::make_shared<cardsState::PlayerCards>(test_constr_dest_cards, test_constr_wagon_cards);

TEST(Basic)
{
    ANN_START("Basic Constructor")
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_constr_hand);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, test_init_player_score);
    CHECK_EQ(player.nbWagons, test_init_player_nbWagons);
    CHECK_EQ(player.nbStations, test_init_player_nbStations);
    CHECK_EQ(player.nbRoads, test_init_player_nbRoads);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("Player Constructor")
}

TEST(Default)
{
    ANN_START("Default Constructor")
    Player player;
    CHECK(player.name.empty());
    CHECK_EQ(player.score, -1);
    CHECK_EQ(player.nbWagons, -1);
    CHECK_EQ(player.nbStations, -1);
    CHECK_EQ(player.nbRoads, -1);
    CHECK_EQ(player.hand, nullptr);
    CHECK_EQ(player.color, PlayerColor::UNKNOWN);
    ANN_END("Default Constructor")
}

TEST(Init)
{
    ANN_START("Init Constructor")
    Player player = Player::Init(test_init_player_name, test_init_player_color, test_constr_hand);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, Player::startScore);
    CHECK_EQ(player.nbWagons, Player::startNbWagons);
    CHECK_EQ(player.nbStations, Player::startNbStations);
    CHECK_EQ(player.nbRoads, 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("Init Constructor")
}

SUITE_START(Generators)

TEST(fillFromInfos){
    ANN_START("fillFromInfos")
    Player player;
    playersInfos info = playersInfos(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_constr_hand);
    player.fillFromInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, test_init_player_score);
    CHECK_EQ(player.nbWagons, test_init_player_nbWagons);
    CHECK_EQ(player.nbStations, test_init_player_nbStations);
    CHECK_EQ(player.nbRoads, test_init_player_nbRoads);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("fillFromInfos")
}

TEST(fillFromInitInfos){
    ANN_START("fillFromInitInfos")
    Player player;
    playersInitInfos info = playersInitInfos(test_init_player_name, test_init_player_color, test_constr_hand);
    player.fillFromInitInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, Player::startScore);
    CHECK_EQ(player.nbWagons, Player::startNbWagons);
    CHECK_EQ(player.nbStations, Player::startNbStations);
    CHECK_EQ(player.nbRoads, 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("fillFromInitInfos")
}

SUITE_END() // Generators

SUITE_START(Wrappers)

TEST(FromInfos)
{
    ANN_START("FromInfos Constructor")
    playersInfos info = playersInfos(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_constr_hand);
    Player player = Player::PlayerFromInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, test_init_player_score);
    CHECK_EQ(player.nbWagons, test_init_player_nbWagons);
    CHECK_EQ(player.nbStations, test_init_player_nbStations);
    CHECK_EQ(player.nbRoads, test_init_player_nbRoads);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("FromInfos Constructor")
}

TEST(FromInitInfos)
{
    ANN_START("FromInitInfos Constructor")
    playersInitInfos info = playersInitInfos(test_init_player_name, test_init_player_color, test_constr_hand);
    Player player = Player::PlayerFromInitInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, Player::startScore);
    CHECK_EQ(player.nbWagons, Player::startNbWagons);
    CHECK_EQ(player.nbStations, Player::startNbStations);
    CHECK_EQ(player.nbRoads, 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("FromInitInfos Constructor")
}

SUITE_END() // Wrappers

SUITE_START(BatchConstructors)

TEST(BatchFromInfos)
{
    ANN_START("BatchFromInfos Constructor")
    playersInfos info1 = playersInfos(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_constr_hand);
    playersInfos info2 = playersInfos("sara", PlayerColor::BLUE, 30, 25, 8, 7, test_constr_hand);
    std::vector<playersInfos> infos = {info1, info2};
    std::vector<std::shared_ptr<Player>> players = Player::BatchFromInfos(infos);
    CHECK_EQ(players.size(), 2);
    CHECK_EQ(players[0]->name, test_init_player_name);
    CHECK_EQ(players[1]->name, "sara");
    ANN_END("BatchFromInfos Constructor")
}

TEST(BatchFromInitInfos)
{
    ANN_START("BatchFromInitInfos Constructor")
    playersInitInfos info1 = playersInitInfos(test_init_player_name, test_init_player_color, test_constr_hand);
    playersInitInfos info2 = playersInitInfos("sara", PlayerColor::BLUE, test_constr_hand);
    std::vector<playersInitInfos> infos = {info1, info2};
    std::vector<std::shared_ptr<Player>> players = Player::BatchFromInitInfos(infos);
    CHECK_EQ(players.size(), 2);
    CHECK_EQ(players[0]->name, test_init_player_name);
    CHECK_EQ(players[1]->name, "sara");
    ANN_END("BatchFromInitInfos Constructor")
}

SUITE_END() // BatchConstructors

SUITE_END() // Constructors

SUITE_START(GettersAndSetters)

cardsState::DestinationCard test_getset_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points, false);
cardsState::WagonCard test_getset_wagon_card(cardsState::ColorCard::RED);

std::vector<std::shared_ptr<cardsState::DestinationCard>> test_getset_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points, false)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_getset_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_getset_hand = std::make_shared<cardsState::PlayerCards>(test_getset_dest_cards, test_getset_wagon_cards);

SUITE_START(Getters)

Player test_getter_player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_getset_hand);

TEST(getName)
{
    ANN_START("getName")
    CHECK_EQ(test_getter_player.getName(), test_init_player_name);
    ANN_END("getName")
}

TEST(getColor)
{
    ANN_START("getColor")
    CHECK_EQ(test_getter_player.getColor(), test_init_player_color);
    ANN_END("getColor")
}

TEST(getScore)
{
    ANN_START("getScore")
    CHECK_EQ(test_getter_player.getScore(), test_init_player_score);
    ANN_END("getScore")
}

TEST(getNbWagons)
{
    ANN_START("getNbWagons")
    CHECK_EQ(test_getter_player.getNbWagons(), test_init_player_nbWagons);
    ANN_END("getNbWagons")
}

TEST(getNbStations)
{
    ANN_START("getNbStations")
    CHECK_EQ(test_getter_player.getNbStations(), test_init_player_nbStations);
    ANN_END("getNbStations")
}

TEST(getNbRoads)
{
    ANN_START("getNbRoads")
    CHECK_EQ(test_getter_player.getNbRoads(), test_init_player_nbRoads);
    ANN_END("getNbRoads")
}

TEST(getHand)
{
    ANN_START("getHand")
    CHECK_EQ(test_getter_player.getHand(), test_getset_hand);
    ANN_END("getHand")
}

SUITE_END() // Getters

SUITE_START(Setters)

cardsState::DestinationCard dest(test_init_stationA, test_init_stationB, test_init_destination_points, false);
cardsState::WagonCard wagon(cardsState::ColorCard::RED);

std::vector<std::shared_ptr<cardsState::DestinationCard>> test_init_destCards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points,false)};
std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> hand = std::make_shared<cardsState::PlayerCards>(test_init_destCards, wagonCardsVec);

TEST(setScore)
{
    ANN_START("setScore")
    int test_new_score = 40;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    player.setScore(test_new_score);
    CHECK_EQ(player.score, test_new_score);
    ANN_END("setScore")
}

TEST(setNbWagons)
{
    ANN_START("setNbWagons")
    int test_new_nbWagons = 20;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    player.setNbWagons(test_new_nbWagons);
    CHECK_EQ(player.nbWagons, test_new_nbWagons);
    ANN_END("setNbWagons")
}

TEST(setNbStations)
{
    ANN_START("setNbStations")
    int test_new_nbStations = 9;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    player.setNbStations(test_new_nbStations);
    CHECK_EQ(player.nbStations, test_new_nbStations);
    ANN_END("setNbStations")
}

TEST(setHand)
{
    ANN_START("setHand")
    std::string test_new_stationA_name = "berlin";
    std::string test_new_stationB_name = "rome";
    std::shared_ptr<mapState::Station> test_new_stationA = std::make_shared<mapState::Station>(test_new_stationA_name, nullptr, nullptr);
    std::shared_ptr<mapState::Station> test_new_stationB = std::make_shared<mapState::Station>(test_new_stationB_name, nullptr, nullptr);

    int test_new_destination_points = 15;
    cardsState::ColorCard test_new_wagon_color = cardsState::ColorCard::BLUE;
    std::vector<std::shared_ptr<cardsState::DestinationCard>> test_new_destCards = {std::make_shared<cardsState::DestinationCard>(test_new_stationA, test_new_stationB, test_new_destination_points,false)};
    std::vector<std::shared_ptr<cardsState::WagonCard>> test_new_wagonCards = {std::make_shared<cardsState::WagonCard>(test_new_wagon_color)};
    std::shared_ptr<cardsState::PlayerCards> test_new_hand = std::make_shared<cardsState::PlayerCards>(test_new_destCards, test_new_wagonCards);

    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, hand);
    player.setHand(test_new_hand);
    CHECK_EQ(player.hand, test_new_hand);
    ANN_END("setHand")
}

SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

cardsState::DestinationCard test_internal_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points, false);
cardsState::WagonCard test_internal_wagon_card(test_init_wagon_color);
std::vector<std::shared_ptr<cardsState::DestinationCard>> test_internal_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points,false)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_internal_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_internal_hand = std::make_shared<cardsState::PlayerCards>(test_internal_dest_cards, test_internal_wagon_cards);

TEST(addScore)
{
    ANN_START("addScore")
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
    int test_new_score_to_add = 10;
    CHECK_EQ(player.score, test_init_player_score);
    player.addScore(test_new_score_to_add);
    CHECK_EQ(player.score, test_new_score_to_add + test_init_player_score);
    ANN_END("addScore")
}

TEST(removeTrain)
{
    ANN_START("removeTrain")
    {
        ANN_START("enough trains case")
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_trains_to_remove = 3;
        CHECK_EQ(player.getNbWagons(), test_init_player_nbWagons);

        player.removeTrain(nb_trains_to_remove);
        CHECK_EQ(player.getNbWagons(), test_init_player_nbWagons - nb_trains_to_remove);
        ANN_END("enough trains case")
    }
    {
        ANN_START("not enough trains case")
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_trains_to_remove = 25;
        CHECK_EQ(player.getNbWagons(), test_init_player_nbWagons);

        CHECK_THROW(player.removeTrain(nb_trains_to_remove), std::invalid_argument);
        CHECK_EQ(player.getNbWagons(), test_init_player_nbWagons);
        ANN_END("not enough trains case")
    }
    ANN_END("removeTrain")
}

TEST(removeStation)
{
    ANN_START("removeStation")
    {
        ANN_START("enough stations case")
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_stations_to_remove = 2;
        CHECK_EQ(player.getNbStations(), test_init_player_nbStations);

        player.removeStation(nb_stations_to_remove);
        CHECK_EQ(player.getNbStations(), test_init_player_nbStations - nb_stations_to_remove);
        ANN_END("enough stations case")
    }
    {
        ANN_START("not enough stations case")
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_internal_hand);
        int nb_stations_to_remove = 15;
        CHECK_EQ(player.getNbStations(), test_init_player_nbStations);

        CHECK_THROW(player.removeStation(nb_stations_to_remove), std::invalid_argument);
        CHECK_EQ(player.getNbStations(), test_init_player_nbStations);
        ANN_END("not enough stations case")
    }
    ANN_END("removeStation")
}

SUITE_END() // Internal

SUITE_START(Interactions)
std::string test_interract_stationA_name = "A";
std::string test_interract_stationB_name = "B";
std::shared_ptr<mapState::MapState> test_interact_map = std::make_shared<mapState::MapState>(mapState::MapState::Test());
cardsState::DestinationCard test_interact_dest_card(test_init_stationA, test_init_stationB, test_init_destination_points, false);
cardsState::WagonCard test_interact_wagon_card(test_init_wagon_color);
std::vector<std::shared_ptr<cardsState::DestinationCard>> test_interact_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points,false)};
std::vector<std::shared_ptr<cardsState::WagonCard>> test_interact_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
std::shared_ptr<cardsState::PlayerCards> test_interact_hand = std::make_shared<cardsState::PlayerCards>(test_interact_dest_cards, test_interact_wagon_cards);


TEST(calculateDestinationPoints)
{
    ANN_START("calculateDestinationPoints")
    {
        ANN_START("completed destinations not empty case")
        int d1_points = 10;
        int d2_points = 8;
        std::shared_ptr<mapState::Station> d1_stationA = std::make_shared<mapState::Station>("A", nullptr, nullptr);
        std::shared_ptr<mapState::Station> d1_stationB = std::make_shared<mapState::Station>("B", nullptr, nullptr);
        std::shared_ptr<mapState::Station> d2_stationA = std::make_shared<mapState::Station>("C", nullptr, nullptr);
        std::shared_ptr<mapState::Station> d2_stationB = std::make_shared<mapState::Station>("D", nullptr, nullptr);
        std::shared_ptr<cardsState::DestinationCard> d1 = std::make_shared<cardsState::DestinationCard>(d1_stationA, d1_stationB, d1_points, false);
        std::shared_ptr<cardsState::DestinationCard> d2 = std::make_shared<cardsState::DestinationCard>(d2_stationA, d2_stationB, d2_points, false);
        std::vector<std::shared_ptr<cardsState::DestinationCard>> dest_cards = {d1, d2};
        std::vector<std::shared_ptr<cardsState::WagonCard>> wagon_cards = {std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED)};
        std::shared_ptr<cardsState::PlayerCards> test_calculate_hand = std::make_shared<cardsState::PlayerCards>(dest_cards, wagon_cards);

        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_calculate_hand);
        CHECK_EQ(player.score, test_init_player_score);

        player.completedDestinations.clear();
        player.completedDestinations.push_back(std::move(d1));
        player.completedDestinations.push_back(std::move(d2));

        int gained = player.calculateDestinationPoints();
        CHECK_EQ(gained, 18);
        ANN_END("completed destinations not empty case")
    }
    {
        ANN_START("completed destinations empty case")
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
        CHECK_EQ(player.score, test_init_player_score);

        player.completedDestinations.clear();

        int gained = player.calculateDestinationPoints();
        CHECK_EQ(gained, 0);
        ANN_END("completed destinations empty case")
    }
    ANN_END("calculateDestinationPoints")
}

// TODO : complete tests (need mapstate methods) + restructure (testing ferries & roads => different map state)
TEST(isRoadBuildable)
{
    ANN_START("isRoadBuildable")
    std::shared_ptr<mapState::Station> test_stationA = test_interact_map->getStationByName(test_interract_stationA_name);
    std::shared_ptr<mapState::Station> test_stationB = test_interact_map->getStationByName(test_interract_stationB_name);
    std::vector<std::shared_ptr<mapState::Road>> roads = test_interact_map->getRoadsBetweenStations(test_stationA, test_stationB);
    std::shared_ptr<mapState::Road> road = roads[0];
    REQUIRE(test_stationA);
    REQUIRE(test_stationB);
    int test_nb_wagons_insufficient = 1;
    {
        ANN_START("not enough trains case")
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_nb_wagons_insufficient, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
        CHECK_EQ(player.isRoadBuildable(test_interact_map, road), false);
        ANN_END("not enough trains case")
    }

    {
        ANN_START("not enough wagon cards case")
        std::vector<std::shared_ptr<cardsState::DestinationCard>> test_insufficient_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points,false)};
        std::vector<std::shared_ptr<cardsState::WagonCard>> test_insufficient_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
        std::shared_ptr<cardsState::PlayerCards> test_insufficient_hand = std::make_shared<cardsState::PlayerCards>(test_insufficient_dest_cards, test_insufficient_wagon_cards);
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_insufficient_hand);
        CHECK_EQ(player.isRoadBuildable(test_interact_map, road), false);
        ANN_END("not enough wagon cards case")
    }

    std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("owner", PlayerColor::GREEN, 0, 10, 0, 0, test_interact_hand);
    {
        ANN_START("already owned case")
        REQUIRE(road);
        road->setOwner(test_owner);
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, test_init_player_nbRoads, test_interact_hand);
        CHECK_EQ(player.isRoadBuildable(test_interact_map, road), false);
        ANN_END("already owned case")
    }
    ANN_END("isRoadBuildable")
}
// TODO : complete tests (need mapstate methods) + restructure (testing ferries & roads => different map state)
TEST(getClaimableRoads)
{
    ANN_START("getClaimableRoads")
    {
        
        ANN_START("success case")

        ANN_END("success case")
    }
    {
        ANN_START("insufficient wagons case")

        ANN_END("insufficient wagons case")
    }
    {
        ANN_START("insufficient cards case")

        ANN_END("insufficient cards case")
    }
    {
        ANN_START("blocked or owned case")

        ANN_END("blocked or owned case")
    }
    ANN_END("getClaimableRoads")
}

// TODO : complete tests
TEST(getLongestPathLength)
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

SUITE_END() // Interactions

SUITE_END() // Operations
/* vim: set sw=2 sts=2 et : */
