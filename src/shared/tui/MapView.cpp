#include "MapView.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Terminal.h"
#include "cardsState/ColorCard.h"
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
    if (station->getBlockStatus()) {
      line << " (blocked)";
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
         << "-" << road->getStationB()->getName() << " ";

    line << colorCardToString(road->getColor())
         << " len:" << road->getLength();

    if (road->getOwner() != nullptr) {
      line << " [" << road->getOwner()->getName() << "]";
    }

    writeClampedLine(term, row, x + kFrameOffset, contentWidth, line.str());
    ++row;
  }

  while (row < endRow) {
    writeClampedLine(term, row, x + kFrameOffset, contentWidth, "");
    ++row;
  }
}

std::string MapView::colorCardToString(cardsState::ColorCard color) {
  // Compact labels keep the route list readable.
  switch (color) {
    case cardsState::ColorCard::RED: return "Red";
    case cardsState::ColorCard::BLUE: return "Blue";
    case cardsState::ColorCard::GREEN: return "Green";
    case cardsState::ColorCard::WHITE: return "White";
    case cardsState::ColorCard::BLACK: return "Black";
    case cardsState::ColorCard::YELLOW: return "Yellow";
    case cardsState::ColorCard::PINK: return "Pink";
    case cardsState::ColorCard::ORANGE: return "Orange";
    case cardsState::ColorCard::LOCOMOTIVE: return "Loco";
    case cardsState::ColorCard::NONE:
    default:
      return "-";
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
