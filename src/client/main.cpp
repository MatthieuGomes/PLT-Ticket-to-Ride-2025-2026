#include <iostream>

// The following lines are here to check that SFML is installed and working
#include <SFML/Graphics.hpp>
#include "client/Client.h"  
#include <cstring>
#include <memory>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "tui/Terminal.h"
#include "tui/TUIManager.h"
#include "mapState/MapState.h"
#include "playersState/PlayersState.h"
#include "cardsState/CardsState.h"

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
    if (strcmp(argv[1],"tui")==0) {
        configureCoverageOutput();
        int cols = kDefaultCols;
        int rows = kDefaultRows;
        getTerminalSize(cols, rows);

        tui::Terminal term;
        std::shared_ptr<mapState::MapState> map_state =
            std::make_shared<mapState::MapState>();
        std::shared_ptr<playersState::PlayersState> players_state =
            std::make_shared<playersState::PlayersState>();
        std::shared_ptr<cardsState::CardsState> cards_state =
            std::make_shared<cardsState::CardsState>();

        tui::TUIManager manager(&term, cols, rows,
                                map_state.get(),
                                players_state.get(),
                                cards_state.get());
        manager.runMainLoop();

        return EXIT_SUCCESS;
    }



    return EXIT_SUCCESS;

}
