#include "MapView.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Terminal.h"
#include "mapState/RoadColor.h"
#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "mapState/Station.h"
#include "playersState/Player.h"

namespace tui {

namespace {

// Frame geometry: 2 cells for borders and content starts 1 cell in.
const int kFrameInset = 2;
const int kFrameOffset = 1;
// Sentinel used when no station is highlighted.
const int kNoHighlight = -1;
// Two-character color chip shown for each route.
const int kRouteChipWidth = 2;
const int kRouteChipGap = 1;

struct ChipColors {
  Color left;
  Color right;
};

ChipColors colorToChip(mapState::RoadColor color) {
  // Use bright variants to keep chips readable on darker backgrounds.
  switch (color) {
    case mapState::RoadColor::RED:
      return {Color::BrightRed, Color::BrightRed};
    case mapState::RoadColor::BLUE:
      return {Color::BrightBlue, Color::BrightBlue};
    case mapState::RoadColor::GREEN:
      return {Color::BrightGreen, Color::BrightGreen};
    case mapState::RoadColor::WHITE:
      return {Color::BrightWhite, Color::BrightWhite};
    case mapState::RoadColor::BLACK:
      return {Color::BrightBlack, Color::BrightBlack};
    case mapState::RoadColor::YELLOW:
      return {Color::BrightYellow, Color::BrightYellow};
    case mapState::RoadColor::PINK:
      return {Color::BrightMagenta, Color::BrightMagenta};
    case mapState::RoadColor::ORANGE:
      return {Color::BrightOrange, Color::BrightOrange};
    case mapState::RoadColor::NONE:
    case mapState::RoadColor::UNKNOWN:
    default:
      return {Color::BrightBlack, Color::BrightBlack};
  }
}

void drawColorChip(
    Terminal& term,
    int row,
    int col,
    const ChipColors& chip,
    Color baseFg,
    Color baseBg) {
  // Two adjacent cells form the chip, then restore the view colors.
  term.setBg(chip.left);
  term.setFg(chip.left);
  term.moveTo(row, col);
  term.writeRepeat(' ', 1);
  term.setBg(chip.right);
  term.setFg(chip.right);
  term.writeRepeat(' ', 1);
  term.setBg(baseBg);
  term.setFg(baseFg);
}

}  // namespace

MapView::MapView(int x, int y, int width, int height)
    : BaseView(x, y, width, height),
      mapState(nullptr),
      highlightCurrentPlayer(false),
      highlightNodeId(kNoHighlight) {
  requestRedraw();
}

void MapView::setMapState(mapState::MapState* state) {
  if (mapState == state) {
    return;
  }
  mapState = state;
  // Reset highlight if the map goes away.
  if (!mapState) {
    highlightNodeId = kNoHighlight;
  }
  requestRedraw();
}

void MapView::setHighlightActivePlayer(bool enable) {
  if (highlightCurrentPlayer == enable) {
    return;
  }
  highlightCurrentPlayer = enable;
  requestRedraw();
}

void MapView::setHighlightNode(int nodeID) {
  if (highlightNodeId == nodeID) {
    return;
  }
  highlightNodeId = nodeID;
  requestRedraw();
}

void MapView::refreshOwnerships() {
  requestRedraw();
}

void MapView::drawContent(Terminal& term) {
  if (width <= kFrameInset || height <= kFrameInset) {
    return;
  }

  const int contentWidth = width - kFrameInset;
  const int contentRows = height - kFrameInset;
  int row = y + kFrameOffset;
  const int endRow = row + contentRows;

  term.setBg(bgColor);
  term.setFg(fgColor);

  if (!mapState) {
    const std::string placeholder = "No map data";
    // Fill the view with a placeholder line and blank lines.
    writeClampedLine(term, row, x + kFrameOffset, contentWidth, placeholder);
    ++row;
    while (row < endRow) {
      writeClampedLine(term, row, x + kFrameOffset, contentWidth, "");
      ++row;
    }
    return;
  }

  const std::vector<std::shared_ptr<mapState::Station>> stations =
      mapState->getStations();
  const std::vector<std::shared_ptr<mapState::Road>> roads =
      mapState->getRoads();

  std::shared_ptr<mapState::Station> highlightedStation;
  if (highlightNodeId >= 0 &&
      highlightNodeId < static_cast<int>(stations.size())) {
    highlightedStation = stations[static_cast<std::size_t>(highlightNodeId)];
  }

  // Station list.
  const std::string stationsHeader = "Stations";
  writeClampedLine(term, row, x + kFrameOffset, contentWidth, stationsHeader);
  ++row;
  for (const std::shared_ptr<mapState::Station>& station : stations) {
    if (row >= endRow) {
      break;
    }

    std::ostringstream line;
    line << ((station == highlightedStation) ? ">" : " ") << station->getName();

    if (station->getOwner() != nullptr) {
      line << " [" << station->getOwner()->getName() << "]";
    }

    writeClampedLine(term, row, x + kFrameOffset, contentWidth, line.str());
    ++row;
  }

  if (row < endRow) {
    writeClampedLine(term, row, x + kFrameOffset, contentWidth, "");
    ++row;
  }
  if (row < endRow) {
    const std::string routesHeader = "Routes";
    writeClampedLine(term, row, x + kFrameOffset, contentWidth, routesHeader);
    ++row;
  }

  // Route list.
  for (const std::shared_ptr<mapState::Road>& road : roads) {
    if (row >= endRow) {
      break;
    }

    std::ostringstream line;
    const bool emphasizeOwner =
        highlightCurrentPlayer && road->getOwner() != nullptr;

    line << (emphasizeOwner ? "*" : " ") << road->getStationA()->getName()
         << "-" << road->getStationB()->getName()
         << " len:" << road->getLength();

    if (road->getOwner() != nullptr) {
      line << " [" << road->getOwner()->getName() << "]";
    }

    const bool drawChip = contentWidth >= kRouteChipWidth;
    int gap = 0;
    if (drawChip && contentWidth > kRouteChipWidth + kRouteChipGap) {
      gap = kRouteChipGap;
    }
    int textWidth = contentWidth;
    if (drawChip) {
      textWidth = contentWidth - kRouteChipWidth - gap;
      if (textWidth < 0) {
        textWidth = 0;
      }
    }

    writeClampedLine(term, row, x + kFrameOffset, textWidth, line.str());
    if (gap > 0) {
      term.moveTo(row, x + kFrameOffset + textWidth);
      term.writeRepeat(' ', gap);
    }
    if (drawChip) {
      const ChipColors chip = colorToChip(road->getColor());
      const int chipCol = x + kFrameOffset + textWidth + gap;
      drawColorChip(term, row, chipCol, chip, fgColor, bgColor);
    }
    ++row;
  }

  while (row < endRow) {
    writeClampedLine(term, row, x + kFrameOffset, contentWidth, "");
    ++row;
  }
}

std::string MapView::colorCardToString(mapState::RoadColor color) {
  // Compact labels keep the route list readable.
  switch (color) {
    case mapState::RoadColor::RED: return "Red";
    case mapState::RoadColor::BLUE: return "Blue";
    case mapState::RoadColor::GREEN: return "Green";
    case mapState::RoadColor::WHITE: return "White";
    case mapState::RoadColor::BLACK: return "Black";
    case mapState::RoadColor::YELLOW: return "Yellow";
    case mapState::RoadColor::PINK: return "Pink";
    case mapState::RoadColor::ORANGE: return "Orange";
    case mapState::RoadColor::NONE: return "-";
    case mapState::RoadColor::UNKNOWN:
    default:
      return "?";
  }
}

void MapView::writeClampedLine(
    Terminal& term,
    int row,
    int col,
    int width,
    const std::string& text) {
  // Truncate line to fit the frame and clear the rest of the row.
  std::string buffer = text;
  if (static_cast<int>(buffer.size()) > width) {
    buffer.resize(width);
  }
  term.moveTo(row, col);
  term.write(buffer);
  const int remaining = width - static_cast<int>(buffer.size());
  if (remaining > 0) {
    term.writeRepeat(' ', remaining);
  }
}

}  // namespace tui
