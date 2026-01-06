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

int clampPositive(int value, int minimum) {
  return std::max(value, minimum);
}

volatile sig_atomic_t g_shouldStop = 0;

void handleSigInt(int) {
  g_shouldStop = 1;
}

bool pollChar(char& ch) {
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  int result = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);
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
  cols = clampPositive(cols, 1);
  rows = clampPositive(rows, 1);

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
    statusbar = new StatusBar(1, 1, cols, 3);
    statusbar->setTitle("StatusBar");
    statusbar->setGameInfo("Ticket to Ride", "0.1");
    statusbar->setTurn(1);
  }
  if (!mapview) {
    mapview = new MapView(1, 1, cols, std::max(1, rows - 6));
    mapview->setTitle("MapView");
    mapview->setMapState(mapstate);
    mapview->setHighlightActivePlayer(true);
  } else {
    mapview->setMapState(mapstate);
  }
  if (!infopanel) {
    infopanel = new InfoPanel(1, 1, cols / 3, std::max(1, rows - 6));
    infopanel->setTitle("InfoPanel");
    infopanel->addMessage("Welcome to Ticket to Ride!");
  }
  if (!commandinput) {
    commandinput = new CommandInput(1, rows - 2, cols, 3);
    commandinput->setTitle("CommandInput");
  }

  updateLayout(cols, rows);
}

void TUIManager::updateLayout(int newCols, int newRows) {
  cols = clampPositive(newCols, 1);
  rows = clampPositive(newRows, 1);

  if (!statusbar || !mapview || !infopanel || !commandinput) {
    return;
  }

  int statusHeight = std::min(3, rows);
  statusHeight = clampPositive(statusHeight, 1);

  int commandHeight = std::min(3, rows - statusHeight);
  commandHeight = clampPositive(commandHeight, 1);

  int contentHeight = rows - statusHeight - commandHeight;
  if (contentHeight < 1) {
    contentHeight = 1;
  }

  statusbar->setPosition(1, 1);
  statusbar->setSize(cols, statusHeight);

  const int contentY = 1 + statusHeight;
  int mapHeight = contentHeight * 2 / 3;
  mapHeight = clampPositive(mapHeight, 1);
  int infoHeight = contentHeight - mapHeight;
  if (infoHeight < 1) {
    infoHeight = 1;
    mapHeight = clampPositive(contentHeight - infoHeight, 1);
  }

  mapview->setPosition(1, contentY);
  mapview->setSize(cols, mapHeight);

  infopanel->setPosition(1, contentY + mapHeight);
  infopanel->setSize(cols, infoHeight);

  const int commandY = contentY + contentHeight;
  commandinput->setPosition(1, commandY);
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
    struct termios raw = originalTermios;
    raw.c_lflag &= static_cast<unsigned long>(~(ECHO | ICANON));
    raw.c_lflag |= ISIG;
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
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
        if (ch == '\n' || ch == '\r') {
          std::string input = commandinput->getInput();
          if (!input.empty()) {
            handleInput(input);
          }
          commandinput->clearInput();
          updated = true;
        } else if (ch == 127 || ch == '\b') {
          std::string input = commandinput->getInput();
          if (!input.empty()) {
            input.pop_back();
            commandinput->setInput(input);
            updated = true;
          }
        } else if (std::isprint(static_cast<unsigned char>(ch)) != 0) {
          std::string input = commandinput->getInput();
          if (static_cast<int>(input.size()) < 50) {
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
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  shutdown();
  term->resetStyles();
  term->flush();

  if (termiosReady) {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
  }
  std::signal(SIGINT, SIG_DFL);
}

}  // namespace tui
