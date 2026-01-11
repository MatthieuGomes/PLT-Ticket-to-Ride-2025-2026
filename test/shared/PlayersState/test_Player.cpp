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

using PlayersInfos = std::tuple<std::string, PlayerColor, int, int, int, std::vector<std::shared_ptr<mapState::Road>>, std::shared_ptr<cardsState::PlayerCards>>;
using PlayersInitInfos = std::tuple<std::string, PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

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
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_constr_hand);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, test_init_player_score);
    CHECK_EQ(player.nbWagons, test_init_player_nbWagons);
    CHECK_EQ(player.nbStations, test_init_player_nbStations);
    CHECK_EQ(player.borrowedRoads.size(), 0);
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
    CHECK_EQ(player.borrowedRoads.size(), 0);
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
    CHECK_EQ(player.borrowedRoads.size(), 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("Init Constructor")
}

SUITE_START(Generators)

TEST(fillFromInfos){
    ANN_START("fillFromInfos")
    Player player;
    PlayersInfos info = PlayersInfos(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_constr_hand);
    player.fillFromInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, test_init_player_score);
    CHECK_EQ(player.nbWagons, test_init_player_nbWagons);
    CHECK_EQ(player.nbStations, test_init_player_nbStations);
    CHECK_EQ(player.borrowedRoads.size(), 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("fillFromInfos")
}

TEST(fillFromInitInfos){
    ANN_START("fillFromInitInfos")
    Player player;
    PlayersInitInfos info = PlayersInitInfos(test_init_player_name, test_init_player_color, test_constr_hand);
    player.fillFromInitInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, Player::startScore);
    CHECK_EQ(player.nbWagons, Player::startNbWagons);
    CHECK_EQ(player.nbStations, Player::startNbStations);
    CHECK_EQ(player.borrowedRoads.size(), 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("fillFromInitInfos")
}

SUITE_END() // Generators

SUITE_START(Wrappers)

TEST(FromInfos)
{
    ANN_START("FromInfos Constructor")
    PlayersInfos info = PlayersInfos(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_constr_hand);
    Player player = Player::PlayerFromInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, test_init_player_score);
    CHECK_EQ(player.nbWagons, test_init_player_nbWagons);
    CHECK_EQ(player.nbStations, test_init_player_nbStations);
    CHECK_EQ(player.borrowedRoads.size(), 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("FromInfos Constructor")
}

TEST(FromInitInfos)
{
    ANN_START("FromInitInfos Constructor")
    PlayersInitInfos info = PlayersInitInfos(test_init_player_name, test_init_player_color, test_constr_hand);
    Player player = Player::PlayerFromInitInfos(info);
    CHECK_EQ(player.name, test_init_player_name);
    CHECK_EQ(player.color, test_init_player_color);
    CHECK_EQ(player.score, Player::startScore);
    CHECK_EQ(player.nbWagons, Player::startNbWagons);
    CHECK_EQ(player.nbStations, Player::startNbStations);
    CHECK_EQ(player.borrowedRoads.size(), 0);
    CHECK_EQ(player.hand, test_constr_hand);
    ANN_END("FromInitInfos Constructor")
}

SUITE_END() // Wrappers

SUITE_START(BatchConstructors)

TEST(BatchFromInfos)
{
    ANN_START("BatchFromInfos Constructor")
    PlayersInfos info1 = PlayersInfos(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_constr_hand);
    PlayersInfos info2 = PlayersInfos("sara", PlayerColor::BLUE, 30, 25, 8, 7, test_constr_hand);
    std::vector<PlayersInfos> infos = {info1, info2};
    std::vector<std::shared_ptr<Player>> players = Player::BatchFromInfos(infos);
    CHECK_EQ(players.size(), 2);
    CHECK_EQ(players[0]->name, test_init_player_name);
    CHECK_EQ(players[1]->name, "sara");
    ANN_END("BatchFromInfos Constructor")
}

TEST(BatchFromInitInfos)
{
    ANN_START("BatchFromInitInfos Constructor")
    PlayersInitInfos info1 = PlayersInitInfos(test_init_player_name, test_init_player_color, test_constr_hand);
    PlayersInitInfos info2 = PlayersInitInfos("sara", PlayerColor::BLUE, test_constr_hand);
    std::vector<PlayersInitInfos> infos = {info1, info2};
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

Player test_getter_player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_getset_hand);

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

