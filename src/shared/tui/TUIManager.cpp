#include <algorithm>
#include <chrono>
#include <csignal>
#include <cctype>
#include <sstream>
#include <thread>

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "playersState/Player.h"
#include "TUIManager.h"

#include "CommandInput.h"
#include "InfoPanel.h"
#include "MapView.h"
#include "StatusBar.h"
#include "Terminal.h"
#include "mapState/MapState.h"
#include "playersState/PlayersState.h"
#include "cardsState/CardsState.h"

namespace tui {

namespace {

// Layout sizing (rows/cols are 1-based in ANSI terminals).
const int kMinDimension = 1;
const int kTerminalOrigin = 1;
const int kStatusBarHeight = 3;
const int kCommandInputHeight = 3;
const int kContentVerticalPadding = kStatusBarHeight + kCommandInputHeight;
// MapView gets roughly 2/3 of the remaining vertical space.
const int kMapHeightNumerator = 2;
const int kMapHeightDenominator = 3;
// Keep input short to avoid overwriting the frame borders.
const int kMaxInputLength = 50;
// TUI refresh throttle to avoid busy looping.
const int kLoopSleepMs = 200;
// select() helper constants.
const int kSelectFdOffset = 1;
const int kSelectTimeoutSec = 0;
const int kSelectTimeoutUsec = 0;
// Termios settings for non-blocking character input.
const int kTermiosVmin = 0;
const int kTermiosVtime = 0;
// Key codes for basic input handling.
const char kNewline = '\n';
const char kCarriageReturn = '\r';
const char kBackspace = '\b';
const char kDelete = 127;

int clampPositive(int value, int minimum) {
  return std::max(value, minimum);
}

volatile sig_atomic_t g_shouldStop = 0;

void handleSigInt(int) {
  g_shouldStop = 1;
}

bool pollChar(char& ch) {
  // Non-blocking single character read from stdin.
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  struct timeval timeout;
  timeout.tv_sec = kSelectTimeoutSec;
  timeout.tv_usec = kSelectTimeoutUsec;

  int result = select(STDIN_FILENO + kSelectFdOffset, &readfds, nullptr, nullptr, &timeout);
  if (result <= 0 || !FD_ISSET(STDIN_FILENO, &readfds)) {
    return false;
  }

  char buffer = '\0';
  ssize_t bytes = read(STDIN_FILENO, &buffer, 1);
  if (bytes != 1) {
    return false;
  }
  ch = buffer;
  return true;
}

}  // namespace

TUIManager::TUIManager(Terminal* terminal, int cols, int rows)
    : cols(cols),
      rows(rows),
      term(terminal),
      statusbar(nullptr),
      mapview(nullptr),
      infopanel(nullptr),
      commandinput(nullptr),
      running(false),
      mapstate(nullptr),
      playerstate(nullptr),
      cardstate(nullptr) {}

void TUIManager::init() {
  cols = clampPositive(cols, kMinDimension);
  rows = clampPositive(rows, kMinDimension);

  // Provide placeholder states for the TUI demo.
  if (!mapstate) {
    mapstate = new mapState::MapState();
  }
  if (!playerstate) {
    playerstate = new playersState::PlayersState();
  }
  if (!cardstate) {
    cardstate = new cardsState::CardsState();
  }

  if (!statusbar) {
    statusbar = new StatusBar(kTerminalOrigin, kTerminalOrigin, cols, kStatusBarHeight);
    statusbar->setTitle("StatusBar");
    statusbar->setGameInfo("Ticket to Ride", "0.1");
    statusbar->setTurn(1);
  }
  if (!mapview) {
    mapview = new MapView(
        kTerminalOrigin,
        kTerminalOrigin,
        cols,
        std::max(kMinDimension, rows - kContentVerticalPadding));
    mapview->setTitle("MapView");
    mapview->setMapState(mapstate);
    mapview->setHighlightActivePlayer(true);
  } else {
    mapview->setMapState(mapstate);
  }
  if (!infopanel) {
    infopanel = new InfoPanel(
        kTerminalOrigin,
        kTerminalOrigin,
        cols,
        std::max(kMinDimension, rows - kContentVerticalPadding));
    infopanel->setTitle("InfoPanel");
    infopanel->addMessage("Welcome to Ticket to Ride!");
  }
  if (!commandinput) {
    commandinput = new CommandInput(
        kTerminalOrigin,
        rows - kCommandInputHeight + kTerminalOrigin,
        cols,
        kCommandInputHeight);
    commandinput->setTitle("CommandInput");
  }

  updateLayout(cols, rows);
}

void TUIManager::updateLayout(int newCols, int newRows) {
  cols = clampPositive(newCols, kMinDimension);
  rows = clampPositive(newRows, kMinDimension);

  if (!statusbar || !mapview || !infopanel || !commandinput) {
    return;
  }

  // Split the screen vertically: status bar, content area, command input.
  int statusHeight = std::min(kStatusBarHeight, rows);
  statusHeight = clampPositive(statusHeight, kMinDimension);

  int commandHeight = std::min(kCommandInputHeight, rows - statusHeight);
  commandHeight = clampPositive(commandHeight, kMinDimension);

  int contentHeight = rows - statusHeight - commandHeight;
  if (contentHeight < kMinDimension) {
    contentHeight = kMinDimension;
  }

  statusbar->setPosition(kTerminalOrigin, kTerminalOrigin);
  statusbar->setSize(cols, statusHeight);

  const int contentY = kTerminalOrigin + statusHeight;
  // MapView gets the larger share of the content height.
  int mapHeight = contentHeight * kMapHeightNumerator / kMapHeightDenominator;
  mapHeight = clampPositive(mapHeight, kMinDimension);
  int infoHeight = contentHeight - mapHeight;
  if (infoHeight < kMinDimension) {
    infoHeight = kMinDimension;
    mapHeight = clampPositive(contentHeight - infoHeight, kMinDimension);
  }

  mapview->setPosition(kTerminalOrigin, contentY);
  mapview->setSize(cols, mapHeight);

  infopanel->setPosition(kTerminalOrigin, contentY + mapHeight);
  infopanel->setSize(cols, infoHeight);

  const int commandY = contentY + contentHeight;
  commandinput->setPosition(kTerminalOrigin, commandY);
  commandinput->setSize(cols, commandHeight);

  statusbar->requestRedraw();
  mapview->requestRedraw();
  infopanel->requestRedraw();
  commandinput->requestRedraw();
}

void TUIManager::drawAll() {
  if (!term) {
    return;
  }
  // Refresh views that requested redraw, then flush once.
  if (statusbar) {
    statusbar->refresh(*term);
  }
  if (mapview) {
    mapview->refresh(*term);
  }
  if (infopanel) {
    infopanel->refresh(*term);
  }
  if (commandinput) {
    commandinput->refresh(*term);
  }
  term->flush();
}

void TUIManager::handleInput(const std::string& input) {
  if (!infopanel || !commandinput) {
    return;
  }
  // For now, echo input into the info panel and clear the prompt.
  commandinput->setInput(input);
  std::ostringstream line;
  line << "> " << input;
  infopanel->addMessage(line.str());
  commandinput->clearInput();
  infopanel->requestRedraw();
  commandinput->requestRedraw();
}

void TUIManager::tick() {
  drawAll();
}

void TUIManager::shutdown() {
  running = false;
  if (term) {
    term->showCursor();
  }

  delete statusbar;
  statusbar = nullptr;

  delete mapview;
  mapview = nullptr;

  delete infopanel;
  infopanel = nullptr;

  delete commandinput;
  commandinput = nullptr;

  delete mapstate;
  mapstate = nullptr;

  delete playerstate;
  playerstate = nullptr;

  delete cardstate;
  cardstate = nullptr;
}

void TUIManager::runMainLoop() {
  if (!term) {
    return;
  }

  struct termios originalTermios;
  bool termiosReady = false;
  if (tcgetattr(STDIN_FILENO, &originalTermios) == 0) {
    // Switch to raw-ish mode: no echo and no line buffering.
    struct termios raw = originalTermios;
    raw.c_lflag &= static_cast<unsigned long>(~(ECHO | ICANON));
    raw.c_lflag |= ISIG;
    raw.c_cc[VMIN] = kTermiosVmin;
    raw.c_cc[VTIME] = kTermiosVtime;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
      termiosReady = true;
    }
  }

