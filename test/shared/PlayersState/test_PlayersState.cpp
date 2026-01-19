#include <boost/test/unit_test.hpp>
#include "../../src/shared/playersState/PlayersState.h"
#include <cstdlib>

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

using namespace playersState;
using namespace mapState;

using PlayersInfos = std::tuple<std::string, PlayerColor, int, int, int, std::vector<std::shared_ptr<mapState::Road>>, std::shared_ptr<cardsState::PlayerCards>>;
using PlayersInitInfos = std::tuple<std::string, PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

std::string test_init_player1_name = "Alice";
playersState::PlayerColor test_init_player1_color = playersState::PlayerColor::RED;
int test_init_player1_score = 23;
int test_init_player1_nbWagons = 21;
int test_init_player1_nbStations = 10;
int test_init_player1_nbRoads = 5;

std::string test_init_player2_name = "Bob";
playersState::PlayerColor test_init_player2_color = playersState::PlayerColor::BLUE;
int test_init_player2_score = 22;
int test_init_player2_nbWagons = 20;
int test_init_player2_nbStations = 9;
int test_init_player2_nbRoads = 6;

std::string test_stationA_name = "Paris";
std::string test_stationB_name = "Rome";
int test_points = 12;
std::vector<std::shared_ptr<mapState::Road>> test_borrowed_roads;
std::shared_ptr<playersState::Player> test_owner =
    std::make_shared<playersState::Player>("TestPlayer",
                                           playersState::PlayerColor::RED, 0, 45, 3, test_borrowed_roads , nullptr);

std::shared_ptr<boost::adjacency_list<>> graph =
    std::make_shared<boost::adjacency_list<>>();

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));

std::shared_ptr<Station> stationA =
    std::make_shared<Station>(test_stationA_name, test_owner, vertexA);

std::shared_ptr<Station> stationB =
    std::make_shared<Station>(test_stationB_name, test_owner, vertexB);

TEST(TestStaticAssert)
{
  CHECK(1);
}

SUITE_START(Constructors)

TEST(Default)
{
  ANN_START("Default Constructor")
  PlayersState ps;
  CHECK(ps.getPlayers().empty());
  CHECK_EQ(PlayersState::nbPlayers, 0);
  ANN_END("Default Constructor")
}
TEST(Basic)
{
  ANN_START("Basic Constructor")
  PlayersState ps = PlayersState({test_owner});
  CHECK_EQ(ps.getPlayers().size(), 1);
  CHECK_EQ(ps.getPlayers()[0], test_owner);
  ANN_END("Basic Constructor")
}

TEST(MaxPlayersAllowed)
{
  ANN_START("MaxPlayersAllowed Constructor")
  std::vector<std::shared_ptr<Player>> players;
  for (int i = 0; i < 5; ++i)
  {
    std::string name = std::string("Player") + std::to_string(i);
    players.push_back(std::make_shared<Player>(name,
                                               playersState::PlayerColor::RED,
                                               0,
                                               test_init_player1_nbWagons,
                                               test_init_player1_nbStations,
                                               test_borrowed_roads,
                                               nullptr));
  }
  PlayersState ps = PlayersState(players);
  CHECK_EQ(ps.getPlayers().size(), 5);
  ANN_END("MaxPlayersAllowed Constructor")
}

TEST(FromInfos)
{
  ANN_START("FromInfos Constructor")
  std::vector<PlayersInfos> infos = {
      std::make_tuple(test_init_player1_name, test_init_player1_color,
                      test_init_player1_score, test_init_player1_nbWagons,
                      test_init_player1_nbStations, test_borrowed_roads, nullptr),
      std::make_tuple(test_init_player2_name, test_init_player2_color,
                      test_init_player2_score, test_init_player2_nbWagons,
                      test_init_player2_nbStations, test_borrowed_roads, nullptr)};
  PlayersState ps = PlayersState(infos);
  CHECK_EQ(ps.getPlayers().size(), 2);
  CHECK_EQ(ps.getPlayers()[0]->getName(), test_init_player1_name);
  CHECK_EQ(ps.getPlayers()[1]->getName(), test_init_player2_name);
  ANN_END("FromInfos Constructor")
}