TEST(getborrowedRoads)
{
    ANN_START("getborrowedRoads")
    CHECK_EQ(test_getter_player.getBorrowedRoads().size(), 0);
    ANN_END("getborrowedRoads")
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
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, hand);
    player.setScore(test_new_score);
    CHECK_EQ(player.score, test_new_score);
    ANN_END("setScore")
}

TEST(setNbWagons)
{
    ANN_START("setNbWagons")
    int test_new_nbWagons = 20;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, hand);
    player.setNbWagons(test_new_nbWagons);
    CHECK_EQ(player.nbWagons, test_new_nbWagons);
    ANN_END("setNbWagons")
}

TEST(setNbStations)
{
    ANN_START("setNbStations")
    int test_new_nbStations = 9;
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, hand);
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

    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, hand);
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
    Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_internal_hand);
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
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_internal_hand);
        int nb_trains_to_remove = 3;
        CHECK_EQ(player.getNbWagons(), test_init_player_nbWagons);

        player.removeTrain(nb_trains_to_remove);
        CHECK_EQ(player.getNbWagons(), test_init_player_nbWagons - nb_trains_to_remove);
        ANN_END("enough trains case")
    }
    {
        ANN_START("not enough trains case")
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_internal_hand);
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
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_internal_hand);
        int nb_stations_to_remove = 2;
        CHECK_EQ(player.getNbStations(), test_init_player_nbStations);

        player.removeStation(nb_stations_to_remove);
        CHECK_EQ(player.getNbStations(), test_init_player_nbStations - nb_stations_to_remove);
        ANN_END("enough stations case")
    }
    {
        ANN_START("not enough stations case")
        Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_internal_hand);
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

        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_calculate_hand);
        CHECK_EQ(player.score, test_init_player_score);

        player.completedDestinations.clear();
        player.completedDestinations.push_back(std::move(d1));
        player.completedDestinations.push_back(std::move(d2));

        int gained = player.calculatePoints();
        CHECK_EQ(gained, 18);
        ANN_END("completed destinations not empty case")
    }
    {
        ANN_START("completed destinations empty case")
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_interact_hand);
        CHECK_EQ(player.score, test_init_player_score);

        player.completedDestinations.clear();

        int gained = player.calculatePoints();
        CHECK_EQ(gained, 0);
        ANN_END("completed destinations empty case")
    }
    ANN_END("calculateDestinationPoints")
}


TEST(calculateWagonPoints)
{
    ANN_START("calculateWagonPoints")

    auto stationA = std::make_shared<mapState::Station>("A", nullptr, nullptr);
    auto stationB = std::make_shared<mapState::Station>("B", nullptr, nullptr);
    auto stationC = std::make_shared<mapState::Station>("C", nullptr, nullptr);
    auto stationD = std::make_shared<mapState::Station>("D", nullptr, nullptr);

    ANN_START("Case 1: Single length 1 route → 1 point")
    {
        auto road1 = std::make_shared<mapState::Road>(
            1, nullptr, stationA, stationB, mapState::RoadColor::RED, 1, nullptr
        );

        std::vector<std::shared_ptr<mapState::Road>> borrowed = {road1};

        playersState::Player player(
            "TestPlayer",
            PlayerColor::RED,
            0,
            45,
            3,
            borrowed,
            nullptr
        );

        CHECK_EQ(player.calculateWagonPoints(), 1);
    }
    ANN_END("Case 1: Single length 1 route → 1 point")

    ANN_START("Case 2: Routes of different lengths (classic case)")
    {
        auto road1 = std::make_shared<mapState::Road>(1,nullptr, stationA, stationB, mapState::RoadColor::RED, 1, nullptr);
        auto road2 = std::make_shared<mapState::Road>(2,nullptr, stationB, stationC, mapState::RoadColor::BLUE, 3, nullptr);
        auto road3 = std::make_shared<mapState::Road>(3,nullptr, stationC, stationD, mapState::RoadColor::GREEN, 6, nullptr);

        std::vector<std::shared_ptr<mapState::Road>> borrowed = {road1, road2, road3};

        playersState::Player player(
            "PlayerClassic",
            PlayerColor::BLUE,
            0,
            40,
            3,
            borrowed,
            nullptr
        );

        CHECK_EQ(player.calculateWagonPoints(), 20);
    }
    ANN_END("Case 2: Routes of different lengths (classic case)")

    ANN_START("Case 3: No routes built → 0 points")
    {
        playersState::Player player(
            "EmptyPlayer",
            PlayerColor::GREEN,
            0,
            45,
            3,
            {},
            nullptr
        );

        CHECK_EQ(player.calculateWagonPoints(), 0);
    }
    ANN_END("Case 3: No routes built → 0 points")

    ANN_START("Case 4: Double route (same length) + long route")
    {
        auto road1 = std::make_shared<mapState::Road>(1,nullptr, stationA, stationB, mapState::RoadColor::RED, 4, nullptr);
        auto road2 = std::make_shared<mapState::Road>(2,nullptr, stationA, stationB, mapState::RoadColor::BLUE, 4, nullptr);
        auto road3 = std::make_shared<mapState::Road>(3,nullptr, stationB, stationC, mapState::RoadColor::PINK, 2, nullptr);

        std::vector<std::shared_ptr<mapState::Road>> borrowed = {road1, road2, road3};

        playersState::Player player(
            "DoubleRoadPlayer",
            PlayerColor::YELLOW,
            0,
            35,
            2,
            borrowed,
            nullptr
        );

        CHECK_EQ(player.calculateWagonPoints(), 16);
    }
    ANN_END("Case 4: Double route (same length) + long route")

    ANN_END("calculateWagonPoints")
}



