#include "GameView.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <json/json.h>
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
const char kDefaultLayoutPath[] = "static/europe_state.json";
// Map layout sizing.
const int kMinMapWidth = 16;
const int kMinDetailsWidth = 26;
const int kMaxDetailsWidth = 40;
const int kDetailsWidthExtra = 7;
const int kPanelGap = 2;
const int kScrollMargin = 1;
const char kPanelSeparatorChar = '|';
// When no player is available.
const char kNoPlayerText[] = "No player data";
const char kNoCardText[] = "No card data";
const int kCardBlockWidth = 3;
const int kCardBlockGap = 1;
const int kCardLabelWidth = 3;
const int kPlayerColorBlockWidth = 3;
const int kPlayerColumnGap = 2;
const int kMinPlayerColumnWidth = 18;
const int kDefaultLocalPlayerIndex = 0;
const int kTableColumnCount = 6;
const int kTableMinTypeWidth = 3;
const int kTableMinEndpointsWidth = 5;
const int kTableMinLengthWidth = 2;
const int kTableMinColorWidth = 3;
const int kTableMinOwnerWidth = 3;
const int kTableMinLocsWidth = 4;

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

Color playerColorToAnsi(playersState::PlayerColor color) {
  switch (color) {
    case playersState::PlayerColor::RED: return Color::BrightRed;
    case playersState::PlayerColor::BLUE: return Color::BrightBlue;
    case playersState::PlayerColor::GREEN: return Color::BrightGreen;
    case playersState::PlayerColor::BLACK: return Color::BrightBlack;
    case playersState::PlayerColor::YELLOW: return Color::BrightYellow;
    case playersState::PlayerColor::UNKNOWN:
    default:
      return Color::Default;
  }
}

std::string playerColorLabel(playersState::PlayerColor color) {
  switch (color) {
    case playersState::PlayerColor::RED: return "RED";
    case playersState::PlayerColor::BLUE: return "BLU";
    case playersState::PlayerColor::GREEN: return "GRN";
    case playersState::PlayerColor::BLACK: return "BLK";
    case playersState::PlayerColor::YELLOW: return "YEL";
    case playersState::PlayerColor::UNKNOWN:
    default:
      return "UNK";
  }
}

Color playerLabelBg(playersState::PlayerColor color) {
  return playerColorToAnsi(color);
}

