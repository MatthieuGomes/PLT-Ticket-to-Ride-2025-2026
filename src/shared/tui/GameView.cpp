#include "GameView.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Terminal.h"
#include "cardsState/CardsState.h"
#include "cardsState/DestinationCard.h"
#include "cardsState/PlayerCards.h"
#include "cardsState/WagonCard.h"
#include "mapState/Ferry.h"
#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "mapState/RoadColor.h"
#include "mapState/Station.h"
#include "mapState/Tunnel.h"
#include "playersState/Player.h"
#include "playersState/PlayersState.h"

namespace tui {

namespace {

// Frame geometry: 2 cells for borders and content starts 1 cell in.
const int kFrameInset = 2;
const int kFrameOffset = 1;
// Sentinel used when no station is highlighted.
const int kNoHighlight = -1;
// Layout defaults.
const char kDefaultLayoutPath[] = "ressources/europe_layout.txt";
// Map layout sizing.
const int kMinMapWidth = 16;
const int kMinDetailsWidth = 26;
const int kMaxDetailsWidth = 40;
const int kPanelGap = 2;
const int kScrollMargin = 1;
// When no player is available.
const char kNoPlayerText[] = "No player data";
const int kCardBlockWidth = 2;
const int kCardBlockGap = 2;
const int kCardLabelWidth = 3;

Color roadColorToAnsi(mapState::RoadColor color) {
  switch (color) {
    case mapState::RoadColor::RED: return Color::BrightRed;
    case mapState::RoadColor::BLUE: return Color::BrightBlue;
    case mapState::RoadColor::GREEN: return Color::BrightGreen;
    case mapState::RoadColor::WHITE: return Color::BrightWhite;
    case mapState::RoadColor::BLACK: return Color::BrightBlack;
    case mapState::RoadColor::YELLOW: return Color::BrightYellow;
    case mapState::RoadColor::PINK: return Color::BrightMagenta;
    case mapState::RoadColor::ORANGE: return Color::BrightOrange;
    case mapState::RoadColor::NONE:
    case mapState::RoadColor::UNKNOWN:
    default:
      return Color::BrightBlack;
  }
}

std::string roadTypeToken(const std::shared_ptr<mapState::Road>& road) {
  if (!road) {
    return "R";
  }
  if (std::dynamic_pointer_cast<mapState::Tunnel>(road)) {
    return "T";
  }
  if (std::dynamic_pointer_cast<mapState::Ferry>(road)) {
    return "F";
  }
  return "R";
}

std::string wagonColorToString(cardsState::ColorCard color) {
  switch (color) {
    case cardsState::ColorCard::RED: return "RED";
    case cardsState::ColorCard::BLUE: return "BLU";
    case cardsState::ColorCard::GREEN: return "GRN";
    case cardsState::ColorCard::BLACK: return "BLK";
    case cardsState::ColorCard::YELLOW: return "YEL";
    case cardsState::ColorCard::ORANGE: return "ORG";
    case cardsState::ColorCard::PINK: return "PNK";
    case cardsState::ColorCard::WHITE: return "WHT";
    case cardsState::ColorCard::LOCOMOTIVE: return "LOC";
    case cardsState::ColorCard::UNKNOWN:
    default:
      return "UNK";
  }
}

Color wagonColorToAnsi(cardsState::ColorCard color) {
  switch (color) {
    case cardsState::ColorCard::RED: return Color::BrightRed;
    case cardsState::ColorCard::BLUE: return Color::BrightBlue;
    case cardsState::ColorCard::GREEN: return Color::BrightGreen;
    case cardsState::ColorCard::BLACK: return Color::BrightBlack;
    case cardsState::ColorCard::YELLOW: return Color::BrightYellow;
    case cardsState::ColorCard::ORANGE: return Color::BrightOrange;
    case cardsState::ColorCard::PINK: return Color::BrightMagenta;
    case cardsState::ColorCard::WHITE: return Color::BrightWhite;
    case cardsState::ColorCard::LOCOMOTIVE: return Color::BrightWhite;
    case cardsState::ColorCard::UNKNOWN:
    default:
      return Color::BrightBlack;
  }
}

std::string toUpperShort(const std::string& text) {
  std::string out;
  for (std::size_t i = 0; i < text.size() && out.size() < 3; ++i) {
    char ch = text[i];
    out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
  }
  while (out.size() < 3) {
    out.push_back(' ');
  }
  return out;
}

std::string stationNameOrPlaceholder(const std::shared_ptr<mapState::Station>& station) {
  if (station) {
    return station->getName();
  }
  return "Unknown";
}

void drawWagonCards(
    Terminal& term,
    int row,
    int col,
    int width,
    const std::vector<std::shared_ptr<cardsState::WagonCard>>& cards,
    Color fg,
    Color bg) {
  if (width <= 0) {
    return;
  }

  int cursor = 0;
  for (std::size_t i = 0; i < cards.size(); ++i) {
    if (cursor + kCardBlockWidth + kCardLabelWidth > width) {
      break;
    }
    std::shared_ptr<cardsState::WagonCard> card = cards[i];
    if (!card) {
      continue;
    }

    term.moveTo(row, col + cursor);
    if (card->getColor() == cardsState::ColorCard::LOCOMOTIVE) {
      term.setBg(Color::BrightMagenta);
      term.setFg(Color::Black);
      term.writeRepeat(' ', 1);
      term.setBg(Color::BrightCyan);
      term.setFg(Color::Black);
      term.writeRepeat(' ', 1);
    } else {
      term.setBg(wagonColorToAnsi(card->getColor()));
      term.setFg(Color::Black);
      term.writeRepeat(' ', kCardBlockWidth);
    }
    term.setBg(bg);
    term.setFg(fg);

    term.write(" ");
    term.write(wagonColorToString(card->getColor()));

    cursor += kCardBlockWidth;
    cursor += 1 + kCardLabelWidth;
    if (cursor + kCardBlockGap > width) {
      break;
    }
    term.moveTo(row, col + cursor);
    term.writeRepeat(' ', kCardBlockGap);
    cursor += kCardBlockGap;
  }
}

std::string trim(const std::string& input) {
  std::size_t start = 0;
  while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start])) != 0) {
    ++start;
  }
  if (start == input.size()) {
    return std::string();
  }
  std::size_t end = input.size();
  while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1])) != 0) {
    --end;
  }
  return input.substr(start, end - start);
}

