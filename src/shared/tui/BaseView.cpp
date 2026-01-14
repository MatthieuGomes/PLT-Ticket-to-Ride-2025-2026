#include "BaseView.h"

#include <algorithm>
#include <iostream>

#include "Terminal.h"

namespace tui {

namespace {

// Frame geometry: 2 cells for the left/right border, and content starts 1 cell in.
const int kFrameInset = 2;
const int kFrameOffset = 1;
// Use a solid gray border (bright black) made of spaces with a gray background.
const Color kFrameBorderFg = Color::BrightBlack;
const Color kFrameBorderBg = Color::BrightBlack;
// Focused border uses a brighter background for emphasis.
const Color kFocusBorderFg = Color::Black;
const Color kFocusBorderBg = Color::BrightWhite;
const char kFrameBorderFill = ' ';
// Header needs at least corners + 2 chars to be worth drawing.
const int kHeaderMinWidth = 4;
// Keep header text away from the left corner.
const int kHeaderPadding = 2;
// Title label colors: light gray background with black text.
const Color kHeaderTextFg = Color::Black;
const Color kHeaderTextBg = Color::BrightWhite;
// Focused header tint to highlight the active panel.
const Color kHeaderFocusTextFg = Color::Black;
const Color kHeaderFocusTextBg = Color::BrightYellow;

}  // namespace

BaseView::BaseView(int x, int y, int width, int height, const std::string& title)
    : x(x),
      y(y),
      width(width),
      height(height),
      title(title),
      needsRedraw(true),
      bgColor(Color::Default),
      fgColor(Color::Default),
      focused(false) {}

BaseView::~BaseView() {}

void BaseView::setPosition(int newX, int newY) {
  if (x == newX && y == newY) {
    return;
  }
  x = newX;
  y = newY;
  requestRedraw();
}

void BaseView::setSize(int newWidth, int newHeight) {
  if (width == newWidth && height == newHeight) {
    return;
  }
  width = newWidth;
  height = newHeight;
  requestRedraw();
}

void BaseView::setTitle(const std::string& newTitle) {
  if (title == newTitle) {
    return;
  }
  title = newTitle;
  requestRedraw();
}

void BaseView::setColors(Color fg, Color bg) {
  if (fgColor == fg && bgColor == bg) {
    return;
  }
  fgColor = fg;
  bgColor = bg;
  requestRedraw();
}

void BaseView::setFocused(bool value) {
  if (focused == value) {
    return;
  }
  focused = value;
  requestRedraw();
}

const std::string& BaseView::getTitle() const {
  return title;
}

void BaseView::requestRedraw() {
  needsRedraw = true;
}

bool BaseView::isRedrawNeeded() const {
  return needsRedraw;
}

void BaseView::clear(Terminal& term) {
  if (width <= 0 || height <= 0) {
    return;
  }
  // Clear the full view area using the current view colors.
  term.setBg(bgColor);
  term.setFg(fgColor);
  term.clearRegion(y, x, width, height);
  term.resetStyles();
}

void BaseView::draw(Terminal& term) {
  drawFrame(term);
  drawHeader(term);
  drawContent(term);
}

void BaseView::refresh(Terminal& term) {
  if (!needsRedraw) {
    return;
  }
  // Always clear before redrawing to avoid leftover text from previous frames.
  clear(term);
  draw(term);
  needsRedraw = false;
}

void BaseView::handleEvent(const std::string& event) {
  (void)event;
}

void BaseView::drawFrame(Terminal& term) {
  if (width <= 0 || height <= 0) {
    return;
  }
  if (width < kFrameInset || height < kFrameInset) {
    return;
  }

  Color borderBg = focused ? kFocusBorderBg : kFrameBorderBg;
  Color borderFg = focused ? kFocusBorderFg : kFrameBorderFg;
  term.setBg(borderBg);
  term.setFg(borderFg);

  // Top border (solid block).
  term.moveTo(y, x);
  term.writeRepeat(kFrameBorderFill, width);

  // Vertical borders.
  int row = kFrameOffset;
  while (row < height - 1) {
    term.moveTo(y + row, x);
    term.writeRepeat(kFrameBorderFill, kFrameOffset);
    term.moveTo(y + row, x + width - kFrameOffset);
    term.writeRepeat(kFrameBorderFill, kFrameOffset);
    row += 1;
  }

  // Bottom border (solid block).
  term.moveTo(y + height - 1, x);
  term.writeRepeat(kFrameBorderFill, width);

  // Restore view colors for follow-up drawing.
  term.setBg(bgColor);
  term.setFg(fgColor);
}

void BaseView::drawHeader(Terminal& term) {
  if (title.empty()) {
    return;
  }
  if (width < kHeaderMinWidth || height < 1) {
    return;
  }

  std::string text = title;
  // Truncate the title so it stays inside the frame.
  const int maxLen = width - kHeaderMinWidth;
  if (static_cast<int>(text.size()) > maxLen) {
    text.resize(static_cast<std::size_t>(maxLen));
  }

  term.setBg(bgColor);
  term.setFg(fgColor);
  Color headerBg = focused ? kHeaderFocusTextBg : kHeaderTextBg;
  Color headerFg = focused ? kHeaderFocusTextFg : kHeaderTextFg;
  term.setBg(headerBg);
  term.setFg(headerFg);
  term.moveTo(y, x + kHeaderPadding);
  term.write(text);
  // Restore view colors for the remaining drawing operations.
  term.setBg(bgColor);
  term.setFg(fgColor);
}

void BaseView::drawContent(Terminal& term) {
  (void)term;
}

int BaseView::getX() const {
  return x;
}

void BaseView::setX(int value) {
  if (x == value) {
    return;
  }
  x = value;
  requestRedraw();
}

int BaseView::getY() const {
  return y;
}

void BaseView::setY(int value) {
  if (y == value) {
    return;
  }
  y = value;
  requestRedraw();
}

int BaseView::getWidth() const {
  return width;
}

void BaseView::setWidth(int value) {
  if (width == value) {
    return;
  }
  width = value;
  requestRedraw();
}

int BaseView::getHeight() const {
  return height;
}

void BaseView::setHeight(int value) {
  if (height == value) {
    return;
  }
  height = value;
  requestRedraw();
}

bool BaseView::getNeedsRedraw() const {
  return needsRedraw;
}

void BaseView::setNeedsRedraw(bool value) {
  needsRedraw = value;
}

Color BaseView::getBgColor() const {
  return bgColor;
}

void BaseView::setBgColor(Color value) {
  if (bgColor == value) {
    return;
  }
  bgColor = value;
  requestRedraw();
}

Color BaseView::getFgColor() const {
  return fgColor;
}

void BaseView::setFgColor(Color value) {
  if (fgColor == value) {
    return;
  }
  fgColor = value;
  requestRedraw();
}

bool BaseView::getFocused() const {
  return focused;
}

}  // namespace tui
