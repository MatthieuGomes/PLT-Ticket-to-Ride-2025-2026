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
    configureCoverageOutput();
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
        if (argc >= 4 && strcmp(argv[2], "--json") == 0) {
            setenv("RENDER_STATE_PATH", argv[3], 1);
            setenv("TUI_LAYOUT_PATH", argv[3], 1);
        } else if (argc >= 4 && strcmp(argv[2], "--state") == 0) {
            if (strcmp(argv[3], "1") == 0) {
                setenv("RENDER_STATE_PATH", "static/europe_state_1.json", 1);
                setenv("TUI_LAYOUT_PATH", "static/europe_state_1.json", 1);
            } else if (strcmp(argv[3], "2") == 0) {
                setenv("RENDER_STATE_PATH", "static/europe_state_2.json", 1);
                setenv("TUI_LAYOUT_PATH", "static/europe_state_2.json", 1);
            }
        }
        client.callRender();
        return EXIT_SUCCESS;
    }
    if (strcmp(argv[1],"tui")==0) {
        // configureCoverageOutput();
        int cols = kDefaultCols;
        int rows = kDefaultRows;
        getTerminalSize(cols, rows);

        tui::Terminal term;
        state::State state("static/europe_state.json");
        playersState::PlayersState::nbPlayers = static_cast<int>(state.players.getPlayers().size());

        tui::TUIManager manager(&term, cols, rows,
                                &state.map,
                                &state.players,
                                &state.cards);
        manager.runMainLoop();

        return EXIT_SUCCESS;
    }



    return EXIT_SUCCESS;

}