int clampInt(int value, int minimum, int maximum) {
  if (value < minimum) {
    return minimum;
  }
  if (value > maximum) {
    return maximum;
  }
  return value;
}

}  // namespace

GameView::GameView(int x, int y, int width, int height)
    : BaseView(x, y, width, height),
      mapState(nullptr),
      playerState(nullptr),
      cardState(nullptr),
      mode(ViewMode::MAP),
      highlightCurrentPlayer(false),
      highlightNodeId(kNoHighlight),
      scrollX(0),
      scrollY(0),
      layoutLoaded(false),
      layoutMinRow(0),
      layoutMaxRow(0),
      layoutMinCol(0),
      layoutMaxCol(0),
      layoutPath(kDefaultLayoutPath),
      layoutEntries() {
  requestRedraw();
}

void GameView::setMapState(mapState::MapState* state) {
  if (mapState == state) {
    return;
  }
  mapState = state;
  if (!mapState) {
    highlightNodeId = kNoHighlight;
    layoutLoaded = false;
    layoutEntries.clear();
    requestRedraw();
    return;
  }

  layoutLoaded = loadLayout(layoutPath);
  if (!layoutLoaded) {
    std::string fallbackPath = std::string("../") + layoutPath;
    layoutLoaded = loadLayout(fallbackPath);
  }
  if (layoutLoaded) {
    std::vector<std::shared_ptr<mapState::Station>> stations = mapState->getStations();
    if (stations.size() != layoutEntries.size()) {
      layoutLoaded = false;
    } else {
      for (const std::shared_ptr<mapState::Station>& station : stations) {
        if (!station) {
          continue;
        }
        if (findLayoutIndexByName(station->getName()) < 0) {
          layoutLoaded = false;
          break;
        }
      }
    }
  }

  if (highlightNodeId == kNoHighlight) {
    highlightNodeId = 0;
  }
  requestRedraw();
}

