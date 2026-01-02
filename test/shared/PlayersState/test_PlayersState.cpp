#include <boost/test/unit_test.hpp>
#include "../../src/shared/playersState/PlayersState.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

using namespace playersState;
using namespace mapState;

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

std::shared_ptr<playersState::Player> test_owner =
    std::make_shared<playersState::Player>("TestPlayer",
                                           playersState::PlayerColor::RED, 0, 45, 3, 5, nullptr);

bool test_is_blocked = false;

std::shared_ptr<boost::adjacency_list<>> graph =
    std::make_shared<boost::adjacency_list<>>();

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexA = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));

std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertexB = std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
    boost::add_vertex(*graph));

std::shared_ptr<Station> stationA =
    std::make_shared<Station>(test_stationA_name, test_owner, test_is_blocked, vertexA);

std::shared_ptr<Station> stationB =
    std::make_shared<Station>(test_stationB_name, test_owner, test_is_blocked, vertexB);

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE(Constructors)

BOOST_AUTO_TEST_CASE(Empty)
{
  PlayersState ps;
  BOOST_CHECK(ps.getPlayers().empty());
}
BOOST_AUTO_TEST_CASE(Default)
{
  PlayersState ps = PlayersState({test_owner});
  BOOST_CHECK_EQUAL(ps.getPlayers().size(), 1);
  BOOST_CHECK_EQUAL(ps.getPlayers()[0], test_owner);
}

BOOST_AUTO_TEST_SUITE_END() // Constructors

BOOST_AUTO_TEST_SUITE(GettersAndSetters)

BOOST_AUTO_TEST_SUITE(Getters)

BOOST_AUTO_TEST_CASE(GetPlayers)
{
  PlayersState ps;

  auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12);
  auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

  std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

  auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
  auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

  auto p1 = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                     test_init_player1_score, test_init_player1_nbWagons,
                                     test_init_player1_nbStations, test_init_player1_nbRoads, hand1);
  auto p2 = std::make_shared<Player>(test_init_player2_name, test_init_player2_color,
                                     test_init_player2_score, test_init_player2_nbWagons,
                                     test_init_player2_nbStations, test_init_player2_nbRoads, hand2);

  std::vector<std::shared_ptr<Player>> v = {p1, p2};
  ps.setPlayers(v);

  auto result = ps.getPlayers();
  BOOST_CHECK_EQUAL(result.size(), 2);
  BOOST_CHECK(result[0] == p1);
  BOOST_CHECK(result[1] == p2);
}

BOOST_AUTO_TEST_SUITE_END() // Getters

BOOST_AUTO_TEST_SUITE(Setters)

BOOST_AUTO_TEST_CASE(SetPlayers)
{
  PlayersState ps;

  auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12);
  auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

  std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
  std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

  auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
  auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

  auto p1 = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                     test_init_player1_score, test_init_player1_nbWagons,
                                     test_init_player1_nbStations, test_init_player1_nbRoads, hand1);
  auto p2 = std::make_shared<Player>(test_init_player2_name, test_init_player2_color,
                                     test_init_player2_score, test_init_player2_nbWagons,
                                     test_init_player2_nbStations, test_init_player2_nbRoads, hand2);

  ps.setPlayers({p1, p2});

  BOOST_CHECK_EQUAL(ps.getPlayers().size(), 2);
}

BOOST_AUTO_TEST_SUITE_END() // Setters

BOOST_AUTO_TEST_SUITE_END() // GettersAndSetters

BOOST_AUTO_TEST_SUITE(Operations)

BOOST_AUTO_TEST_SUITE(Internal)

BOOST_AUTO_TEST_SUITE(Interactions)

BOOST_AUTO_TEST_CASE(IsRoadClaimable)
{
}

BOOST_AUTO_TEST_CASE(Display)
{
  std::cout << "Display Test Started ..." << std::endl;
  {
    std::cout << "empty case test started ..." << std::endl;
    PlayersState ps;
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    ps.display();
    std::cout.rdbuf(old);
    std::string out = buffer.str();
    
    BOOST_CHECK(out.find("ÉTAT DES JOUEURS") != std::string::npos);
    BOOST_CHECK(out.find("Aucun joueur dans la partie.\n") != std::string::npos);
    std::cout << "empty case test finished !" << std::endl;
  }
  {
   
    std::cout << "non-empty case test started ..." << std::endl;
    PlayersState ps;

    auto dest = std::make_shared<cardsState::DestinationCard>(stationA, stationB, 12);
    auto wagon = std::make_shared<cardsState::WagonCard>(cardsState::ColorCard::RED);

    std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards = {dest};
    std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCardsVec = {wagon};

    auto hand1 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);
    auto hand2 = std::make_shared<cardsState::PlayerCards>(destCards, wagonCardsVec);

    auto p1 = std::make_shared<Player>(test_init_player1_name, test_init_player1_color,
                                       test_init_player1_score, test_init_player1_nbWagons,
                                       test_init_player1_nbStations, test_init_player1_nbRoads, hand1);
    auto p2 = std::make_shared<Player>(test_init_player2_name, test_init_player2_color,
                                       test_init_player2_score, test_init_player2_nbWagons,
                                       test_init_player2_nbStations, test_init_player2_nbRoads, hand2);

    ps.setPlayers({p1, p2});

    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());

    ps.display();

    std::cout.rdbuf(old);

    std::string out = buffer.str();

    BOOST_CHECK(out.find("ÉTAT DES JOUEURS") != std::string::npos);
    BOOST_CHECK(out.find("-----------------------------") != std::string::npos);
    BOOST_CHECK(out.find("Alice") != std::string::npos);
    BOOST_CHECK(out.find("Bob") != std::string::npos);
    std::cout << "non-empty case test finished !" << std::endl;
  }
  std::cout << "Display Test Finished !\n"
            << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // Interactions

BOOST_AUTO_TEST_SUITE_END() // Internal
BOOST_AUTO_TEST_SUITE_END() // Operations

/* vim: set sw=2 sts=2 et : */
