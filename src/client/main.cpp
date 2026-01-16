#include <iostream>

// The following lines are here to check that SFML is installed and working
#include <SFML/Graphics.hpp>
#include "client/Client.h"  
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "tui/Terminal.h"
#include "tui/TUIManager.h"
#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "mapState/Station.h"
#include "playersState/PlayersState.h"
#include "cardsState/CardsState.h"
#include "cardsState/Deck.h"
#include "cardsState/DestinationCard.h"
#include "state/State.h"

void testSFML() {
    sf::Texture texture;
}
// end of test SFML

namespace {

const int kDefaultCols = 100;
const int kDefaultRows = 30;
const bool kEnableGcovPrefix = true; // Set to false to disable gcov output redirection
const char kGcovPrefixEnv[] = "GCOV_PREFIX";
const char kGcovPrefixStripEnv[] = "GCOV_PREFIX_STRIP";
const char kGcovPrefixDir[] = ".gcov";
const int kGcovDirMode = 0755;

}  // namespace

static void getTerminalSize(int& cols, int& rows) {
    cols = kDefaultCols;
    rows = kDefaultRows;

    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_col > 0) {
            cols = static_cast<int>(ws.ws_col);
        }
        if (ws.ws_row > 0) {
            rows = static_cast<int>(ws.ws_row);
        }
    }
}

static void configureCoverageOutput() {
    if (!kEnableGcovPrefix) {
        return;
    }
    const char* prefix = std::getenv(kGcovPrefixEnv);
    if (prefix != nullptr && prefix[0] != '\0') {
        return;
    }
    mkdir(kGcovPrefixDir, kGcovDirMode);
    setenv(kGcovPrefixEnv, kGcovPrefixDir, 0);
    setenv(kGcovPrefixStripEnv, "0", 0);
}

using namespace std;