  std::signal(SIGINT, handleSigInt);

  init();
  running = true;
  term->hideCursor();

  while (running) {
    if (g_shouldStop != 0) {
      running = false;
      break;
    }

    if (commandinput != nullptr) {
      char ch = '\0';
      bool updated = false;
      while (pollChar(ch)) {
        if (ch == kNewline || ch == kCarriageReturn) {
          // Submit current input line.
          std::string input = commandinput->getInput();
          if (!input.empty()) {
            handleInput(input);
          }
          commandinput->clearInput();
          updated = true;
        } else if (ch == kDelete || ch == kBackspace) {
          // Backspace support.
          std::string input = commandinput->getInput();
          if (!input.empty()) {
            input.pop_back();
            commandinput->setInput(input);
            updated = true;
          }
        } else if (std::isprint(static_cast<unsigned char>(ch)) != 0) {
          // Accept printable characters up to the input limit.
          std::string input = commandinput->getInput();
          if (static_cast<int>(input.size()) < kMaxInputLength) {
            input.push_back(ch);
            commandinput->setInput(input);
            updated = true;
          }
        }
      }
      if (updated) {
        commandinput->requestRedraw();
      }
    }

    drawAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(kLoopSleepMs));
  }

  shutdown();
  term->resetStyles();
  term->flush();

  // Restore terminal settings on exit.
  if (termiosReady) {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
  }
  std::signal(SIGINT, SIG_DFL);
}

}  // namespace tui