TEST(calculatePoints)
{
    ANN_START("calculatePoints")

    auto stationA = std::make_shared<mapState::Station>("A", nullptr, nullptr);
    auto stationB = std::make_shared<mapState::Station>("B", nullptr, nullptr);
    auto stationC = std::make_shared<mapState::Station>("C", nullptr, nullptr);

    ANN_START("Empty player - no roads, no completed destinations → 0 points")
    {
        playersState::Player player(
            "EmptyPlayer",
            PlayerColor::RED,
            0,
            45,
            3,
            {},
            nullptr
        );

        int total = player.calculatePoints();
        CHECK_EQ(total, 0);
    }
    ANN_END("Empty player - no roads, no completed destinations → 0 points")

    ANN_START("Classic case - some roads + completed destinations")
    {
        auto road1 = std::make_shared<mapState::Road>(1,nullptr, stationA, stationB, mapState::RoadColor::RED, 2, nullptr);
        auto road2 = std::make_shared<mapState::Road>(2,nullptr, stationB, stationC, mapState::RoadColor::BLUE, 4, nullptr);

        std::vector<std::shared_ptr<mapState::Road>> borrowed = {road1, road2};

        playersState::Player player(
            "NormalPlayer",
            PlayerColor::BLUE,
            0,
            39,
            3,
            borrowed,
            nullptr
        );

        player.completedDestinations = {
            std::make_shared<cardsState::DestinationCard>(stationA, stationB, 20, false),
            std::make_shared<cardsState::DestinationCard>(stationB, stationC, 13, false)
        };

        int total = player.calculatePoints();
        CHECK_EQ(total, 42);
    }
    ANN_END("Classic case - some roads + completed destinations")

    ANN_START("Only long roads - no destinations")
    {
        auto road1 = std::make_shared<mapState::Road>(1,nullptr, stationA, stationB, mapState::RoadColor::GREEN, 6, nullptr);
        auto road2 = std::make_shared<mapState::Road>(2,nullptr, stationB, stationC, mapState::RoadColor::PINK, 5, nullptr);

        std::vector<std::shared_ptr<mapState::Road>> borrowed = {road1, road2};

        playersState::Player player(
            "LongRoadsPlayer",
            PlayerColor::YELLOW,
            0,
            34,
            3,
            borrowed,
            nullptr
        );

        player.completedDestinations.clear();

        CHECK_EQ(player.calculatePoints(), 25);
    }
    ANN_END("Only long roads - no destinations")

    ANN_END("calculatePoints")
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
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_nb_wagons_insufficient, test_init_player_nbStations, {}, test_interact_hand);
        CHECK_EQ(player.isRoadBuildable(test_interact_map, road), false);
        ANN_END("not enough trains case")
    }

    {
        ANN_START("not enough wagon cards case")
        std::vector<std::shared_ptr<cardsState::DestinationCard>> test_insufficient_dest_cards = {std::make_shared<cardsState::DestinationCard>(test_init_stationA, test_init_stationB, test_init_destination_points,false)};
        std::vector<std::shared_ptr<cardsState::WagonCard>> test_insufficient_wagon_cards = {std::make_shared<cardsState::WagonCard>(test_init_wagon_color)};
        std::shared_ptr<cardsState::PlayerCards> test_insufficient_hand = std::make_shared<cardsState::PlayerCards>(test_insufficient_dest_cards, test_insufficient_wagon_cards);
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_insufficient_hand);
        CHECK_EQ(player.isRoadBuildable(test_interact_map, road), false);
        ANN_END("not enough wagon cards case")
    }

    std::shared_ptr<playersState::Player> test_owner = std::make_shared<playersState::Player>("test_owner",playersState::PlayerColor::GREEN,10,0,3,std::vector<std::shared_ptr<mapState::Road>>(),test_interact_hand);
    {
        ANN_START("already owned case")
        REQUIRE(road);
        road->setOwner(test_owner);
        playersState::Player player(test_init_player_name, test_init_player_color, test_init_player_score, test_init_player_nbWagons, test_init_player_nbStations, {}, test_interact_hand);
        CHECK_EQ(player.isRoadBuildable(test_interact_map, road), false);
        ANN_END("already owned case")
    }
    ANN_END("isRoadBuildable")
}
// TODO : complete tests (need mapstate methods) + restructure (testing ferries & roads => different map state)

