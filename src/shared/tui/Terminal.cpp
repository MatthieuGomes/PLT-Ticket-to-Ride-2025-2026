#include "Terminal.h"

#include <algorithm>
#include <iostream>
#include <string>

namespace {

// ANSI SGR color codes for foreground and background colors.
const int kAnsiFgBlack = 30;
const int kAnsiFgRed = 31;
const int kAnsiFgGreen = 32;
const int kAnsiFgYellow = 33;
const int kAnsiFgBlue = 34;
const int kAnsiFgMagenta = 35;
const int kAnsiFgCyan = 36;
const int kAnsiFgWhite = 37;
const int kAnsiFgBrightBlack = 90;
const int kAnsiFgBrightRed = 91;
const int kAnsiFgBrightGreen = 92;
const int kAnsiFgBrightYellow = 93;
const int kAnsiFgBrightBlue = 94;
const int kAnsiFgBrightMagenta = 95;
const int kAnsiFgBrightCyan = 96;
const int kAnsiFgBrightWhite = 97;
const int kAnsiFgDefault = 39;

const int kAnsiBgBlack = 40;
const int kAnsiBgRed = 41;
const int kAnsiBgGreen = 42;
const int kAnsiBgYellow = 43;
const int kAnsiBgBlue = 44;
const int kAnsiBgMagenta = 45;
const int kAnsiBgCyan = 46;
const int kAnsiBgWhite = 47;
const int kAnsiBgBrightBlack = 100;
const int kAnsiBgBrightRed = 101;
const int kAnsiBgBrightGreen = 102;
const int kAnsiBgBrightYellow = 103;
const int kAnsiBgBrightBlue = 104;
const int kAnsiBgBrightMagenta = 105;
const int kAnsiBgBrightCyan = 106;
const int kAnsiBgBrightWhite = 107;
const int kAnsiBgDefault = 49;

// ANSI reset and cursor control sequences.
const int kAnsiReset = 0;
// Terminal coordinates are 1-based.
const int kMinPosition = 1;
const char kAnsiCursorHide[] = "\033[?25l";
const char kAnsiCursorShow[] = "\033[?25h";

int colorToAnsi(tui::Color color, bool background) {
  switch (color) {
    case tui::Color::Black:        return background ? kAnsiBgBlack : kAnsiFgBlack;
    case tui::Color::Red:          return background ? kAnsiBgRed : kAnsiFgRed;
    case tui::Color::Green:        return background ? kAnsiBgGreen : kAnsiFgGreen;
    case tui::Color::Yellow:       return background ? kAnsiBgYellow : kAnsiFgYellow;
    case tui::Color::Blue:         return background ? kAnsiBgBlue : kAnsiFgBlue;
    case tui::Color::Magenta:      return background ? kAnsiBgMagenta : kAnsiFgMagenta;
    case tui::Color::Cyan:         return background ? kAnsiBgCyan : kAnsiFgCyan;
    case tui::Color::White:        return background ? kAnsiBgWhite : kAnsiFgWhite;
    case tui::Color::BrightBlack:  return background ? kAnsiBgBrightBlack : kAnsiFgBrightBlack;
    case tui::Color::BrightRed:    return background ? kAnsiBgBrightRed : kAnsiFgBrightRed;
    case tui::Color::BrightGreen:  return background ? kAnsiBgBrightGreen : kAnsiFgBrightGreen;
    case tui::Color::BrightYellow: return background ? kAnsiBgBrightYellow : kAnsiFgBrightYellow;
    case tui::Color::BrightBlue:   return background ? kAnsiBgBrightBlue : kAnsiFgBrightBlue;
    case tui::Color::BrightMagenta:return background ? kAnsiBgBrightMagenta : kAnsiFgBrightMagenta;
    case tui::Color::BrightCyan:   return background ? kAnsiBgBrightCyan : kAnsiFgBrightCyan;
    case tui::Color::BrightWhite:  return background ? kAnsiBgBrightWhite : kAnsiFgBrightWhite;
    case tui::Color::Default:
    default:
      return background ? kAnsiBgDefault : kAnsiFgDefault;
  }
}

}  // namespace

namespace tui {

Terminal::Terminal(std::string outputStream)
    : out(outputStream), cursorHidden(false) {}

void Terminal::moveTo(int row, int col) {
  // ANSI cursor positions are 1-based; clamp to avoid invalid output.
  row = std::max(kMinPosition, row);
  col = std::max(kMinPosition, col);
  out += "\033[" + std::to_string(row) + ";" + std::to_string(col) + "H";
}

void Terminal::setFg(Color color) {
  out += "\033[" + std::to_string(colorToAnsi(color, /*background=*/false)) + "m";
}

void Terminal::setBg(Color color) {
  out += "\033[" + std::to_string(colorToAnsi(color, /*background=*/true)) + "m";
}

void Terminal::resetStyles() {
  out += "\033[" + std::to_string(kAnsiReset) + "m";
}

void Terminal::clearRegion(int row, int col, int width, int height) {
  if (width <= 0 || height <= 0) {
    return;
  }
  // Clear each row individually; avoids relying on terminal scroll.
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
  out += kAnsiCursorHide;
  cursorHidden = true;
}

void Terminal::showCursor() {
  if (cursorHidden) {
    out += kAnsiCursorShow;
    cursorHidden = false;
  } else {
    out += kAnsiCursorShow;
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
