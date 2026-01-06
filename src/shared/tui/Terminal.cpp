#include "Terminal.h"

#include <algorithm>
#include <iostream>
#include <string>

namespace {

int colorToAnsi(tui::Color color, bool background) {
  switch (color) {
    case tui::Color::Black:        return background ? 40 : 30;
    case tui::Color::Red:          return background ? 41 : 31;
    case tui::Color::Green:        return background ? 42 : 32;
    case tui::Color::Yellow:       return background ? 43 : 33;
    case tui::Color::Blue:         return background ? 44 : 34;
    case tui::Color::Magenta:      return background ? 45 : 35;
    case tui::Color::Cyan:         return background ? 46 : 36;
    case tui::Color::White:        return background ? 47 : 37;
    case tui::Color::BrightBlack:  return background ? 100 : 90;
    case tui::Color::BrightRed:    return background ? 101 : 91;
    case tui::Color::BrightGreen:  return background ? 102 : 92;
    case tui::Color::BrightYellow: return background ? 103 : 93;
    case tui::Color::BrightBlue:   return background ? 104 : 94;
    case tui::Color::BrightMagenta:return background ? 105 : 95;
    case tui::Color::BrightCyan:   return background ? 106 : 96;
    case tui::Color::BrightWhite:  return background ? 107 : 97;
    case tui::Color::Default:
    default:
      return background ? 49 : 39;
  }
}

}  // namespace

namespace tui {

Terminal::Terminal(std::string outputStream)
    : out(outputStream), cursorHidden(false) {}

void Terminal::moveTo(int row, int col) {
  row = std::max(1, row);
  col = std::max(1, col);
  out += "\033[" + std::to_string(row) + ";" + std::to_string(col) + "H";
}

void Terminal::setFg(Color color) {
  out += "\033[" + std::to_string(colorToAnsi(color, /*background=*/false)) + "m";
}

void Terminal::setBg(Color color) {
  out += "\033[" + std::to_string(colorToAnsi(color, /*background=*/true)) + "m";
}

void Terminal::resetStyles() {
  out += "\033[0m";
}

void Terminal::clearRegion(int row, int col, int width, int height) {
  if (width <= 0 || height <= 0) {
    return;
  }
  for (int r = 0; r < height; ++r) {
    moveTo(row + r, col);
    writeRepeat(' ', width);
  }
  moveTo(row, col);
}

void Terminal::write(const std::string& text) {
  out += text;
}

void Terminal::writeRepeat(char ch, int count) {
  if (count <= 0) {
    return;
  }
  out.append(static_cast<std::size_t>(count), ch);
}

void Terminal::hideCursor() {
  if (cursorHidden) {
    return;
  }
  out += "\033[?25l";
  cursorHidden = true;
}

void Terminal::showCursor() {
  if (cursorHidden) {
    out += "\033[?25h";
    cursorHidden = false;
  } else {
    out += "\033[?25h";
  }
}

void Terminal::flush() {
  if (out.empty()) {
    return;
  }
  std::cout << out;
  std::cout.flush();
  out.clear();
}

}  // namespace tui
