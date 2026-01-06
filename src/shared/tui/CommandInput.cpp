#include "CommandInput.h"

#include <string>

#include "Terminal.h"

namespace tui {

namespace {

// Frame geometry: 2 cells for borders and content starts 1 cell in.
const int kFrameInset = 2;
const int kFrameOffset = 1;
// Prompt prefix shown before user input.
const char kPrompt[] = "> ";
const int kPromptLength = static_cast<int>(sizeof(kPrompt) - 1);

void writeClampedLine(Terminal& term, int row, int col, int width,
                      const std::string& text) {
  // Clamp text to the available width, then pad with spaces to clear leftovers.
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
  const int contentWidth = width - kFrameInset;
  const int contentHeight = height - kFrameInset;
  if (contentWidth <= 0 || contentHeight <= 0) {
    return;
  }

  term.setBg(bgColor);
  term.setFg(fgColor);

  // Draw current input on the first line.
  std::string line = std::string(kPrompt) + currentInput;
  writeClampedLine(term, y + kFrameOffset, x + kFrameOffset, contentWidth, line);

  // Clear the remaining content area.
  int row = kFrameOffset;
  while (row < contentHeight) {
    writeClampedLine(term, y + kFrameOffset + row, x + kFrameOffset, contentWidth, "");
    row += 1;
  }

  // Place cursor after the prompt + input, clamped to the frame width.
  int cursorCol = x + kFrameOffset + kPromptLength + static_cast<int>(currentInput.size());
  const int maxCol = x + kFrameOffset + contentWidth;
  if (cursorCol > maxCol) {
    cursorCol = maxCol;
  }
  term.moveTo(y + kFrameOffset, cursorCol);
}

}  // namespace tui
