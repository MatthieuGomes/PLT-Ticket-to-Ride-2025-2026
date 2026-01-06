#include <iostream>

#include "playersState/Player.h"
#include "TUIManager.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <thread>

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

  init();
  running = true;
  term->hideCursor();

  while (running) {
    drawAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  term->showCursor();
}

}  // namespace tui