void GameView::setPlayerState(playersState::PlayersState* state) {
  if (playerState == state) {
    return;
  }
  playerState = state;
  requestRedraw();
}

void GameView::setCardsState(cardsState::CardsState* state) {
  if (cardState == state) {
    return;
  }
  cardState = state;
  requestRedraw();
}

void GameView::setMode(ViewMode newMode) {
  if (mode == newMode) {
    return;
  }
  mode = newMode;
  requestRedraw();
}

ViewMode GameView::getMode() const {
  return mode;
}

void GameView::setHighlightActivePlayer(bool enable) {
  if (highlightCurrentPlayer == enable) {
    return;
  }
  highlightCurrentPlayer = enable;
  requestRedraw();
}

void GameView::setHighlightNode(int nodeID) {
  if (highlightNodeId == nodeID) {
    return;
  }
  highlightNodeId = nodeID;
  requestRedraw();
}

void GameView::moveSelection(int deltaRow, int deltaCol) {
  if (mode != ViewMode::MAP || !layoutLoaded || layoutEntries.empty()) {
    return;
  }
  if (highlightNodeId < 0 || highlightNodeId >= static_cast<int>(layoutEntries.size())) {
    highlightNodeId = 0;
    requestRedraw();
    return;
  }

  // Group stations by their row, then order each row by column.
  std::map<int, std::vector<int>> rows;
  for (std::size_t i = 0; i < layoutEntries.size(); ++i) {
    rows[layoutEntries[i].row].push_back(static_cast<int>(i));
  }

  std::vector<int> rowKeys;
  for (std::map<int, std::vector<int>>::iterator it = rows.begin(); it != rows.end(); ++it) {
    std::vector<int>& indices = it->second;
    std::sort(indices.begin(), indices.end(),
              [this](int left, int right) {
                return layoutEntries[static_cast<std::size_t>(left)].col <
                    layoutEntries[static_cast<std::size_t>(right)].col;
              });
    rowKeys.push_back(it->first);
  }

  const StationLayout& current = layoutEntries[static_cast<std::size_t>(highlightNodeId)];
  std::map<int, std::vector<int>>::iterator rowIt = rows.find(current.row);
  if (rowIt == rows.end()) {
    return;
  }

  const std::vector<int>& currentRow = rowIt->second;
  int currentColIndex = 0;
  for (std::size_t i = 0; i < currentRow.size(); ++i) {
    if (currentRow[i] == highlightNodeId) {
      currentColIndex = static_cast<int>(i);
      break;
    }
  }

  int rowPosition = 0;
  bool rowFound = false;
  for (std::size_t i = 0; i < rowKeys.size(); ++i) {
    if (rowKeys[i] == current.row) {
      rowPosition = static_cast<int>(i);
      rowFound = true;
      break;
    }
  }
  if (!rowFound) {
    return;
  }

  int targetIndex = -1;
  if (deltaRow != 0) {
    int step = (deltaRow > 0) ? 1 : -1;
    int newRowPos = rowPosition + step;
    if (newRowPos < 0 || newRowPos >= static_cast<int>(rowKeys.size())) {
      return;
    }
    std::vector<int>& targetRow = rows[rowKeys[static_cast<std::size_t>(newRowPos)]];
    int targetColIndex = currentColIndex;
    if (targetColIndex >= static_cast<int>(targetRow.size())) {
      targetColIndex = static_cast<int>(targetRow.size()) - 1;
    }
    if (targetColIndex < 0) {
      return;
    }
    targetIndex = targetRow[static_cast<std::size_t>(targetColIndex)];
  } else if (deltaCol != 0) {
    int step = (deltaCol > 0) ? 1 : -1;
    int targetColIndex = currentColIndex + step;
    if (targetColIndex < 0 || targetColIndex >= static_cast<int>(currentRow.size())) {
      return;
    }
    targetIndex = currentRow[static_cast<std::size_t>(targetColIndex)];
  }

  if (targetIndex >= 0 && targetIndex != highlightNodeId) {
    highlightNodeId = targetIndex;
    requestRedraw();
  }
}