int main(int argc,char* argv[])
{
    cout << "Client application started." << endl;
    if (argc<=1) {
        cout << "So far, nothing here..." << endl;
        cout << "Testing map" << endl;
        return EXIT_FAILURE;
    }
    client::Client client = client::Client();
    if (strcmp(argv[1],"hello")==0) {
        client.helloWorld();
        return EXIT_SUCCESS;
    }
    if (strcmp(argv[1],"state")==0) {
        client.printState();
        return EXIT_SUCCESS;
    }
    if (strcmp(argv[1],"render")==0) {
        configureCoverageOutput();
        int cols = kDefaultCols;
        int rows = kDefaultRows;
        getTerminalSize(cols, rows);

        const int playerCount = 6;
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        std::vector<playersState::PlayerColor> colors;
        colors.push_back(playersState::PlayerColor::RED);
        colors.push_back(playersState::PlayerColor::BLUE);
        colors.push_back(playersState::PlayerColor::GREEN);
        colors.push_back(playersState::PlayerColor::BLACK);
        colors.push_back(playersState::PlayerColor::YELLOW);

        for (int i = static_cast<int>(colors.size()) - 1; i > 0; --i) {
            int j = std::rand() % (i + 1);
            playersState::PlayerColor tmp = colors[static_cast<std::size_t>(i)];
            colors[static_cast<std::size_t>(i)] = colors[static_cast<std::size_t>(j)];
            colors[static_cast<std::size_t>(j)] = tmp;
        }

        std::vector<std::tuple<std::string, playersState::PlayerColor,
                               std::shared_ptr<cardsState::PlayerCards>>> playersInfos;
        for (int i = 0; i < playerCount; ++i) {
            std::string name = std::string("Player ") + std::to_string(i + 1);
            playersInfos.push_back(std::make_tuple(
                name,
                colors[static_cast<std::size_t>(i % static_cast<int>(colors.size()))],
                std::shared_ptr<cardsState::PlayerCards>()));
        }

        state::State debugState;
        debugState.map = mapState::MapState::Europe();
        debugState.cards = cardsState::CardsState::Europe(debugState.map.getStations(), playerCount);
        debugState.players = playersState::PlayersState(playersInfos, debugState.cards.playersCards);

        tui::Terminal term;
        tui::TUIManager manager(&term, cols, rows,
                                &debugState.map,
                                &debugState.players,
                                &debugState.cards);
        manager.setDebugRender(true);
        manager.runMainLoop();
        return EXIT_SUCCESS;
    }
    if (strcmp(argv[1],"tui")==0) {
        configureCoverageOutput();
        int cols = kDefaultCols;
        int rows = kDefaultRows;
        getTerminalSize(cols, rows);

        tui::Terminal term;
        std::shared_ptr<mapState::MapState> map_state =
            std::make_shared<mapState::MapState>(mapState::MapState::Europe());

        const int playerCount = 6;
        playersState::PlayersState::nbPlayers = playerCount;
        std::shared_ptr<cardsState::CardsState> cards_state =
            std::make_shared<cardsState::CardsState>(
                cardsState::CardsState::Europe(map_state->getStations(),
                                               playersState::PlayersState::nbPlayers));

        std::vector<playersState::PlayerColor> colors;
        colors.push_back(playersState::PlayerColor::RED);
        colors.push_back(playersState::PlayerColor::BLUE);
        colors.push_back(playersState::PlayerColor::GREEN);
        colors.push_back(playersState::PlayerColor::BLACK);
        colors.push_back(playersState::PlayerColor::YELLOW);

        std::vector<std::tuple<std::string, playersState::PlayerColor,
                               std::shared_ptr<cardsState::PlayerCards>>> playersInfos;
        for (int i = 0; i < playerCount; ++i) {
            std::string name = std::string("Player ") + std::to_string(i + 1);
            std::shared_ptr<cardsState::PlayerCards> hand;
            if (i < static_cast<int>(cards_state->playersCards.size())) {
                hand = cards_state->playersCards[static_cast<std::size_t>(i)];
            } else {
                hand = std::make_shared<cardsState::PlayerCards>();
            }
            playersInfos.push_back(std::make_tuple(
                name,
                colors[static_cast<std::size_t>(i % static_cast<int>(colors.size()))],
                hand));
        }

        std::shared_ptr<playersState::PlayersState> players_state =
            std::make_shared<playersState::PlayersState>(playersInfos,
                                                         cards_state->playersCards);

        std::shared_ptr<playersState::Player> player;
        if (players_state && !players_state->players.empty()) {
            player = players_state->players[0];
        }
        std::shared_ptr<cardsState::PlayerCards> player_hand;
        if (player) {
            player_hand = player->getHand();
        } else {
            player_hand = std::make_shared<cardsState::PlayerCards>();
        }

        std::vector<std::shared_ptr<mapState::Station>> stations = map_state->getStations();
        std::shared_ptr<mapState::Station> lisboaStation =
            mapState::Station::getStationByName(stations, "lisboa");
        std::shared_ptr<mapState::Station> danzigStation =
            mapState::Station::getStationByName(stations, "danzig");
        std::vector<std::shared_ptr<cardsState::DestinationCard>> destinationCards;
        if (lisboaStation && danzigStation) {
            std::shared_ptr<cardsState::DestinationCard> lisboaDanzig =
                std::make_shared<cardsState::DestinationCard>(lisboaStation, danzigStation, 20, true);
            destinationCards.push_back(lisboaDanzig);
        }
        player_hand->destinationCards =
            std::make_shared<cardsState::Deck<cardsState::DestinationCard>>(destinationCards);

        if (!destinationCards.empty()) {
            if (player) {
                player->completedDestinations.push_back(destinationCards[0]);
            }
        }

        // Claim a Lisbon -> Danzig path for demo purposes.
        std::vector<std::pair<std::string, std::string>> ownedPairs;
        ownedPairs.push_back(std::make_pair("lisboa", "madrid"));
        ownedPairs.push_back(std::make_pair("madrid", "pamplona"));
        ownedPairs.push_back(std::make_pair("pamplona", "paris"));
        ownedPairs.push_back(std::make_pair("paris", "frankfurt"));
        ownedPairs.push_back(std::make_pair("frankfurt", "berlin"));
        ownedPairs.push_back(std::make_pair("berlin", "danzig"));

        std::vector<std::shared_ptr<mapState::Road>> roads = map_state->getRoads();
        std::vector<bool> claimedPairs(ownedPairs.size(), false);
        for (std::size_t i = 0; i < roads.size(); ++i) {
            std::shared_ptr<mapState::Road> road = roads[i];
            if (!road || !road->getStationA() || !road->getStationB()) {
                continue;
            }
            const std::string a = road->getStationA()->getName();
            const std::string b = road->getStationB()->getName();
            for (std::size_t j = 0; j < ownedPairs.size(); ++j) {
                if (claimedPairs[j]) {
                    continue;
                }
                const std::string& first = ownedPairs[j].first;
                const std::string& second = ownedPairs[j].second;
                if ((a == first && b == second) || (a == second && b == first)) {
                    if (player) {
                        road->setOwner(player);
                    }
                    claimedPairs[j] = true;
                    break;
                }
            }
        }

        tui::TUIManager manager(&term, cols, rows,
                                map_state.get(),
                                players_state.get(),
                                cards_state.get());
        manager.runMainLoop();

        return EXIT_SUCCESS;
    }



    return EXIT_SUCCESS;

}
