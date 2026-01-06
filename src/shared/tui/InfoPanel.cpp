#include "InfoPanel.h"

#include <algorithm>
#include <iostream>

#include "Terminal.h"

namespace tui {

namespace {

// Frame geometry: 2 cells for borders and content starts 1 cell in.
const int kFrameInset = 2;
const int kFrameOffset = 1;
// Keep at least one message so the panel can display something.
const int kMinMaxMessages = 1;

int clampValue(int value, int minValue, int maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

void writeClampedLine(Terminal& term, int row, int col, int width,
                      const std::string& text) {
  // Clamp text to available width, then pad to clear leftover characters.
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

InfoPanel::InfoPanel(int x, int y, int width, int height, int maxMessages)
    : BaseView(x, y, width, height),
      scrollOffset(0),
      maxMessages(maxMessages) {
  if (this->maxMessages < kMinMaxMessages) {
    this->maxMessages = kMinMaxMessages;
  }
  requestRedraw();
}

void InfoPanel::addMessage(const std::string& message) {
  messages.push_back(message);
  // Trim the oldest messages once the buffer limit is reached.
  if (static_cast<int>(messages.size()) > maxMessages) {
    const int excess = static_cast<int>(messages.size()) - maxMessages;
    messages.erase(messages.begin(), messages.begin() + excess);
  }
  // Keep scroll offset valid as new messages arrive.
  int maxOffset = static_cast<int>(messages.size()) - getVisibleLineCount();
  if (maxOffset < 0) {
    maxOffset = 0;
  }
  if (scrollOffset > maxOffset) {
    scrollOffset = maxOffset;
  }
  requestRedraw();
}

void InfoPanel::clearMessages() {
  messages.clear();
  scrollOffset = 0;
  requestRedraw();
}

void InfoPanel::scroll(int delta) {
  // Clamp scrolling so we never go past the available message range.
  int maxOffset = static_cast<int>(messages.size()) - getVisibleLineCount();
  if (maxOffset < 0) {
    maxOffset = 0;
  }
  scrollOffset = clampValue(scrollOffset + delta, 0, maxOffset);
  requestRedraw();
}

int InfoPanel::getVisibleLineCount() const {
  // Content height excludes the top/bottom frame border.
  int lines = height - kFrameInset;
  if (lines < 0) {
    lines = 0;
  }
  return lines;
}

void InfoPanel::drawContent(Terminal& term) {
  const int contentWidth = width - kFrameInset;
  const int contentHeight = getVisibleLineCount();
  if (contentWidth <= 0 || contentHeight <= 0) {
    return;
  }

  // If the view shrinks, adjust scroll offset so it stays within bounds.
  int maxOffset = static_cast<int>(messages.size()) - contentHeight;
  if (maxOffset < 0) {
    maxOffset = 0;
  }
  if (scrollOffset > maxOffset) {
    scrollOffset = maxOffset;
  }

  int startIndex = 0;
  if (static_cast<int>(messages.size()) > contentHeight) {
    // Show the most recent messages, with scroll offset applied.
    startIndex = static_cast<int>(messages.size()) - contentHeight - scrollOffset;
  }

  term.setBg(bgColor);
  term.setFg(fgColor);

  int row = 0;
  while (row < contentHeight) {
    int index = startIndex + row;
    std::string line;
    if (index >= 0 && index < static_cast<int>(messages.size())) {
      line = messages[static_cast<std::size_t>(index)];
    }
    writeClampedLine(term, y + kFrameOffset + row, x + kFrameOffset, contentWidth, line);
    row += 1;
  }
}

}  // namespace tui