void GameView::refreshOwnerships() {
  requestRedraw();
}

void GameView::drawContent(Terminal& term) {
  if (width <= kFrameInset || height <= kFrameInset) {
    return;
  }

  const int contentWidth = width - kFrameInset;
  const int contentRows = height - kFrameInset;
  int row = y + kFrameOffset;
  const int endRow = row + contentRows;

  term.setBg(bgColor);
  term.setFg(fgColor);

  if (mode == ViewMode::PLAYER) {
    const int playerWidth = contentWidth;
    for (int r = 0; r < contentRows; ++r) {
      writeClampedLine(term, row + r, x + kFrameOffset, playerWidth, "");
    }
    if (!playerState || playerState->players.empty()) {
      writeClampedLine(term, row, x + kFrameOffset, playerWidth, kNoPlayerText);
      return;
    }

    std::shared_ptr<playersState::Player> player = playerState->players[0];
    std::ostringstream header;
    header << "Player: " << player->getName() << " | Score " << player->getScore();
    writeClampedLine(term, row, x + kFrameOffset, playerWidth, header.str());
    row += 2;

    std::ostringstream wagons;
    wagons << "Wagons: " << player->getNbWagons() << " | Stations: " << player->getNbStations();
    writeClampedLine(term, row, x + kFrameOffset, playerWidth, wagons.str());
    row += 2;

    std::shared_ptr<cardsState::PlayerCards> hand = player->getHand();
    writeClampedLine(term, row, x + kFrameOffset, playerWidth, "Hand:");
    ++row;

    if (hand && hand->wagonCards) {
      std::vector<std::shared_ptr<cardsState::WagonCard>> wagonsCards = hand->wagonCards->cards;
      writeClampedLine(term, row, x + kFrameOffset, playerWidth, "");
      drawWagonCards(term, row, x + kFrameOffset, playerWidth, wagonsCards, fgColor, bgColor);
      ++row;
    }

    if (cardState && cardState->gameWagonCards && cardState->gameWagonCards->faceUpCards) {
      writeClampedLine(term, row, x + kFrameOffset, playerWidth, "Face-up:");
      ++row;
      const std::vector<std::shared_ptr<cardsState::WagonCard>> faceUpCards =
          cardState->gameWagonCards->faceUpCards->cards;
      writeClampedLine(term, row, x + kFrameOffset, playerWidth, "");
      drawWagonCards(term, row, x + kFrameOffset, playerWidth, faceUpCards, fgColor, bgColor);
      row += 2;
    }

    writeClampedLine(term, row, x + kFrameOffset, playerWidth, "Destinations:");
    ++row;
    if (hand && hand->destinationCards) {
      const std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards =
          hand->destinationCards->cards;
      for (std::size_t i = 0; i < destCards.size() && row < endRow; ++i) {
        if (!destCards[i]) {
          continue;
        }
        std::shared_ptr<mapState::Station> stationA = destCards[i]->getstationA();
        std::shared_ptr<mapState::Station> stationB = destCards[i]->getstationB();
        const std::string a = stationNameOrPlaceholder(stationA);
        const std::string b = stationNameOrPlaceholder(stationB);
        bool completed = false;
        for (std::size_t j = 0; j < player->completedDestinations.size(); ++j) {
          std::shared_ptr<cardsState::DestinationCard> completedCard =
              player->completedDestinations[j];
          if (!completedCard) {
            continue;
          }
          std::shared_ptr<mapState::Station> completedA = completedCard->getstationA();
          std::shared_ptr<mapState::Station> completedB = completedCard->getstationB();
          if (!completedA || !completedB || !stationA || !stationB) {
            continue;
          }
          if (completedA->getName() == a && completedB->getName() == b) {
            completed = true;
            break;
          }
        }
        std::ostringstream line;
        line << a << "-" << b << " " << (completed ? "V" : "X");
        std::string lineText = line.str();
        writeClampedLine(term, row, x + kFrameOffset, playerWidth, lineText);
        int markerOffset = static_cast<int>(lineText.size()) - 1;
        if (markerOffset >= 0 && markerOffset < playerWidth) {
          term.setBg(bgColor);
          term.setFg(completed ? Color::BrightGreen : Color::BrightRed);
          term.moveTo(row, x + kFrameOffset + markerOffset);
          term.write(completed ? "V" : "X");
          term.setFg(fgColor);
        }
        ++row;
      }
    }
    return;
  }

  if (!mapState) {
    const std::string placeholder = "No map data";
    writeClampedLine(term, row, x + kFrameOffset, contentWidth, placeholder);
    ++row;
    while (row < endRow) {
      writeClampedLine(term, row, x + kFrameOffset, contentWidth, "");
      ++row;
    }
    return;
  }

  int detailsWidth = std::min(kMaxDetailsWidth, std::max(kMinDetailsWidth, contentWidth / 3));
  int mapWidth = contentWidth - detailsWidth - kPanelGap;
  if (mapWidth < kMinMapWidth) {
    mapWidth = kMinMapWidth;
    detailsWidth = contentWidth - mapWidth - kPanelGap;
    if (detailsWidth < 0) {
      detailsWidth = 0;
    }
  }
  const int detailsX = x + kFrameOffset + mapWidth + kPanelGap;

  for (int r = 0; r < contentRows; ++r) {
    writeClampedLine(term, row + r, x + kFrameOffset, mapWidth, "");
    if (detailsWidth > 0) {
      writeClampedLine(term, row + r, detailsX, detailsWidth, "");
    }
  }

  if (!layoutLoaded || layoutEntries.empty()) {
    const std::vector<std::shared_ptr<mapState::Station>> stations = mapState->getStations();
    const std::vector<std::shared_ptr<mapState::Road>> roads = mapState->getRoads();

    std::shared_ptr<mapState::Station> highlightedStation;
    if (highlightNodeId >= 0 && highlightNodeId < static_cast<int>(stations.size())) {
      highlightedStation = stations[static_cast<std::size_t>(highlightNodeId)];
    }

    writeClampedLine(term, row, x + kFrameOffset, contentWidth, "Stations");
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
      writeClampedLine(term, row, x + kFrameOffset, contentWidth, "Routes");
      ++row;
    }

    for (const std::shared_ptr<mapState::Road>& road : roads) {
      if (row >= endRow) {
        break;
      }
      const bool emphasizeOwner = highlightCurrentPlayer && road->getOwner() != nullptr;
      std::ostringstream line;
      line << (emphasizeOwner ? "*" : " ") << road->getStationA()->getName()
           << "-" << road->getStationB()->getName()
           << " len:" << road->getLength()
           << " " << colorCardToString(road->getColor());
      if (road->getOwner() != nullptr) {
        line << " [" << road->getOwner()->getName() << "]";
      }
      writeClampedLine(term, row, x + kFrameOffset, contentWidth, line.str());
      ++row;
    }
    return;
  }

  if (highlightNodeId < 0 || highlightNodeId >= static_cast<int>(layoutEntries.size())) {
    highlightNodeId = 0;
  }

  const StationLayout& selected = layoutEntries[static_cast<std::size_t>(highlightNodeId)];
  const int labelWidth = static_cast<int>(selected.label.size()) + 2;

  const int maxScrollRow = std::max(layoutMinRow, layoutMaxRow - contentRows + 1);
  const int maxScrollCol = std::max(layoutMinCol, layoutMaxCol - mapWidth + labelWidth);

  int desiredScrollX = scrollX;
  int desiredScrollY = scrollY;
  const int visibleRow = selected.row - desiredScrollY;
  const int visibleCol = selected.col - desiredScrollX;

  if (visibleRow < kScrollMargin) {
    desiredScrollY = selected.row - kScrollMargin;
  } else if (visibleRow >= contentRows - kScrollMargin) {
    desiredScrollY = selected.row - (contentRows - kScrollMargin - 1);
  }
  if (visibleCol < kScrollMargin) {
    desiredScrollX = selected.col - kScrollMargin;
  } else if (visibleCol + labelWidth > mapWidth - kScrollMargin) {
    desiredScrollX = selected.col - (mapWidth - kScrollMargin - labelWidth);
  }

  scrollY = clampInt(desiredScrollY, layoutMinRow, maxScrollRow);
  scrollX = clampInt(desiredScrollX, layoutMinCol, maxScrollCol);

  for (std::size_t i = 0; i < layoutEntries.size(); ++i) {
    const StationLayout& entry = layoutEntries[i];
    const int drawRow = row + (entry.row - scrollY);
    const int drawCol = x + kFrameOffset + (entry.col - scrollX);
    if (drawRow < row || drawRow >= endRow) {
      continue;
    }
    if (drawCol < x + kFrameOffset || drawCol >= x + kFrameOffset + mapWidth) {
      continue;
    }

    std::string label = "[" + entry.label + "]";
    if (drawCol + static_cast<int>(label.size()) > x + kFrameOffset + mapWidth) {
      label = label.substr(0, x + kFrameOffset + mapWidth - drawCol);
    }

    if (static_cast<int>(i) == highlightNodeId) {
      term.setBg(Color::BrightWhite);
      term.setFg(Color::Black);
    } else {
      term.setBg(bgColor);
      term.setFg(fgColor);
    }

    term.moveTo(drawRow, drawCol);
    term.write(label);
  }

  if (detailsWidth > 0) {
    int detailsRow = row;
    writeClampedLine(term, detailsRow, detailsX, detailsWidth, "Station Details");
    ++detailsRow;

    std::ostringstream selectedLine;
    selectedLine << "Selected: [" << selected.label << "] " << selected.name;
    writeClampedLine(term, detailsRow, detailsX, detailsWidth, selectedLine.str());
    detailsRow += 2;

    writeClampedLine(term, detailsRow, detailsX, detailsWidth, "Roads:");
    ++detailsRow;

    const std::vector<std::shared_ptr<mapState::Road>> roads = mapState->getRoads();
    for (std::size_t i = 0; i < roads.size() && detailsRow < endRow; ++i) {
      const std::shared_ptr<mapState::Road>& road = roads[i];
      if (!road || !road->getStationA() || !road->getStationB()) {
        continue;
      }
      const std::string a = road->getStationA()->getName();
      const std::string b = road->getStationB()->getName();
      if (a != selected.name && b != selected.name) {
        continue;
      }

      std::string otherName = (a == selected.name) ? b : a;
      int layoutIndex = findLayoutIndexByName(otherName);
      std::string otherLabel = (layoutIndex >= 0)
          ? layoutEntries[static_cast<std::size_t>(layoutIndex)].label
          : toUpperShort(otherName);

      const std::string ownerTag =
          (road->getOwner() != nullptr) ? road->getOwner()->getName() : "X";

      std::ostringstream line;
      line << "- " << otherLabel << " " << road->getLength() << " "
           << roadTypeToken(road) << " "
           << colorCardToString(road->getColor()) << " " << ownerTag;

      term.setFg(roadColorToAnsi(road->getColor()));
      writeClampedLine(term, detailsRow, detailsX, detailsWidth, line.str());
      term.setFg(fgColor);
      ++detailsRow;
    }

    if (detailsRow < endRow) {
      ++detailsRow;
    }
    if (detailsRow < endRow) {
      writeClampedLine(term, detailsRow, detailsX, detailsWidth, "Legend:");
      ++detailsRow;
    }
    if (detailsRow < endRow) {
      writeClampedLine(term, detailsRow, detailsX, detailsWidth, "R=road T=tunnel F=ferry");
      ++detailsRow;
    }
    if (detailsRow < endRow) {
      writeClampedLine(term, detailsRow, detailsX, detailsWidth, "X=unclaimed");
    }
  }
}

