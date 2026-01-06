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

MapView::MapView(int x, int y, int width, int height)
    : BaseView(x, y, width, height),
      mapState(nullptr),
      highlightCurrentPlayer(false),
      highlightNodeId(-1) {
  requestRedraw();
}

void MapView::setMapState(mapState::MapState* state) {
  if (mapState == state) {
    return;
  }
  mapState = state;
  if (!mapState) {
    highlightNodeId = -1;
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
  if (width <= 2 || height <= 2) {
    return;
  }

  const int contentWidth = width - 2;
  const int contentRows = height - 2;
  int row = y + 1;
  const int endRow = row + contentRows;

  term.setBg(bgColor);
  term.setFg(fgColor);

  if (!mapState) {
    const std::string placeholder = "No map data";
    writeClampedLine(term, row, x + 1, contentWidth, placeholder);
    ++row;
    while (row < endRow) {
      writeClampedLine(term, row, x + 1, contentWidth, "");
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

  const std::string stationsHeader = "Stations";
  writeClampedLine(term, row, x + 1, contentWidth, stationsHeader);
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

      writeClampedLine(term, row, x + 1, contentWidth, line.str());
      ++row;
  }

  if (row < endRow) {
    writeClampedLine(term, row, x + 1, contentWidth, "");
    ++row;
  }
  if (row < endRow) {
    const std::string routesHeader = "Routes";
    writeClampedLine(term, row, x + 1, contentWidth, routesHeader);
    ++row;
  }

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

    writeClampedLine(term, row, x + 1, contentWidth, line.str());
    ++row;
  }

  while (row < endRow) {
    writeClampedLine(term, row, x + 1, contentWidth, "");
    ++row;
  }
}

std::string MapView::colorCardToString(cardsState::ColorCard color) {
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
