#include "InfoPanel.h"

#include <algorithm>
#include <iostream>

#include "Terminal.h"

namespace tui {

namespace {

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

InfoPanel::InfoPanel(int x, int y, int width, int height)
    : BaseView(x, y, width, height), scrollOffset(0) {
  requestRedraw();
}

void InfoPanel::addMessage(const std::string& message) {
  messages.push_back(message);
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
  int maxOffset = static_cast<int>(messages.size()) - getVisibleLineCount();
  if (maxOffset < 0) {
    maxOffset = 0;
  }
  scrollOffset = clampValue(scrollOffset + delta, 0, maxOffset);
  requestRedraw();
}

int InfoPanel::getVisibleLineCount() const {
  int lines = height - 2;
  if (lines < 0) {
    lines = 0;
  }
  return lines;
}

void InfoPanel::drawContent(Terminal& term) {
  const int contentWidth = width - 2;
  const int contentHeight = getVisibleLineCount();
  if (contentWidth <= 0 || contentHeight <= 0) {
    return;
  }

  int maxOffset = static_cast<int>(messages.size()) - contentHeight;
  if (maxOffset < 0) {
    maxOffset = 0;
  }
  if (scrollOffset > maxOffset) {
    scrollOffset = maxOffset;
  }

  int startIndex = 0;
  if (static_cast<int>(messages.size()) > contentHeight) {
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
    writeClampedLine(term, y + 1 + row, x + 1, contentWidth, line);
    row += 1;
  }
}

}  // namespace tui