TEST(FromInitInfos)
{

  ANN_START("FromInitInfos Constructor")
  std::vector<PlayersInitInfos> infos = {
      std::make_tuple(test_init_player1_name, test_init_player1_color, nullptr),
      std::make_tuple(test_init_player2_name, test_init_player2_color, nullptr)};
  PlayersState ps = PlayersState::InitFromInfos(infos);
  CHECK_EQ(ps.getPlayers().size(), 2);
  CHECK_EQ(ps.getPlayers()[0]->getName(), test_init_player1_name);
  CHECK_EQ(ps.getPlayers()[1]->getName(), test_init_player2_name);
  ANN_END("FromInitInfos Constructor")
}
TEST(Constructor_with_infos_and_hands_sets_players_and_hands_correctly)
{
    ANN_START("Constructor with infos + hands")


    auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);
    auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

    std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
    std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

    auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
    auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

    std::vector<PlayersInitInfos> infos = {
        std::make_tuple("Alice", PlayerColor::RED,   hand1),
        std::make_tuple("Bob",   PlayerColor::BLUE,  hand2)
    };

    std::vector<std::shared_ptr<cardsState::PlayerCards>> hands = { hand1, hand2 };

    PlayersState ps(infos, hands);

    BOOST_CHECK_EQUAL(ps.getPlayers().size(), 2);
    BOOST_CHECK_EQUAL(PlayersState::nbPlayers, 2);

    auto players = ps.getPlayers();

    BOOST_CHECK_EQUAL(players[0]->getName(), "Alice");
    BOOST_CHECK_EQUAL(players[0]->getColor(), PlayerColor::RED);

    BOOST_CHECK_EQUAL(players[1]->getName(), "Bob");
    BOOST_CHECK_EQUAL(players[1]->getColor(), PlayerColor::BLUE);

    auto p1_hand = players[0]->getHand();
    auto p2_hand = players[1]->getHand();

    BOOST_CHECK(p1_hand != nullptr);
    BOOST_CHECK(p2_hand != nullptr);

    ANN_END("Constructor with infos + hands")

}
SUITE_END() // Constructors

SUITE_START(GettersAndSetters)

SUITE_START(Getters)

TEST(getPlayers)
{
  ANN_START("getPlayers")
  PlayersState ps;

  auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);
  auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

  std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

  auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
  auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

  auto p1 = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                     test_init_player1_score, test_init_player1_nbWagons,
                                     test_init_player1_nbStations, test_borrowed_roads, hand1);
  auto p2 = std::make_shared<Player>(test_init_player2_name, test_init_player2_color,
                                     test_init_player2_score, test_init_player2_nbWagons,
                                     test_init_player2_nbStations, test_borrowed_roads, hand2);

  std::vector<std::shared_ptr<Player>> v = {p1, p2};
  ps.setPlayers(v);

  auto result = ps.getPlayers();
  CHECK_EQ(result.size(), 2);
  CHECK(result[0] == p1);
  CHECK(result[1] == p2);
  ANN_END("getPlayers")
}

SUITE_END() // Getters

SUITE_START(Setters)

TEST(setPlayers)
{
  ANN_START("setPlayers")
  PlayersState ps;

  auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);
  auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

  std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

  auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
  auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

  auto p1 = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                     test_init_player1_score, test_init_player1_nbWagons,
                                     test_init_player1_nbStations, test_borrowed_roads, hand1);
  auto p2 = std::make_shared<Player>(test_init_player2_name, test_init_player2_color,
                                     test_init_player2_score, test_init_player2_nbWagons,
                                     test_init_player2_nbStations, test_borrowed_roads, hand2);

  ps.setPlayers({p1, p2});

  CHECK_EQ(ps.getPlayers().size(), 2);
  ANN_END("setPlayers")
}
TEST(setupPlayersHand)
{

    ANN_START("setupPlayersHand assigns different hands correctly")

    auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);

    auto wagonRed   = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);
    auto wagonBlack = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::BLACK);

    std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};

    std::vector<std::shared_ptr<cardsState::WagonCard>> wagonsAlice = {wagonRed};
    std::vector<std::shared_ptr<cardsState::WagonCard>> wagonsBob   = {wagonBlack};

    auto handAlice = std::make_shared<cardsState::PlayerCards>(destCards, wagonsAlice);
    auto handBob   = std::make_shared<cardsState::PlayerCards>(destCards, wagonsBob);

    std::vector<std::shared_ptr<cardsState::PlayerCards>> hands = { handAlice, handBob };

    auto alice = std::make_shared<Player>("Alice", playersState::PlayerColor::RED,   0, 45, 3, std::vector<std::shared_ptr<Road>>{}, handAlice);
    auto bob   = std::make_shared<Player>("Bob",   playersState::PlayerColor::BLUE,  0, 40, 3, std::vector<std::shared_ptr<Road>>{}, handBob);

    PlayersState ps({alice, bob});

    ps.setupPlayersHand(hands);

    auto aliceHandAfter = alice->getHand();
    auto bobHandAfter   = bob->getHand();

    BOOST_CHECK_EQUAL(aliceHandAfter, handAlice);
    BOOST_CHECK_EQUAL(bobHandAfter,   handBob);

    ANN_END("setupPlayersHand assigns different hands correctly")

}


SUITE_END() // Setters

SUITE_END() // GettersAndSetters

SUITE_START(Operations)

SUITE_START(Internal)

SUITE_START(Interactions)

