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
#include "GameView.h"
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
const char kGameName[] = "Ticket to Ride";
const char kGameVersion[] = "0.1";
// GameView gets roughly 2/3 of the remaining vertical space.
const int kMapHeightNumerator = 2;
const int kMapHeightDenominator = 3;
// InfoPanel keeps only the most recent messages.
const int kInfoPanelMaxMessages = 200;
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
// Escape sequence parsing (PgUp/PgDn are ESC [ 5/6 ~).
const char kEscape = 27;
const char kEscapeBracket = '[';
const char kEscapeTerminator = '~';
const char kPageUpCode = '5';
const char kPageDownCode = '6';
const char kArrowUpCode = 'A';
const char kArrowDownCode = 'B';
const char kArrowRightCode = 'C';
const char kArrowLeftCode = 'D';
const char kShiftTabCode = 'Z';
const int kMinScrollLines = 1;
const int kArrowScrollLines = 1;
// Disable line wrapping so drawing to the last column doesn't scroll the screen.
const char kDisableLineWrap[] = "\033[?7l";
const char kEnableLineWrap[] = "\033[?7h";
// Key codes for basic input handling.
const char kTab = '\t';
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

std::string extractAction(const std::string& json) {
  const std::string key = "\"action\":\"";
  const std::size_t start = json.find(key);
  if (start == std::string::npos) {
    return "";
  }
  const std::size_t value_start = start + key.size();
  const std::size_t end = json.find('"', value_start);
  if (end == std::string::npos || end <= value_start) {
    return "";
  }
  return json.substr(value_start, end - value_start);
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
      gameview(nullptr),
      infopanel(nullptr),
      commandinput(nullptr),
      running(false),
      debugRender(false),
      focus(Focus::COMMAND_FOCUS),
      mapstate(nullptr),
      playerstate(nullptr),
      cardstate(nullptr),
      ownsMapState(true),
      ownsPlayerState(true),
      ownsCardState(true),
      parser() {}
      
// Parametrized constructor to accept external game states
TUIManager::TUIManager(
    Terminal* terminal,
    int cols,
    int rows,
    mapState::MapState* mapState,
    playersState::PlayersState* playerState,
    cardsState::CardsState* cardState)
    : cols(cols),
      rows(rows),
      term(terminal),
      statusbar(nullptr),
      gameview(nullptr),
      infopanel(nullptr),
      commandinput(nullptr),
      running(false),
      debugRender(false),
      focus(Focus::COMMAND_FOCUS),
      mapstate(mapState),
      playerstate(playerState),
      cardstate(cardState),
      ownsMapState(mapState == nullptr),
      ownsPlayerState(playerState == nullptr),
      ownsCardState(cardState == nullptr),
      parser() {}

void TUIManager::init() {
  cols = clampPositive(cols, kMinDimension);
  rows = clampPositive(rows, kMinDimension);

  // Provide placeholder states for the TUI demo.
  if (!mapstate) {
    mapstate = new mapState::MapState();
    ownsMapState = true;
  }
  if (!playerstate) {
    playerstate = new playersState::PlayersState();
    ownsPlayerState = true;
  }
  if (!cardstate) {
    cardstate = new cardsState::CardsState();
    ownsCardState = true;
  }

  if (!statusbar) {
    statusbar = new StatusBar(kTerminalOrigin, kTerminalOrigin, cols, kStatusBarHeight);
    statusbar->setTitle("StatusBar");
    statusbar->setGameInfo(kGameName, kGameVersion);
    statusbar->setTurn(1);
    statusbar->setColors(Color::White, Color::Red);
  }
  if (!gameview) {
    gameview = new GameView(
        kTerminalOrigin,
        kTerminalOrigin,
        cols,
        std::max(kMinDimension, rows - kContentVerticalPadding));
    gameview->setTitle("GameView");
    gameview->setMapState(mapstate);
    gameview->setPlayerState(playerstate);
    gameview->setCardsState(cardstate);
    gameview->setHighlightActivePlayer(true);
  } else {
    gameview->setMapState(mapstate);
    gameview->setPlayerState(playerstate);
    gameview->setCardsState(cardstate);
  }
  if (gameview) {
    gameview->compactOtherPlayers = !debugRender;
    gameview->showLocalPlayerMarker = !debugRender;
  }
  if (statusbar) {
    statusbar->setDebugRender(debugRender);
  }
  if (!infopanel) {
    infopanel = new InfoPanel(
        kTerminalOrigin,
        kTerminalOrigin,
        cols,
        std::max(kMinDimension, rows - kContentVerticalPadding),
        kInfoPanelMaxMessages);
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
  setFocus(focus);
}

void TUIManager::setDebugRender(bool enabled) {
  debugRender = enabled;
  if (gameview) {
    gameview->compactOtherPlayers = !debugRender;
    gameview->showLocalPlayerMarker = !debugRender;
    gameview->requestRedraw();
  }
  if (statusbar) {
    statusbar->setDebugRender(debugRender);
  }
}

void TUIManager::updateLayout(int newCols, int newRows) {
  cols = clampPositive(newCols, kMinDimension);
  rows = clampPositive(newRows, kMinDimension);

  if (!statusbar || !gameview || !infopanel || !commandinput) {
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
  // GameView gets the larger share of the content height.
  int mapHeight = contentHeight * kMapHeightNumerator / kMapHeightDenominator;
  mapHeight = clampPositive(mapHeight, kMinDimension);
  int infoHeight = contentHeight - mapHeight;
  if (infoHeight < kMinDimension) {
    infoHeight = kMinDimension;
    mapHeight = clampPositive(contentHeight - infoHeight, kMinDimension);
  }

  gameview->setPosition(kTerminalOrigin, contentY);
  gameview->setSize(cols, mapHeight);

  infopanel->setPosition(kTerminalOrigin, contentY + mapHeight);
  infopanel->setSize(cols, infoHeight);

  const int commandY = contentY + contentHeight;
  commandinput->setPosition(kTerminalOrigin, commandY);
  commandinput->setSize(cols, commandHeight);

  statusbar->requestRedraw();
  gameview->requestRedraw();
  infopanel->requestRedraw();
  commandinput->requestRedraw();
}

void TUIManager::setFocus(Focus nextFocus) {
  if (focus == Focus::MAP_FOCUS && nextFocus != Focus::MAP_FOCUS) {
    if (gameview) {
      gameview->clearSearchQuery();
    }
  }
  focus = nextFocus;
  if (gameview) {
    gameview->setFocused(focus == Focus::MAP_FOCUS);
  }
  if (infopanel) {
    infopanel->setFocused(focus == Focus::INFO_FOCUS);
  }
  if (commandinput) {
    commandinput->setFocused(focus == Focus::COMMAND_FOCUS);
  }
}

void TUIManager::focusNext() {
  if (focus == Focus::COMMAND_FOCUS) {
    setFocus(Focus::MAP_FOCUS);
  } else if (focus == Focus::MAP_FOCUS) {
    setFocus(Focus::INFO_FOCUS);
  } else {
    setFocus(Focus::COMMAND_FOCUS);
  }
}

void TUIManager::focusPrev() {
  if (focus == Focus::COMMAND_FOCUS) {
    setFocus(Focus::INFO_FOCUS);
  } else if (focus == Focus::INFO_FOCUS) {
    setFocus(Focus::MAP_FOCUS);
  } else {
    setFocus(Focus::COMMAND_FOCUS);
  }
}

void TUIManager::drawAll() {
  if (!term) {
    return;
  }
  // Refresh views that requested redraw, then flush once.
  if (statusbar) {
    statusbar->refresh(*term);
  }
  if (gameview) {
    gameview->refresh(*term);
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
  commandinput->setInput(input);
  std::ostringstream line;
  line << "> " << input;
  infopanel->addMessage(line.str());

  ParseResult result = parser.parse(input);
  if (!result.ok) {
    if (!result.error.empty()) {
      infopanel->addMessage(result.error);
    }
  } else {
    const std::string action = extractAction(result.json);
    if (action == "exit") {
      infopanel->addMessage("Exiting...");
      running = false;
    } else if (action == "help") {
      infopanel->addMessage("Available commands: exit");
    } else {
      // JSON payload is ready for the engine when it's wired in.
    }
  }

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

  delete gameview;
  gameview = nullptr;

  delete infopanel;
  infopanel = nullptr;

  delete commandinput;
  commandinput = nullptr;

  if (ownsMapState) {
    delete mapstate;
  }
  mapstate = nullptr;

  if (ownsPlayerState) {
    delete playerstate;
  }
  playerstate = nullptr;

  if (ownsCardState) {
    delete cardstate;
  }
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
  term->write(kDisableLineWrap);

  while (running) {
    if (g_shouldStop != 0) {
      running = false;
      break;
    }

    if (commandinput != nullptr) {
      char ch = '\0';
      bool updated = false;
      while (pollChar(ch)) {
        if (ch == kTab) {
          focusNext();
          updated = true;
          continue;
        }

        if (ch == kEscape) {
          // Handle PageUp/PageDown escape sequences to scroll the info panel.
          char seq1 = '\0';
          if (!pollChar(seq1) || seq1 != kEscapeBracket) {
            continue;
          }
          char seq2 = '\0';
          if (!pollChar(seq2)) {
            continue;
          }
          if (seq2 == kShiftTabCode) {
            focusPrev();
            updated = true;
            continue;
          }
          if (seq2 == kPageUpCode || seq2 == kPageDownCode) {
            char seq3 = '\0';
            if (!pollChar(seq3) || seq3 != kEscapeTerminator) {
              continue;
            }
            if (infopanel != nullptr && focus == Focus::INFO_FOCUS) {
              int scrollAmount = infopanel->getVisibleLineCount();
              if (scrollAmount < kMinScrollLines) {
                scrollAmount = kMinScrollLines;
              }
              const int delta = (seq2 == kPageUpCode) ? scrollAmount : -scrollAmount;
              infopanel->scroll(delta);
              updated = true;
            }
            continue;
          }

          if (infopanel != nullptr && focus == Focus::INFO_FOCUS) {
            if (seq2 == kArrowUpCode) {
              infopanel->scroll(kArrowScrollLines);
              updated = true;
              continue;
            }
            if (seq2 == kArrowDownCode) {
              infopanel->scroll(-kArrowScrollLines);
              updated = true;
              continue;
            }
          }

          if (gameview != nullptr && focus == Focus::MAP_FOCUS &&
              (gameview->getMode() == ViewMode::MAP ||
               gameview->getMode() == ViewMode::PLAYER)) {
            if (seq2 == kArrowUpCode) {
              gameview->moveSelection(-1, 0);
              updated = true;
            } else if (seq2 == kArrowDownCode) {
              gameview->moveSelection(1, 0);
              updated = true;
            } else if (seq2 == kArrowLeftCode) {
              gameview->moveSelection(0, -1);
              updated = true;
            } else if (seq2 == kArrowRightCode) {
              gameview->moveSelection(0, 1);
              updated = true;
            }
          }
        } else if (focus == Focus::COMMAND_FOCUS) {
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
        } else if (focus == Focus::MAP_FOCUS) {
          if (ch == kNewline || ch == kCarriageReturn) {
            if (gameview != nullptr && gameview->getMode() == ViewMode::MAP) {
              gameview->clearSearchQuery();
              updated = true;
            }
          } else if (ch == kDelete || ch == kBackspace) {
            if (gameview != nullptr && gameview->getMode() == ViewMode::MAP) {
              gameview->backspaceSearch();
              updated = true;
            }
          } else if (ch == 'M') {
            if (gameview != nullptr) {
              gameview->setMode(ViewMode::MAP);
              updated = true;
            }
          } else if (ch == 'P') {
            if (gameview != nullptr) {
              gameview->setMode(ViewMode::PLAYER);
              updated = true;
            }
          } else if (ch == 'C') {
            if (debugRender && gameview != nullptr) {
              gameview->setMode(ViewMode::CARDS);
              updated = true;
            }
          } else if (std::isalnum(static_cast<unsigned char>(ch)) != 0) {
            if (gameview != nullptr && gameview->getMode() == ViewMode::MAP) {
              gameview->appendSearchChar(ch);
              updated = true;
            }
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
  term->write(kEnableLineWrap);
  term->resetStyles();
  term->clearRegion(kTerminalOrigin, kTerminalOrigin, cols, rows);
  term->moveTo(kTerminalOrigin, kTerminalOrigin);
  term->flush();

  // Restore terminal settings on exit.
  if (termiosReady) {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
  }
  std::signal(SIGINT, SIG_DFL);
}

}  // namespace tui
