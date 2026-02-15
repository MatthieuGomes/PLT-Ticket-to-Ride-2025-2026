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
#include "engine/Engine.h"

#define ENABLE_INFOPANEL_LOG 1

void testSFML()
{
    sf::Texture texture;
}
// end of test SFML

namespace
{

    const int kDefaultCols = 100;
    const int kDefaultRows = 30;
    const bool kEnableGcovPrefix = true; // Set to false to disable gcov output redirection
    const char kGcovPrefixEnv[] = "GCOV_PREFIX";
    const char kGcovPrefixStripEnv[] = "GCOV_PREFIX_STRIP";
    const char kGcovPrefixDir[] = ".gcov";
    const int kGcovDirMode = 0755;

} // namespace

static void getTerminalSize(int &cols, int &rows)
{
    cols = kDefaultCols;
    rows = kDefaultRows;

    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
    {
        if (ws.ws_col > 0)
        {
            cols = static_cast<int>(ws.ws_col);
        }
        if (ws.ws_row > 0)
        {
            rows = static_cast<int>(ws.ws_row);
        }
    }
}

static void configureCoverageOutput()
{
    if (!kEnableGcovPrefix)
    {
        return;
    }
    const char *prefix = std::getenv(kGcovPrefixEnv);
    if (prefix != nullptr && prefix[0] != '\0')
    {
        return;
    }
    mkdir(kGcovPrefixDir, kGcovDirMode);
    setenv(kGcovPrefixEnv, kGcovPrefixDir, 0);
    setenv(kGcovPrefixStripEnv, "0", 0);
}

static bool fileExists(const char *path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return false;
    }
    struct stat sb;
    if (stat(path, &sb) != 0)
    {
        return false;
    }
    return S_ISREG(sb.st_mode);
}

static std::string buildInfoPanelLogPath()
{
    const char kLogsDir[] = "logs";
    const int kLogsDirMode = 0755;
    mkdir(kLogsDir, kLogsDirMode);

    std::time_t now = std::time(nullptr);
    std::tm localTime = *std::localtime(&now);
    char buffer[32];
    if (std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &localTime) == 0)
    {
        return "";
    }
    std::string path = std::string(kLogsDir) + "/game_session_infopanel_" + buffer + ".log";
    return path;
}

using namespace std;

int main(int argc, char *argv[])
{
    configureCoverageOutput();
    if (argc <= 1)
    {
        cout << "So far, nothing here..." << endl;
        cout << "exiting." << endl;
        return EXIT_FAILURE;
    }
    client::Client client = client::Client();
    if (strcmp(argv[1], "hello") == 0 || strcmp(argv[1], "state") == 0 || strcmp(argv[1], "render") == 0 || strcmp(argv[1], "tui") == 0)
    {
        std::cout << "Client started ..." << std::endl;
        if (strcmp(argv[1], "hello") == 0)
        {
            client.helloWorld();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[1], "state") == 0)
        {
            client.printState();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[1], "render") == 0)
        {
            if (argc < 3)
            {
                std::cerr << "Missing render option. Use --json or --custom." << std::endl;
                return EXIT_FAILURE;
            }
            if (argc >= 3 && (strcmp(argv[2], "--custom") == 0 || strcmp(argv[2], "--json") == 0) &&
                argc < 4)
            {
                std::cerr << "Missing argument for " << argv[2] << std::endl;
                return EXIT_FAILURE;
            }
            if (argc >= 4 && strcmp(argv[2], "--custom") == 0)
            {
                if (!fileExists(argv[3]))
                {
                    std::cerr << "Invalid json path for --custom: " << argv[3] << std::endl;
                    return EXIT_FAILURE;
                }
                setenv("RENDER_STATE_PATH", argv[3], 1);
                setenv("TUI_LAYOUT_PATH", argv[3], 1);
            }
            else if (argc >= 4 && strcmp(argv[2], "--json") == 0)
            {
                if (strcmp(argv[3], "1") == 0)
                {
                    setenv("RENDER_STATE_PATH", "static/europe_state_1.json", 1);
                    setenv("TUI_LAYOUT_PATH", "static/europe_state_1.json", 1);
                }
                else if (strcmp(argv[3], "2") == 0)
                {
                    setenv("RENDER_STATE_PATH", "static/europe_state_2.json", 1);
                    setenv("TUI_LAYOUT_PATH", "static/europe_state_2.json", 1);
                }
                else
                {
                    std::cerr << "Unknown json state loading option: " << argv[3] << std::endl;
                    return EXIT_FAILURE;
                }
            }
            else if (argc >= 3)
            {
                std::cerr << "Unknown render option: " << argv[2] << std::endl;
                return EXIT_FAILURE;
            }
            client.callRender();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[1], "tui") == 0)
        {
            // configureCoverageOutput();
            int cols = kDefaultCols;
            int rows = kDefaultRows;
            getTerminalSize(cols, rows);

            tui::Terminal term;
            std::shared_ptr<state::State> state(new state::State("static/europe_state.json"));
            playersState::PlayersState::nbPlayers = static_cast<int>(state->players.getPlayers().size());
            std::shared_ptr<engine::Engine> engine(new engine::Engine(state));

            tui::TUIManager manager(&term, cols, rows,
                                    &state->map,
                                    &state->players,
                                    &state->cards);
            manager.setEngine(engine);
#if ENABLE_INFOPANEL_LOG
            std::string logPath = buildInfoPanelLogPath();
            if (!logPath.empty())
            {
                setenv("INFOPANEL_LOG_PATH", logPath.c_str(), 1);
            }
#endif
            manager.runMainLoop();

            return EXIT_SUCCESS;
        }
    }
    else
    {
        cerr << "Unknown option: " << argv[1] << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