TEST(getClaimableRoads)
{
    ANN_START("getClaimableRoads")
    auto map = std::make_shared<mapState::MapState>(mapState::MapState::Test());
    {
        ANN_START("basic_success_many_wagons")
        auto player = playersState::Player::Init("SimpleTest", PlayerColor::GREEN, nullptr);
        player.nbWagons = 45;
        auto claimable = player.getClaimableRoads(map);
        BOOST_TEST(!claimable.empty());
        ANN_END("basic_success_many_wagons")
    }
    {
        ANN_START("very_few_wagons")
        auto player = playersState::Player::Init("PoorPlayer", PlayerColor::BLUE, nullptr);
        player.nbWagons = 1;
        auto claimable = player.getClaimableRoads(map);
        BOOST_TEST(claimable.size() <= 9);
        ANN_END("very_few_wagons")
    }
    ANN_END("getClaimableRoads")
}

TEST(isDestinationReached)
{

    ANN_START("isDestinationReached")
    {

        ANN_START("nullptr_safety_checks")
        Player player("TestPlayer", PlayerColor::RED, 0, 45, 3, {}, nullptr);

        BOOST_TEST(player.isDestinationReached(nullptr, nullptr) == false);
        BOOST_TEST(player.isDestinationReached(std::make_shared<mapState::MapState>(), nullptr) == false);

        auto map = std::make_shared<mapState::MapState>();
        BOOST_TEST(player.isDestinationReached(map, nullptr) == false);
        ANN_END("nullptr_safety_checks")
    }
    {
        ANN_START("destination_with_null_stations")
        auto map = std::make_shared<mapState::MapState>();
        Player player("Alice", PlayerColor::BLUE, 0, 40, 3, {}, nullptr);

        auto badDest = std::make_shared<cardsState::DestinationCard>(nullptr, nullptr,0, 0);
        BOOST_TEST(player.isDestinationReached(map, badDest) == false);
        ANN_END("destination_with_null_stations")
    }
    {
        ANN_START("destination_not_reached")
        auto map = std::make_shared<mapState::MapState>();

        Player player("Bob", PlayerColor::GREEN, 0, 35, 4, {}, nullptr);
        auto dest = std::make_shared<cardsState::DestinationCard>(
        std::make_shared<mapState::Station>("Madrid", nullptr, nullptr),
        std::make_shared<mapState::Station>("Berlin", nullptr, nullptr),
            12, 20
        );

        BOOST_TEST(player.isDestinationReached(map, dest) == false);
        ANN_END("destination_not_reached")
    }
    {
              {
                  ANN_START("destination_reached")

                  auto map = std::make_shared<mapState::MapState>(mapState::MapState::Europe());

                  Player player("Bob", PlayerColor::GREEN, 0, 35, 4, {}, nullptr);

                  auto madrid = map->getStationByName("madrid");
                  auto berlin = map->getStationByName("berlin");

                  BOOST_REQUIRE(madrid != nullptr);
                  BOOST_REQUIRE(berlin != nullptr);

                  auto dest = std::make_shared<cardsState::DestinationCard>(madrid, berlin, 12, 20);

                  BOOST_TEST(player.isDestinationReached(map, dest) == false);

                  std::vector<std::string> path_stations = {
                      "madrid", "pamplona", "paris", "frankfurt", "berlin"
                  };

                  std::vector<std::shared_ptr<mapState::Road>> borrowed_roads;

                  for(size_t i = 0; i + 1 < path_stations.size(); ++i)
                  {
                      auto stationA = map->getStationByName(path_stations[i]);
                      auto stationB = map->getStationByName(path_stations[i+1]);

                      BOOST_REQUIRE(stationA && stationB);

                      auto roads_between = map->getRoadsBetweenStations(stationA, stationB);
                      BOOST_REQUIRE(!roads_between.empty());

                      borrowed_roads.push_back(roads_between[0]);
                  }

                  player.borrowedRoads = borrowed_roads;

                  BOOST_TEST(player.isDestinationReached(map, dest) == true);

                  if (!player.borrowedRoads.empty())
                  {
                      player.borrowedRoads.pop_back();
                      BOOST_TEST(player.isDestinationReached(map, dest) == false);
                  }

                  ANN_END("destination_reached")
              }
    }
}
TEST(display_player) {
    ANN_START("display") {

        ANN_START("default constructed player (invalid state)")
        {
            Player p;
            std::stringstream buffer;
            std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
            p.display(0);
            std::cout.rdbuf(old);
            std::string out = buffer.str();

            CHECK(out.find("##### PLAYER #####") != std::string::npos);
            CHECK(out.find("Name: ") != std::string::npos);
            CHECK(out.find("Color: UNKNOWN") != std::string::npos);
            CHECK(out.find("Score: -1") != std::string::npos);
            CHECK(out.find("NbWagons: -1") != std::string::npos);
            CHECK(out.find("NbStations: -1") != std::string::npos);
            CHECK(out.find("===== BORROWED ROADS =====") != std::string::npos);
            CHECK(out.find("\tno roads borrowed yet") != std::string::npos);
            CHECK(out.find("No hand available.") != std::string::npos);
            CHECK(out.find("#####################") != std::string::npos);
        }
        ANN_END("default constructed player (invalid state)")

        ANN_START("well initialized player - no roads")
        {
            std::shared_ptr<mapState::Station> test_stationA = test_interact_map->getStationByName(test_interract_stationA_name);
            std::shared_ptr<mapState::Station> test_stationB = test_interact_map->getStationByName(test_interract_stationB_name);
            auto dest = std::make_shared<cardsState::DestinationCard>(test_stationA,test_stationB, 12, false);
            auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::GREEN);
            std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
            std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCards = {wagon};
            auto hand = std::make_shared<cardsState::PlayerCards>(destCards, wagonCards);

            Player p("Sophie", PlayerColor::GREEN, 142, 38, 2, {}, hand);

            std::stringstream buffer;
            std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
            p.display(1);
            std::cout.rdbuf(old);
            std::string out = buffer.str();

            CHECK(out.find("\t##### PLAYER #####") != std::string::npos);
            CHECK(out.find("\tName: Sophie") != std::string::npos);
            CHECK(out.find("\tColor: GREEN") != std::string::npos);
            CHECK(out.find("\tScore: 142") != std::string::npos);
            CHECK(out.find("\tNbWagons: 38") != std::string::npos);
            CHECK(out.find("\tNbStations: 2") != std::string::npos);
            CHECK(out.find("\t===== BORROWED ROADS =====") != std::string::npos);
            CHECK(out.find("\t\tno roads borrowed yet") != std::string::npos);
            CHECK(out.find("\t#####################") != std::string::npos);
        }
        ANN_END("well initialized player - no roads")
    }
    ANN_END("display")
}



SUITE_END() // Interactions

SUITE_END() // Operations
/* vim: set sw=2 sts=2 et : */
