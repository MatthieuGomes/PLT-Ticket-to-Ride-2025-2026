#include "playersState/Player.h"
#include "StatusBar.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "Terminal.h"

namespace tui {

namespace {

// Frame geometry: 2 cells for borders and content starts 1 cell in.
const int kFrameInset = 2;
const int kFrameOffset = 1;

}  // namespace

StatusBar::StatusBar(int x, int y, int width, int height)
    : BaseView(x, y, width, height) {
  gameName.clear();
  version.clear();
  currentTurn = 0;
  currentPlayer = nullptr;
  debugRender = false;
  requestRedraw();
}

void StatusBar::setGameInfo(const std::string& name, const std::string& ver) {
  if (gameName == name && version == ver) {
    return;
  }
  gameName = name;
  version = ver;
  requestRedraw();
}

void StatusBar::setTurn(int turn) {
  if (currentTurn == turn) {
    return;
  }
  currentTurn = turn;
  requestRedraw();
}

void StatusBar::setCurrentPlayer(playersState::Player* player) {
  if (currentPlayer == player) {
    return;
  }
  currentPlayer = player;
  requestRedraw();
}

void StatusBar::setDebugRender(bool enabled) {
  if (debugRender == enabled) {
    return;
  }
  debugRender = enabled;
  requestRedraw();
}

void StatusBar::drawFrame(Terminal& term) {
  BaseView::drawFrame(term);
}

void StatusBar::drawHeader(Terminal& term) {
  if (width <= 0 || height <= 0) {
    return;
  }

  term.setBg(bgColor);
  term.setFg(fgColor);

  // Compose a single-line status string: game/version, turn, and player name.
  std::ostringstream statusLine;
  if (!gameName.empty()) {
    statusLine << gameName;
    if (!version.empty()) {
      statusLine << " v" << version;
    }
  } else if (!version.empty()) {
    statusLine << "v" << version;
  }

  if (debugRender) {
    if (statusLine.tellp() > 0) {
      statusLine << " | ";
    }
    statusLine << "RENDER DEBUG";
  } else {
    if (currentTurn > 0) {
      if (statusLine.tellp() > 0) {
        statusLine << " | ";
      }
      statusLine << "Turn " << currentTurn;
      statusLine << " | Your turn";
    }

    if (currentPlayer != nullptr) {
      if (statusLine.tellp() > 0) {
        statusLine << " - ";
      }
      statusLine << currentPlayer->getName();
    }
  }

  std::string text = statusLine.str();
  const int available = std::max(0, width - kFrameInset);
  if (static_cast<int>(text.size()) > available) {
    text.resize(available);
  }

  // Keep text in the middle row for a compact bar.
  const int textRow = y + height / 2;
  const int textCol = x + kFrameOffset;
  term.moveTo(textRow, textCol);
  term.write(text);
  if (static_cast<int>(text.size()) < available) {
    term.writeRepeat(' ', available - static_cast<int>(text.size()));
  }
}

}  // namespace tui
