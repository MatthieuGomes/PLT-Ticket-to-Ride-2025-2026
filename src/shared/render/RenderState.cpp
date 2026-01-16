#include "RenderState.h"

#include <cstdlib>
#include <ctime>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tui/TUIManager.h"

namespace {

const int kDefaultCols = 100;
const int kDefaultRows = 30;
const bool kEnableGcovPrefix = true;
const char kGcovPrefixEnv[] = "GCOV_PREFIX";
const char kGcovPrefixStripEnv[] = "GCOV_PREFIX_STRIP";
const char kGcovPrefixDir[] = ".gcov";
const int kGcovDirMode = 0755;
const int kMinPlayers = 1;

void getTerminalSize(int& cols, int& rows) {
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

void configureCoverageOutput() {
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

}  // namespace

namespace render
{

  int RenderState::renderStateLoop(int playerCount)
  {
    configureCoverageOutput();
    int cols = kDefaultCols;
    int rows = kDefaultRows;
    getTerminalSize(cols, rows);

    if (playerCount < kMinPlayers) {
      playerCount = kMinPlayers;
    }
    this->playerCount = playerCount;

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

}
