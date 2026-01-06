#include "playersState/Player.h"
#include "StatusBar.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "Terminal.h"

namespace tui {

StatusBar::StatusBar(int x, int y, int width, int height)
    : BaseView(x, y, width, height) {
  gameName.clear();
  version.clear();
  currentTurn = 0;
  currentPlayer = nullptr;
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

void StatusBar::drawFrame(Terminal& term) {
  if (width <= 0 || height <= 0) {
    return;
  }
  term.setBg(bgColor);
  term.setFg(fgColor);
  for (int row = 0; row < height; ++row) {
    term.moveTo(y + row, x);
    term.writeRepeat(' ', width);
  }
}

void StatusBar::drawHeader(Terminal& term) {
  if (width <= 0 || height <= 0) {
    return;
  }

  term.setBg(bgColor);
  term.setFg(fgColor);

  std::ostringstream statusLine;
  if (!gameName.empty()) {
    statusLine << gameName;
    if (!version.empty()) {
      statusLine << " v" << version;
    }
  } else if (!version.empty()) {
    statusLine << "v" << version;
  }

  if (currentTurn > 0) {
    if (statusLine.tellp() > 0) {
      statusLine << " | ";
    }
    statusLine << "Turn " << currentTurn;
  }

  if (currentPlayer != nullptr) {
    if (statusLine.tellp() > 0) {
      statusLine << " - ";
    }
    statusLine << currentPlayer->getName();
  }

  std::string text = statusLine.str();
  const int available = std::max(0, width - 2);
  if (static_cast<int>(text.size()) > available) {
    text.resize(available);
  }

  const int textRow = y + height / 2;
  const int textCol = x + 1;
  term.moveTo(textRow, textCol);
  term.write(text);
  if (static_cast<int>(text.size()) < available) {
    term.writeRepeat(' ', available - static_cast<int>(text.size()));
  }
}

}  // namespace tui