Color playerLabelFg(playersState::PlayerColor color) {
  if (color == playersState::PlayerColor::YELLOW) {
    return Color::Black;
  }
  return Color::BrightWhite;
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

std::string roadTypeName(const std::shared_ptr<mapState::Road>& road) {
  if (!road) {
    return "Road";
  }
  if (std::dynamic_pointer_cast<mapState::Tunnel>(road)) {
    return "Tunnel";
  }
  if (std::dynamic_pointer_cast<mapState::Ferry>(road)) {
    return "Ferry";
  }
  return "Road";
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

std::string wagonColorLabel(cardsState::ColorCard color) {
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

Color wagonLabelBg(cardsState::ColorCard color) {
  if (color == cardsState::ColorCard::BLACK) {
    return Color::Black;
  }
  if (color == cardsState::ColorCard::WHITE) {
    return Color::White;
  }
  if (color == cardsState::ColorCard::LOCOMOTIVE) {
    return Color::BrightMagenta;
  }
  return wagonColorToAnsi(color);
}

Color wagonLabelFg(cardsState::ColorCard color) {
  if (color == cardsState::ColorCard::WHITE) {
    return Color::Black;
  }
  return Color::BrightWhite;
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

std::string toLowerText(const std::string& text) {
  std::string out;
  out.reserve(text.size());
  for (std::size_t i = 0; i < text.size(); ++i) {
    char ch = text[i];
    out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  }
  return out;
}

std::string stationNameOrPlaceholder(const std::shared_ptr<mapState::Station>& station) {
  if (station) {
    return station->getName();
  }
  return "Unknown";
}

std::shared_ptr<mapState::Station> findStationByName(
    const std::vector<std::shared_ptr<mapState::Station>>& stations,
    const std::string& name) {
  for (std::size_t i = 0; i < stations.size(); ++i) {
    if (stations[i] && stations[i]->getName() == name) {
      return stations[i];
    }
  }
  return std::shared_ptr<mapState::Station>();
}

int findPlayerIndex(
    const std::vector<std::shared_ptr<playersState::Player>>& players,
    const std::shared_ptr<playersState::Player>& player) {
  for (std::size_t i = 0; i < players.size(); ++i) {
    if (players[i] == player) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

std::string playerShortLabel(int index) {
  std::ostringstream out;
  out << "PLYR" << (index + 1);
  return out.str();
}

std::string playerDisplayLabel(
    const std::shared_ptr<playersState::Player>& player,
    const std::vector<std::shared_ptr<playersState::Player>>* players,
    int localPlayerIndex,
    bool showLocalMarker,
    bool useCompactLabels) {
  if (!player || !players) {
    return "PLYR?";
  }
  if (!useCompactLabels) {
    return player->getName();
  }
  int index = findPlayerIndex(*players, player);
  if (index < 0) {
    return "PLYR?";
  }
  if (showLocalMarker && index == localPlayerIndex) {
    return "YOU";
  }
  return playerShortLabel(index);
}

std::string playerHeaderLabel(
    const std::shared_ptr<playersState::Player>& player,
    const std::vector<std::shared_ptr<playersState::Player>>* players,
    int localPlayerIndex,
    bool showLocalMarker,
    bool compactLabels) {
  if (!player || !players) {
    return "PLYR?";
  }
  int index = findPlayerIndex(*players, player);
  if (compactLabels) {
    if (showLocalMarker && index == localPlayerIndex) {
      return "YOU";
    }
    return playerShortLabel(index);
  }
  return player->getName();
}

std::string padCell(const std::string& text, int width) {
  if (width <= 0) {
    return std::string();
  }
  if (static_cast<int>(text.size()) >= width) {
    return text.substr(0, static_cast<std::size_t>(width));
  }
  return text + std::string(static_cast<std::size_t>(width - text.size()), ' ');
}

std::string buildTableRow(
    const std::vector<std::string>& cells,
    const std::vector<int>& widths) {
  std::ostringstream out;
  out << "|";
  for (std::size_t i = 0; i < cells.size() && i < widths.size(); ++i) {
    out << padCell(cells[i], widths[i]) << "|";
  }
  return out.str();
}

std::string buildTableSeparator(const std::vector<int>& widths) {
  std::ostringstream out;
  out << "|";
  for (std::size_t i = 0; i < widths.size(); ++i) {
    out << std::string(static_cast<std::size_t>(widths[i]), '-') << "|";
  }
  return out.str();
}

std::string buildTableFooter(const std::vector<int>& widths) {
  std::ostringstream out;
  out << "|";
  for (std::size_t i = 0; i < widths.size(); ++i) {
    out << std::string(static_cast<std::size_t>(widths[i]), '-') << "|";
  }
  return out.str();
}

void drawWagonBlocks(
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
    if (cursor + kCardBlockWidth > width) {
      break;
    }
    std::shared_ptr<cardsState::WagonCard> card = cards[i];
    if (!card) {
      continue;
    }

    term.moveTo(row, col + cursor);
    const cardsState::ColorCard color = card->getColor();
    std::string label = wagonColorLabel(color);
    if (label.size() < static_cast<std::size_t>(kCardBlockWidth)) {
      label.append(static_cast<std::size_t>(kCardBlockWidth) - label.size(), ' ');
    } else if (label.size() > static_cast<std::size_t>(kCardBlockWidth)) {
      label = label.substr(0, static_cast<std::size_t>(kCardBlockWidth));
    }
    if (color == cardsState::ColorCard::LOCOMOTIVE) {
      term.setFg(Color::Black);
      term.setBg(Color::BrightMagenta);
      term.write(label.substr(0, 1));
      term.setBg(Color::BrightCyan);
      term.write(label.substr(1, 1));
      term.setBg(Color::BrightMagenta);
      term.write(label.substr(2, 1));
    } else {
      term.setBg(wagonLabelBg(color));
      term.setFg(wagonLabelFg(color));
      term.write(label);
    }
    term.setBg(bg);
    term.setFg(fg);

    cursor += kCardBlockWidth;
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
      compactOtherPlayers(false),
      showLocalPlayerMarker(false),
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
      layoutEntries(),
      searchQuery(),
      playerScrollIndex(0),
      localPlayerIndex(kDefaultLocalPlayerIndex) {
  requestRedraw();
}

void GameView::setMapState(mapState::MapState* state) {
  if (mapState == state) {
    return;
  }
  mapState = state;
  searchQuery.clear();
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
  playerScrollIndex = 0;
  if (playerState) {
    int playerCount = static_cast<int>(playerState->players.size());
    if (playerCount <= 0) {
      localPlayerIndex = kDefaultLocalPlayerIndex;
    } else if (localPlayerIndex < 0 || localPlayerIndex >= playerCount) {
      localPlayerIndex = kDefaultLocalPlayerIndex;
    }
  } else {
    localPlayerIndex = kDefaultLocalPlayerIndex;
  }
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
  if (!searchQuery.empty()) {
    searchQuery.clear();
  }
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

void GameView::setLocalPlayerIndex(int index) {
  if (localPlayerIndex == index) {
    return;
  }
  localPlayerIndex = index;
  playerScrollIndex = 0;
  requestRedraw();
}

void GameView::setCompactOtherPlayers(bool enabled) {
  if (compactOtherPlayers == enabled) {
    return;
  }
  compactOtherPlayers = enabled;
  requestRedraw();
}

void GameView::setShowLocalPlayerMarker(bool enabled) {
  if (showLocalPlayerMarker == enabled) {
    return;
  }
  showLocalPlayerMarker = enabled;
  requestRedraw();
}

void GameView::moveSelection(int deltaRow, int deltaCol) {
  if (mode == ViewMode::PLAYER) {
    if (!playerState || playerState->players.empty()) {
      return;
    }
    const int playerCount = static_cast<int>(playerState->players.size());
    if (playerCount <= 1) {
      return;
    }
    int maxPlayers = (width - kFrameInset) / kMinPlayerColumnWidth;
    if (maxPlayers < 1) {
      maxPlayers = 1;
    }
    int shownPlayers = playerCount;
    if (shownPlayers > maxPlayers) {
      shownPlayers = maxPlayers;
    }
    if (shownPlayers <= 1) {
      return;
    }
    int otherCount = playerCount - 1;
    int visibleOthers = shownPlayers - 1;
    if (visibleOthers <= 0 || otherCount <= visibleOthers) {
      return;
    }
    int maxScroll = otherCount - visibleOthers;
    if (deltaCol != 0) {
      int nextIndex = playerScrollIndex + ((deltaCol > 0) ? 1 : -1);
      if (nextIndex < 0) {
        nextIndex = 0;
      }
      if (nextIndex > maxScroll) {
        nextIndex = maxScroll;
      }
      if (nextIndex != playerScrollIndex) {
        playerScrollIndex = nextIndex;
        requestRedraw();
      }
    }
    return;
  }

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

void GameView::setSearchQuery(const std::string& query) {
  std::string normalized = toLowerText(trim(query));
  if (searchQuery == normalized) {
    return;
  }
  searchQuery = normalized;
  if (!searchQuery.empty()) {
    int matchIndex = findUniqueMatchIndex(searchQuery);
    if (matchIndex >= 0) {
      highlightNodeId = matchIndex;
    }
  }
  requestRedraw();
}

void GameView::appendSearchChar(char ch) {
  if (std::isalnum(static_cast<unsigned char>(ch)) == 0) {
    return;
  }
  std::string next = searchQuery;
  next.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  setSearchQuery(next);
}

void GameView::backspaceSearch() {
  if (searchQuery.empty()) {
    return;
  }
  std::string next = searchQuery;
  next.pop_back();
  setSearchQuery(next);
}

void GameView::clearSearchQuery() {
  if (searchQuery.empty()) {
    return;
  }
  searchQuery.clear();
  requestRedraw();
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
    for (int r = 0; r < contentRows; ++r) {
      writeClampedLine(term, row + r, x + kFrameOffset, contentWidth, "");
    }
    if (!playerState || playerState->players.empty()) {
      writeClampedLine(term, row, x + kFrameOffset, contentWidth, kNoPlayerText);
      return;
    }

    int playerCount = static_cast<int>(playerState->players.size());
    int maxPlayers = contentWidth / kMinPlayerColumnWidth;
    if (maxPlayers < 1) {
      maxPlayers = 1;
    }
    int shownPlayers = playerCount;
    if (shownPlayers > maxPlayers) {
      shownPlayers = maxPlayers;
    }

    int totalGap = kPlayerColumnGap * (shownPlayers - 1);
    int columnWidth = (contentWidth - totalGap) / shownPlayers;
    if (columnWidth < 1) {
      columnWidth = contentWidth;
    }

    const std::vector<std::shared_ptr<playersState::Player>>& playersRef =
        playerState->players;
    std::vector<std::shared_ptr<playersState::Player>> displayPlayers;
    int localIndex = localPlayerIndex;
    if (localIndex < 0 || localIndex >= playerCount) {
      localIndex = 0;
    }
    displayPlayers.push_back(playersRef[static_cast<std::size_t>(localIndex)]);

    std::vector<std::shared_ptr<playersState::Player>> otherPlayers;
    for (int i = 0; i < playerCount; ++i) {
      if (i == localIndex) {
        continue;
      }
      otherPlayers.push_back(playersRef[static_cast<std::size_t>(i)]);
    }
    const int visibleOthers = shownPlayers - 1;
    int maxScroll = 0;
    if (visibleOthers > 0 && static_cast<int>(otherPlayers.size()) > visibleOthers) {
      maxScroll = static_cast<int>(otherPlayers.size()) - visibleOthers;
    }
    if (playerScrollIndex < 0) {
      playerScrollIndex = 0;
    }
    if (playerScrollIndex > maxScroll) {
      playerScrollIndex = maxScroll;
    }
    for (int i = 0; i < visibleOthers; ++i) {
      int index = playerScrollIndex + i;
      if (index >= 0 && index < static_cast<int>(otherPlayers.size())) {
        displayPlayers.push_back(otherPlayers[static_cast<std::size_t>(index)]);
      }
    }

    for (int i = 0; i < static_cast<int>(displayPlayers.size()); ++i) {
      std::shared_ptr<playersState::Player> player = displayPlayers[static_cast<std::size_t>(i)];
      int col = x + kFrameOffset + i * (columnWidth + kPlayerColumnGap);
      int currentRow = row;

      if (!player) {
        std::ostringstream placeholder;
        placeholder << "PLYR" << (i + 1);
        writeClampedLine(term, currentRow, col, columnWidth, placeholder.str());
        continue;
      }

      const std::vector<std::shared_ptr<playersState::Player>>* players =
          playerState ? &playerState->players : nullptr;
      const std::string playerLabel = playerHeaderLabel(
          player, players, localIndex, showLocalPlayerMarker, compactOtherPlayers);
      {
        const playersState::PlayerColor color = player->getColor();
        std::string label = playerColorLabel(color);
        if (label.size() < static_cast<std::size_t>(kPlayerColorBlockWidth)) {
          label.append(static_cast<std::size_t>(kPlayerColorBlockWidth) - label.size(), ' ');
        } else if (label.size() > static_cast<std::size_t>(kPlayerColorBlockWidth)) {
          label = label.substr(0, static_cast<std::size_t>(kPlayerColorBlockWidth));
        }
        term.setBg(playerLabelBg(color));
        term.setFg(playerLabelFg(color));
        term.moveTo(currentRow, col);
        term.write(label);
      }
      term.setBg(bgColor);
      term.setFg(fgColor);
      writeClampedLine(term, currentRow, col + kPlayerColorBlockWidth,
                       columnWidth - kPlayerColorBlockWidth, " " + playerLabel);
      ++currentRow;

      const bool isLocal = (player == displayPlayers[0]);
      const bool showFullDetails = (!compactOtherPlayers) || isLocal;

      if (currentRow >= endRow) {
        continue;
      }
      std::ostringstream scoreLine;
      scoreLine << "Score: " << player->getScore();
      writeClampedLine(term, currentRow, col, columnWidth, scoreLine.str());
      ++currentRow;

      if (currentRow >= endRow) {
        continue;
      }
      std::ostringstream wagonsLine;
      wagonsLine << "Wagons: " << player->getNbWagons();
      writeClampedLine(term, currentRow, col, columnWidth, wagonsLine.str());
      ++currentRow;

      if (currentRow >= endRow) {
        continue;
      }
      std::ostringstream stationsLine;
      stationsLine << "Stations: " << player->getNbStations();
      writeClampedLine(term, currentRow, col, columnWidth, stationsLine.str());
      ++currentRow;

      if (!showFullDetails || currentRow >= endRow) {
        continue;
      }
      writeClampedLine(term, currentRow, col, columnWidth, "Hand:");
      ++currentRow;

      std::shared_ptr<cardsState::PlayerCards> hand = player->getHand();
      if (currentRow < endRow && hand && hand->wagonCards) {
        drawWagonBlocks(term, currentRow, col, columnWidth, hand->wagonCards->cards,
                        fgColor, bgColor);
        ++currentRow;
      }

      if (currentRow >= endRow) {
        continue;
      }
      writeClampedLine(term, currentRow, col, columnWidth, "Destinations:");
      ++currentRow;

      if (hand && hand->destinationCards) {
        const std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards =
            hand->destinationCards->cards;
        for (std::size_t j = 0; j < destCards.size() && currentRow < endRow; ++j) {
          if (!destCards[j]) {
            continue;
          }
          std::shared_ptr<mapState::Station> stationA = destCards[j]->getstationA();
          std::shared_ptr<mapState::Station> stationB = destCards[j]->getstationB();
          const std::string a = stationNameOrPlaceholder(stationA);
          const std::string b = stationNameOrPlaceholder(stationB);
          bool completed = false;
          for (std::size_t k = 0; k < player->completedDestinations.size(); ++k) {
            std::shared_ptr<cardsState::DestinationCard> completedCard =
                player->completedDestinations[k];
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
          writeClampedLine(term, currentRow, col, columnWidth, lineText);
          int markerOffset = static_cast<int>(lineText.size()) - 1;
          if (markerOffset >= 0 && markerOffset < columnWidth) {
            term.setBg(bgColor);
            term.setFg(completed ? Color::BrightGreen : Color::BrightRed);
            term.moveTo(currentRow, col + markerOffset);
            term.write(completed ? "V" : "X");
            term.setFg(fgColor);
          }
          ++currentRow;
        }
      }
    }
    return;
  }

  if (mode == ViewMode::CARDS) {
    for (int r = 0; r < contentRows; ++r) {
      writeClampedLine(term, row + r, x + kFrameOffset, contentWidth, "");
    }
    if (!cardState) {
      writeClampedLine(term, row, x + kFrameOffset, contentWidth, kNoCardText);
      return;
    }

    int currentRow = row;
    writeClampedLine(term, currentRow, x + kFrameOffset, contentWidth, "Shared Wagon Deck");
    ++currentRow;

    if (cardState->gameWagonCards) {
      int faceDownCount = 0;
      int trashCount = 0;
      if (cardState->gameWagonCards->faceDownCards) {
        faceDownCount =
            static_cast<int>(cardState->gameWagonCards->faceDownCards->cards.size());
      }
      if (cardState->gameWagonCards->trash) {
        trashCount = static_cast<int>(cardState->gameWagonCards->trash->cards.size());
      }
      std::ostringstream wagonInfo;
      wagonInfo << "Face down: " << faceDownCount << " | Trash: " << trashCount;
      writeClampedLine(term, currentRow, x + kFrameOffset, contentWidth, wagonInfo.str());
      ++currentRow;

      if (cardState->gameWagonCards->faceUpCards && currentRow < endRow) {
        writeClampedLine(term, currentRow, x + kFrameOffset, contentWidth, "Face up:");
        ++currentRow;
        if (currentRow < endRow) {
          drawWagonBlocks(term, currentRow, x + kFrameOffset, contentWidth,
                          cardState->gameWagonCards->faceUpCards->cards, fgColor, bgColor);
          ++currentRow;
        }
      }
    }

    if (currentRow < endRow) {
      ++currentRow;
    }
    if (currentRow < endRow) {
      writeClampedLine(term, currentRow, x + kFrameOffset, contentWidth, "Destination Deck");
      ++currentRow;
    }

    if (cardState->gameDestinationCards && currentRow < endRow) {
      int destFaceDown = 0;
      int destTrash = 0;
      int outOfGame = 0;
      if (cardState->gameDestinationCards->faceDownCards) {
        destFaceDown =
            static_cast<int>(cardState->gameDestinationCards->faceDownCards->cards.size());
      }
      if (cardState->gameDestinationCards->trash) {
        destTrash = static_cast<int>(cardState->gameDestinationCards->trash->cards.size());
      }
      if (cardState->outOfGameCards) {
        outOfGame = static_cast<int>(cardState->outOfGameCards->cards.size());
      }
      std::ostringstream destInfo;
      destInfo << "Face down: " << destFaceDown
               << " | Trash: " << destTrash
               << " | Out: " << outOfGame;
      writeClampedLine(term, currentRow, x + kFrameOffset, contentWidth, destInfo.str());
      ++currentRow;
    }

    if (currentRow < endRow) {
      ++currentRow;
    }

    int playerCount = 0;
    if (playerState && !playerState->players.empty()) {
      playerCount = static_cast<int>(playerState->players.size());
    } else if (!cardState->playersCards.empty()) {
      playerCount = static_cast<int>(cardState->playersCards.size());
    }
    if (playerCount == 0) {
      writeClampedLine(term, currentRow, x + kFrameOffset, contentWidth, kNoPlayerText);
      return;
    }

    int maxPlayers = contentWidth / kMinPlayerColumnWidth;
    if (maxPlayers < 1) {
      maxPlayers = 1;
    }
    int shownPlayers = playerCount;
    if (shownPlayers > maxPlayers) {
      shownPlayers = maxPlayers;
    }
    int totalGap = kPlayerColumnGap * (shownPlayers - 1);
    int columnWidth = (contentWidth - totalGap) / shownPlayers;
    if (columnWidth < 1) {
      columnWidth = contentWidth;
    }

    for (int i = 0; i < shownPlayers; ++i) {
      int col = x + kFrameOffset + i * (columnWidth + kPlayerColumnGap);
      int playerRow = currentRow;
      std::shared_ptr<playersState::Player> player;
      if (playerState && i < static_cast<int>(playerState->players.size())) {
        player = playerState->players[i];
      }
      std::shared_ptr<cardsState::PlayerCards> hand;
      if (player) {
        hand = player->getHand();
      } else if (i < static_cast<int>(cardState->playersCards.size())) {
        hand = cardState->playersCards[static_cast<std::size_t>(i)];
      }

      const std::vector<std::shared_ptr<playersState::Player>>* players =
          playerState ? &playerState->players : nullptr;
      int localIndex = localPlayerIndex;
      if (players && (localIndex < 0 || localIndex >= static_cast<int>(players->size()))) {
        localIndex = 0;
      }
      if (player) {
        const playersState::PlayerColor color = player->getColor();
        const std::string playerLabel = playerDisplayLabel(
            player, players, localIndex, showLocalPlayerMarker, compactOtherPlayers);
        std::string label = playerColorLabel(color);
        if (label.size() < static_cast<std::size_t>(kPlayerColorBlockWidth)) {
          label.append(static_cast<std::size_t>(kPlayerColorBlockWidth) - label.size(), ' ');
        } else if (label.size() > static_cast<std::size_t>(kPlayerColorBlockWidth)) {
          label = label.substr(0, static_cast<std::size_t>(kPlayerColorBlockWidth));
        }
        term.setBg(playerLabelBg(color));
        term.setFg(playerLabelFg(color));
        term.moveTo(playerRow, col);
        term.write(label);
        term.setBg(bgColor);
        term.setFg(fgColor);
        writeClampedLine(term, playerRow, col + kPlayerColorBlockWidth,
                         columnWidth - kPlayerColorBlockWidth, " " + playerLabel);
      } else {
        std::ostringstream label;
        label << "PLYR" << (i + 1);
        writeClampedLine(term, playerRow, col, columnWidth, label.str());
      }
      ++playerRow;

      if (playerRow >= endRow) {
        continue;
      }
      writeClampedLine(term, playerRow, col, columnWidth, "Destinations:");
      ++playerRow;

      if (hand && hand->destinationCards) {
        const std::vector<std::shared_ptr<cardsState::DestinationCard>> destCards =
            hand->destinationCards->cards;
        for (std::size_t j = 0; j < destCards.size() && playerRow < endRow; ++j) {
          if (!destCards[j]) {
            continue;
          }
          std::shared_ptr<mapState::Station> stationA = destCards[j]->getstationA();
          std::shared_ptr<mapState::Station> stationB = destCards[j]->getstationB();
          const std::string a = stationNameOrPlaceholder(stationA);
          const std::string b = stationNameOrPlaceholder(stationB);
          writeClampedLine(term, playerRow, col, columnWidth, a + "-" + b);
          ++playerRow;
        }
      }

      if (playerRow >= endRow) {
        continue;
      }
      writeClampedLine(term, playerRow, col, columnWidth, "Hand:");
      ++playerRow;

      if (hand && hand->wagonCards && playerRow < endRow) {
        drawWagonBlocks(term, playerRow, col, columnWidth, hand->wagonCards->cards,
                        fgColor, bgColor);
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

  int detailsWidth = std::min(kMaxDetailsWidth + kDetailsWidthExtra,
                              std::max(kMinDetailsWidth + kDetailsWidthExtra,
                                       contentWidth / 3 + kDetailsWidthExtra));
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
  if (detailsWidth > 0) {
    const int separatorCol = x + kFrameOffset + mapWidth;
    term.setBg(bgColor);
    term.setFg(fgColor);
    for (int r = 0; r < contentRows; ++r) {
      term.moveTo(row + r, separatorCol);
      term.write(std::string(1, kPanelSeparatorChar));
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

    const std::vector<std::shared_ptr<mapState::Station>> stations = mapState->getStations();
    const std::vector<std::shared_ptr<playersState::Player>>* players =
        playerState ? &playerState->players : nullptr;
    int localIndex = localPlayerIndex;
    if (players && (localIndex < 0 || localIndex >= static_cast<int>(players->size()))) {
      localIndex = 0;
    }
    std::shared_ptr<mapState::Station> selectedStation = findStationByName(stations, selected.name);
    std::ostringstream selectedLine;
    selectedLine << "Selected: [" << selected.label << "] " << selected.name;
    if (selectedStation && selectedStation->getOwner() != nullptr) {
      selectedLine << " (Owned by "
                   << playerDisplayLabel(selectedStation->getOwner(), players, localIndex,
                                         showLocalPlayerMarker, compactOtherPlayers)
                   << ")";
    } else {
      selectedLine << " (unowned)";
    }
    writeClampedLine(term, detailsRow, detailsX, detailsWidth, selectedLine.str());
    detailsRow += 2;

    const std::string headerType = "RoadType";
    const std::string headerEndpoints = "Endpoints";
    const std::string headerLength = "Length";
    const std::string headerColor = "Color";
    const std::string headerOwner = "Owner";
    const std::string headerLocs = "Locs";
    std::vector<int> columnWidths;
    columnWidths.push_back(static_cast<int>(headerType.size()));
    columnWidths.push_back(static_cast<int>(headerEndpoints.size()));
    columnWidths.push_back(static_cast<int>(headerLength.size()));
    columnWidths.push_back(static_cast<int>(headerColor.size()));
    columnWidths.push_back(static_cast<int>(headerOwner.size()));
    columnWidths.push_back(static_cast<int>(headerLocs.size()));
    const int separatorCount = kTableColumnCount + 1;
    int totalWidth = separatorCount;
    for (std::size_t i = 0; i < columnWidths.size(); ++i) {
      totalWidth += columnWidths[i];
    }
    const int minWidths[kTableColumnCount] = {
      kTableMinTypeWidth,
      kTableMinEndpointsWidth,
      kTableMinLengthWidth,
      kTableMinColorWidth,
      kTableMinOwnerWidth,
      kTableMinLocsWidth
    };
    const int shrinkOrder[kTableColumnCount] = {1, 4, 5, 0, 3, 2};
    while (totalWidth > detailsWidth) {
      bool reduced = false;
      for (int idx = 0; idx < kTableColumnCount && totalWidth > detailsWidth; ++idx) {
        const int colIndex = shrinkOrder[idx];
        if (columnWidths[colIndex] > minWidths[colIndex]) {
          columnWidths[colIndex] -= 1;
          totalWidth -= 1;
          reduced = true;
        }
      }
      if (!reduced) {
        break;
      }
    }
    if (detailsRow < endRow) {
      std::vector<std::string> headerCells;
      headerCells.push_back(headerType);
      headerCells.push_back(headerEndpoints);
      headerCells.push_back(headerLength);
      headerCells.push_back(headerColor);
      headerCells.push_back(headerOwner);
      headerCells.push_back(headerLocs);
      writeClampedLine(term, detailsRow, detailsX, detailsWidth,
                       buildTableRow(headerCells, columnWidths));
      ++detailsRow;
    }
    if (detailsRow < endRow) {
      writeClampedLine(term, detailsRow, detailsX, detailsWidth,
                       buildTableSeparator(columnWidths));
      ++detailsRow;
    }

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

      std::string ownerTag = "X";
      if (road->getOwner() != nullptr) {
        ownerTag = playerDisplayLabel(road->getOwner(), players, localIndex,
                                      showLocalPlayerMarker, compactOtherPlayers);
      }
      std::string locsText = "NONE";
      if (std::dynamic_pointer_cast<mapState::Ferry>(road)) {
        std::shared_ptr<mapState::Ferry> ferry =
            std::dynamic_pointer_cast<mapState::Ferry>(road);
        if (ferry) {
          locsText = std::to_string(ferry->getLocomotives());
        }
      }

      const std::string endpointText = selected.label + "<->" + otherLabel;
      const std::string lengthText = std::to_string(road->getLength());
      std::vector<std::string> rowCells;
      rowCells.push_back(roadTypeName(road));
      rowCells.push_back(endpointText);
      rowCells.push_back(lengthText);
      rowCells.push_back(colorCardToString(road->getColor()));
      rowCells.push_back(ownerTag);
      rowCells.push_back(locsText);

      term.setFg(roadColorToAnsi(road->getColor()));
      writeClampedLine(term, detailsRow, detailsX, detailsWidth,
                       buildTableRow(rowCells, columnWidths));
      term.setFg(fgColor);
      ++detailsRow;
    }

    if (detailsRow < endRow) {
      writeClampedLine(term, detailsRow, detailsX, detailsWidth,
                       buildTableFooter(columnWidths));
      ++detailsRow;
    }
    if (detailsRow < endRow) {
      writeClampedLine(term, detailsRow, detailsX, detailsWidth, "Legend:");
      ++detailsRow;
    }
    if (detailsRow < endRow) {
      writeClampedLine(term, detailsRow, detailsX, detailsWidth, "    Ownership: X=unclaimed");
      ++detailsRow;
    }
  }

  // Draw the search query on the last line of the map area.
  const int searchRow = endRow - 1;
  if (searchRow >= row) {
    std::string prompt = "Search: " + searchQuery;
    term.setBg(bgColor);
    term.setFg(fgColor);
    writeClampedLine(term, searchRow, x + kFrameOffset, mapWidth, prompt);
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

int GameView::findUniqueMatchIndex(const std::string& query) {
  if (query.empty()) {
    return -1;
  }
  int matchIndex = -1;
  for (std::size_t i = 0; i < layoutEntries.size(); ++i) {
    const std::string& name = layoutEntries[i].name;
    if (name.compare(0, query.size(), query) == 0) {
      if (matchIndex >= 0) {
        return -1;
      }
      matchIndex = static_cast<int>(i);
    }
  }
  return matchIndex;
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

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();
  std::string trimmed = trim(content);
  bool hasJson = false;
  if (!path.empty()) {
    std::string lower = path;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
      return static_cast<char>(std::tolower(c));
    });
    if (lower.size() >= 5 && lower.substr(lower.size() - 5) == ".json") {
      hasJson = true;
    }
  }
  if (!trimmed.empty() && (trimmed[0] == '{' || trimmed[0] == '[')) {
    hasJson = true;
  }

  bool firstEntry = true;
  if (hasJson) {
    Json::Value root;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::string error;
    std::istringstream input(content);
    if (!Json::parseFromStream(builder, input, &root, &error)) {
      return false;
    }
    Json::Value mapNode;
    if (root.isObject() && root.isMember("map")) {
      mapNode = root["map"];
    } else {
      mapNode = root;
    }
    if (!mapNode.isObject() || !mapNode.isMember("stations") || !mapNode["stations"].isArray()) {
      return false;
    }
    const Json::Value& stations = mapNode["stations"];
    for (Json::ArrayIndex i = 0; i < stations.size(); ++i) {
      const Json::Value& station = stations[i];
      if (!station.isObject()) {
        continue;
      }
      if (!station.isMember("name") || !station["name"].isString()) {
        continue;
      }
      if (!station.isMember("TUIrow") || !station.isMember("TUIcolumn")) {
        continue;
      }
      if (!station["TUIrow"].isInt() || !station["TUIcolumn"].isInt()) {
        continue;
      }
      StationLayout entry;
      entry.name = station["name"].asString();
      if (station.isMember("TUIlabel") && station["TUIlabel"].isString()) {
        entry.label = station["TUIlabel"].asString();
      } else {
        entry.label = toUpperShort(entry.name);
      }
      entry.row = station["TUIrow"].asInt();
      entry.col = station["TUIcolumn"].asInt();
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
  } else {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
      std::string trimmedLine = trim(line);
      if (trimmedLine.empty() || trimmedLine[0] == '#') {
        continue;
      }

      std::istringstream rowStream(trimmedLine);
      std::string name;
      std::string label;
      std::string rowStr;
      std::string colStr;

      if (!std::getline(rowStream, name, ',')) {
        continue;
      }
      if (!std::getline(rowStream, label, ',')) {
        continue;
      }
      if (!std::getline(rowStream, rowStr, ',')) {
        continue;
      }
      if (!std::getline(rowStream, colStr)) {
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