std::string GameView::colorCardToString(mapState::RoadColor color) {
  switch (color) {
    case mapState::RoadColor::RED: return "RED";
    case mapState::RoadColor::BLUE: return "BLU";
    case mapState::RoadColor::GREEN: return "GRN";
    case mapState::RoadColor::WHITE: return "WHT";
    case mapState::RoadColor::BLACK: return "BLK";
    case mapState::RoadColor::YELLOW: return "YEL";
    case mapState::RoadColor::PINK: return "PNK";
    case mapState::RoadColor::ORANGE: return "ORG";
    case mapState::RoadColor::NONE:
    case mapState::RoadColor::UNKNOWN:
    default:
      return "NON";
  }
}

void GameView::writeClampedLine(
    Terminal& term,
    int row,
    int col,
    int width,
    const std::string& text) {
  if (width <= 0) {
    return;
  }
  term.moveTo(row, col);
  if (static_cast<int>(text.size()) >= width) {
    term.write(text.substr(0, static_cast<std::size_t>(width)));
  } else {
    term.write(text);
    term.writeRepeat(' ', width - static_cast<int>(text.size()));
  }
}

bool GameView::loadLayout(const std::string& path) {
  layoutEntries.clear();
  layoutLoaded = false;
  layoutMinRow = 0;
  layoutMaxRow = 0;
  layoutMinCol = 0;
  layoutMaxCol = 0;

  std::ifstream file(path.c_str());
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  bool firstEntry = true;
  while (std::getline(file, line)) {
    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#') {
      continue;
    }

    std::istringstream stream(trimmed);
    std::string name;
    std::string label;
    std::string rowStr;
    std::string colStr;

    if (!std::getline(stream, name, ',')) {
      continue;
    }
    if (!std::getline(stream, label, ',')) {
      continue;
    }
    if (!std::getline(stream, rowStr, ',')) {
      continue;
    }
    if (!std::getline(stream, colStr)) {
      continue;
    }

    StationLayout entry;
    entry.name = trim(name);
    entry.label = trim(label);
    entry.row = std::atoi(trim(rowStr).c_str());
    entry.col = std::atoi(trim(colStr).c_str());
    layoutEntries.push_back(entry);

    if (firstEntry) {
      layoutMinRow = entry.row;
      layoutMaxRow = entry.row;
      layoutMinCol = entry.col;
      layoutMaxCol = entry.col;
      firstEntry = false;
    } else {
      layoutMinRow = std::min(layoutMinRow, entry.row);
      layoutMaxRow = std::max(layoutMaxRow, entry.row);
      layoutMinCol = std::min(layoutMinCol, entry.col);
      layoutMaxCol = std::max(layoutMaxCol, entry.col);
    }
  }

  layoutLoaded = !layoutEntries.empty();
  return layoutLoaded;
}

int GameView::findLayoutIndexByName(const std::string& name) {
  for (std::size_t i = 0; i < layoutEntries.size(); ++i) {
    if (layoutEntries[i].name == name) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

}  // namespace tui