// TODO : add tests for isRoadClaimable and getClaimableRoads
TEST(isRoadClaimable)
{

  ANN_START("isRoadClaimable")
    auto map = std::make_shared<MapState>();

  {
      ANN_START("Road_already_taken_by_other_player_should_return_false")
      auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);
      auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

      std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
      std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

      auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
      auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

      auto player1 = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                         test_init_player1_score, test_init_player1_nbWagons,
                                         test_init_player1_nbStations, test_borrowed_roads, hand1);

      auto player2 = std::make_shared<Player>(test_init_player2_name, test_init_player2_color,
                                           test_init_player2_score, test_init_player2_nbWagons,
                                           test_init_player2_nbStations, test_borrowed_roads, hand2);

      auto road = std::make_shared<Road>(1,nullptr, stationA,stationB, mapState::RoadColor::RED,4,nullptr);
      road->setOwner(player2);
      PlayersState ps({player1, player2});
      bool canClaim = ps.isRoadClaimable(map, road, player1);
      BOOST_CHECK_EQUAL(canClaim, false);
      ANN_END("Road_already_taken_by_other_player_should_return_false")
  }
    {

      ANN_START("Free_road_enough_wagons_should_return_true")
      auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);
      auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::BLACK);

      std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
      std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

      auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
      auto player = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                         test_init_player1_score, 21,
                                         test_init_player1_nbStations, test_borrowed_roads, hand1);

      auto road = std::make_shared<Road>(1,nullptr, stationA,stationB, mapState::RoadColor::BLACK,4,nullptr);
        PlayersState ps({player});

      bool canClaim = ps.isRoadClaimable(map, road, player);

      BOOST_CHECK_EQUAL(canClaim, true);
      ANN_END("Free_road_enough_wagons_should_return_true")
    }
    {
      ANN_START("Player_who_already_owns_the_road_cannot_claim_again")
      auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);
      auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::BLACK);

      std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
      std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

      auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
      auto player = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                         test_init_player1_score, 21,
                                         test_init_player1_nbStations, test_borrowed_roads, hand1);

      auto road = std::make_shared<Road>(1,nullptr, stationA,stationB, mapState::RoadColor::BLACK,4,nullptr);
      road->setOwner(player);
      PlayersState ps({player});
      bool canClaim = ps.isRoadClaimable(map, road, player);
      BOOST_CHECK_EQUAL(canClaim, false);
      ANN_END("Player_who_already_owns_the_road_cannot_claim_again")

  }
  ANN_END("isRoadClaimable")
}
TEST(getClaimableRoads)
{
  ANN_START("getClaimableRoads")
    auto map = std::make_shared<MapState>();
  {
    ANN_START("getClaimableRoads_delegates_to_map_correctly")
    auto player = std::make_shared<Player>(
        "Alice",PlayerColor::RED,0, 45, 3,std::vector<std::shared_ptr<Road>>{},nullptr);

    PlayersState ps({player});

    auto claimable_roads = ps.getClaimableRoads(map, player);
    BOOST_CHECK_NO_THROW(ps.getClaimableRoads(map, player));
    BOOST_CHECK(claimable_roads.size() >= 0);
    ANN_END("getClaimableRoads_delegates_to_map_correctly")
  }
  ANN_END("getClaimableRoads")
}

TEST(display){
    ANN_START("display"){
        ANN_START("empty case")
            PlayersState ps;
std::stringstream buffer;
std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
ps.display();
std::cout.rdbuf(old);
std::string out = buffer.str();

CHECK(out.find("~~~~~~ PLAYERS STATE ~~~~~~") != std::string::npos);
CHECK(out.find("No players in the game.") != std::string::npos);
ANN_END("empty case")
}
{

  ANN_START("non-empty case")
  PlayersState ps;

  auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12, false);
  auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

  std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

  auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
  auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

  auto p1 = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                     test_init_player1_score, test_init_player1_nbWagons,
                                     test_init_player1_nbStations, test_borrowed_roads, hand1);
  auto p2 = std::make_shared<Player>(test_init_player2_name, test_init_player2_color,
                                     test_init_player2_score, test_init_player2_nbWagons,
                                     test_init_player2_nbStations, test_borrowed_roads, hand2);

  ps.setPlayers({p1, p2});

  std::stringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

  ps.display();

  std::cout.rdbuf(old);

  std::string out = buffer.str();

  CHECK(out.find("~~~~~~ PLAYERS STATE ~~~~~~") != std::string::npos);
  CHECK(out.find("##### PLAYERS #####") != std::string::npos);
  CHECK(out.find("Alice") != std::string::npos);
  CHECK(out.find("Bob") != std::string::npos);
  ANN_END("non-empty case")
}
ANN_END("display")
}

SUITE_END() // Interactions

SUITE_END() // Internal
SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
