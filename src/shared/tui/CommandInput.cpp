#include "CommandInput.h"

#include <string>

#include "Terminal.h"

namespace tui {

namespace {

void writeClampedLine(Terminal& term, int row, int col, int width,
                      const std::string& text) {
  std::string buffer = text;
  if (width <= 0) {
    return;
  }
  if (static_cast<int>(buffer.size()) > width) {
    buffer.resize(static_cast<std::size_t>(width));
  }
  term.moveTo(row, col);
  term.write(buffer);
  const int remaining = width - static_cast<int>(buffer.size());
  if (remaining > 0) {
    term.writeRepeat(' ', remaining);
  }
}

}  // namespace

CommandInput::CommandInput(int x, int y, int width, int height)
    : BaseView(x, y, width, height), currentInput("") {
  requestRedraw();
}

const std::string& CommandInput::getInput() const {
  return currentInput;
}

void CommandInput::setInput(const std::string& text) {
  if (currentInput == text) {
    return;
  }
  currentInput = text;
  requestRedraw();
}

void CommandInput::clearInput() {
  if (currentInput.empty()) {
    return;
  }
  currentInput.clear();
  requestRedraw();
}

void CommandInput::parseCommand(std::string& out) {
  out = currentInput;
}

void CommandInput::drawContent(Terminal& term) {
  const int contentWidth = width - 2;
  const int contentHeight = height - 2;
  if (contentWidth <= 0 || contentHeight <= 0) {
    return;
  }

  term.setBg(bgColor);
  term.setFg(fgColor);

  std::string line = "> " + currentInput;
  writeClampedLine(term, y + 1, x + 1, contentWidth, line);

  int row = 1;
  while (row < contentHeight) {
    writeClampedLine(term, y + 1 + row, x + 1, contentWidth, "");
    row += 1;
  }

  const int promptLen = 2;
  int cursorCol = x + 1 + promptLen + static_cast<int>(currentInput.size());
  const int maxCol = x + 1 + contentWidth;
  if (cursorCol > maxCol) {
    cursorCol = maxCol;
  }
  term.moveTo(y + 1, cursorCol);
}

}  // namespace tui
