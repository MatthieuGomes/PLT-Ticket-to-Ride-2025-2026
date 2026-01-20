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
#include "engine/Engine.h"

namespace {

const int kDefaultCols = 100;
const int kDefaultRows = 30;
const bool kEnableGcovPrefix = true;
const char kGcovPrefixEnv[] = "GCOV_PREFIX";
const char kGcovPrefixStripEnv[] = "GCOV_PREFIX_STRIP";
const char kGcovPrefixDir[] = ".gcov";
const int kGcovDirMode = 0755;
const int kMinPlayers = 1;
const char kDefaultStatePath[] = "static/europe_state.json";
const char kRenderStatePathEnv[] = "RENDER_STATE_PATH";

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

    const char* pathOverride = std::getenv(kRenderStatePathEnv);
    std::string statePath = kDefaultStatePath;
    if (pathOverride != nullptr && pathOverride[0] != '\0') {
      statePath = pathOverride;
    }

    std::shared_ptr<state::State> debugState(new state::State(statePath));
    playersState::PlayersState::nbPlayers =
        static_cast<int>(debugState->players.getPlayers().size());
    std::shared_ptr<engine::Engine> engine(new engine::Engine(debugState));

    tui::Terminal term;
    tui::TUIManager manager(&term, cols, rows,
                            &debugState->map,
                            &debugState->players,
                            &debugState->cards);
    manager.setDebugRender(true);
    manager.setEngine(engine);
    manager.runMainLoop();
    return EXIT_SUCCESS;
  }

}
