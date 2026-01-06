#include "BaseView.h"

#include <algorithm>
#include <iostream>

#include "Terminal.h"

namespace tui {

namespace {

// Frame geometry: 2 cells for the left/right border, and content starts 1 cell in.
const int kFrameInset = 2;
const int kFrameOffset = 1;
// Header needs at least corners + 2 chars to be worth drawing.
const int kHeaderMinWidth = 4;
// Keep header text away from the left corner.
const int kHeaderPadding = 2;

}  // namespace

BaseView::BaseView(int x, int y, int width, int height, const std::string& title)
    : x(x),
      y(y),
      width(width),
      height(height),
      title(title),
      needsRedraw(true),
      bgColor(Color::Default),
      fgColor(Color::Default) {}

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

  term.setBg(bgColor);
  term.setFg(fgColor);

  // Top border.
  term.moveTo(y, x);
  term.write("+");
  if (width > kFrameInset) {
    term.writeRepeat('-', width - kFrameInset);
  }
  term.write("+");

  // Vertical borders.
  int row = kFrameOffset;
  while (row < height - 1) {
    term.moveTo(y + row, x);
    term.write("|");
    if (width > kFrameInset) {
      term.writeRepeat(' ', width - kFrameInset);
    }
    term.write("|");
    row += 1;
  }

  // Bottom border.
  term.moveTo(y + height - 1, x);
  term.write("+");
  if (width > kFrameInset) {
    term.writeRepeat('-', width - kFrameInset);
  }
  term.write("+");
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
  term.moveTo(y, x + kHeaderPadding);
  term.write(text);
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

}  // namespace tui
