#include "CardsState.h"
#include "PlayerCards.h"
#include "SharedDeck.h"
#include "OutOfGame.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <json/json.h>

#include "DestinationCard.h"
#include "WagonCard.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState
{
  class DestinationCard;
  class WagonCard;
  class PlayerCards;
}

namespace cardsState
{
  namespace {

    std::string toUpper(const std::string& value)
    {
      std::string out = value;
      std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
      });
      return out;
    }

    std::string normalizeName(const std::string& value)
    {
      std::string out = value;
      std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
      });
      return out;
    }

    bool parseRoot(const std::string& jsonText, Json::Value& root, std::string& error)
    {
      error.clear();
      Json::CharReaderBuilder builder;
      builder["collectComments"] = false;
      std::istringstream input(jsonText);
      return Json::parseFromStream(builder, input, &root, &error);
    }

    std::string readString(const Json::Value& node, const char* key)
    {
      if (node.isMember(key) && node[key].isString())
      {
        return node[key].asString();
      }
      return std::string();
    }

    int readInt(const Json::Value& node, const char* key, int fallback)
    {
      if (node.isMember(key) && node[key].isInt())
      {
        return node[key].asInt();
      }
      return fallback;
    }

    bool readBool(const Json::Value& node, const char* key, bool fallback)
    {
      if (node.isMember(key) && node[key].isBool())
      {
        return node[key].asBool();
      }
      return fallback;
    }

    cardsState::ColorCard parseCardColorValue(const Json::Value& value)
    {
      if (value.isInt())
      {
        return static_cast<cardsState::ColorCard>(value.asInt());
      }
      if (value.isString())
      {
        std::string token = toUpper(value.asString());
        if (token == "RED") return cardsState::ColorCard::RED;
        if (token == "BLUE") return cardsState::ColorCard::BLUE;
        if (token == "GREEN") return cardsState::ColorCard::GREEN;
        if (token == "BLACK") return cardsState::ColorCard::BLACK;
        if (token == "YELLOW") return cardsState::ColorCard::YELLOW;
        if (token == "ORANGE") return cardsState::ColorCard::ORANGE;
        if (token == "PINK") return cardsState::ColorCard::PINK;
        if (token == "WHITE") return cardsState::ColorCard::WHITE;
        if (token == "LOCOMOTIVE") return cardsState::ColorCard::LOCOMOTIVE;
      }
      return cardsState::ColorCard::UNKNOWN;
    }

    std::vector<std::shared_ptr<DestinationCard>> parseDestinationCardsArray(
        const Json::Value& array,
        const std::shared_ptr<mapState::MapState>& mapState)
    {
      std::vector<std::shared_ptr<DestinationCard>> cards;
      if (!array.isArray() || !mapState)
      {
        return cards;
      }
      for (Json::ArrayIndex i = 0; i < array.size(); ++i)
      {
        const Json::Value& dest = array[i];
        if (!dest.isObject())
        {
          continue;
        }
        std::string from = readString(dest, "stationA");
        if (from.empty())
        {
          from = readString(dest, "from");
        }
        std::string to = readString(dest, "stationB");
        if (to.empty())
        {
          to = readString(dest, "to");
        }
        if (from.empty() || to.empty())
        {
          continue;
        }
        int points = readInt(dest, "points", 0);
        bool isLong = readBool(dest, "isLong", false);
        std::shared_ptr<mapState::Station> stationA = mapState->getStationByName(normalizeName(from));
        std::shared_ptr<mapState::Station> stationB = mapState->getStationByName(normalizeName(to));
        if (!stationA || !stationB)
        {
          continue;
        }
        cards.push_back(std::make_shared<DestinationCard>(stationA, stationB, points, isLong));
      }
      return cards;
    }

    std::vector<std::shared_ptr<WagonCard>> parseWagonCardsArray(const Json::Value& array)
    {
      std::vector<std::shared_ptr<WagonCard>> cards;
      if (!array.isArray())
      {
        return cards;
      }
      for (Json::ArrayIndex i = 0; i < array.size(); ++i)
      {
        const Json::Value& cardValue = array[i];
        cardsState::ColorCard color = cardsState::ColorCard::UNKNOWN;
        if (cardValue.isObject() && cardValue.isMember("color"))
        {
          color = parseCardColorValue(cardValue["color"]);
        }
        else
        {
          color = parseCardColorValue(cardValue);
        }
        cards.push_back(std::make_shared<WagonCard>(color));
      }
      return cards;
    }

  } // namespace

  /// class CardsState -
  CardsState::CardsState()
  {
  }

  CardsState CardsState::Europe(std::vector<std::shared_ptr<mapState::Station>> stations, int nbPlayers)
  {
    CardsState cardsState;
    cardsState.outOfGameCards = std::make_shared<OutOfGame<DestinationCard>>();
    cardsState.gameDestinationCards = std::make_shared<SharedDeck<DestinationCard>>(SharedDeck<DestinationCard>::Europe(stations));
    cardsState.gameWagonCards = std::make_shared<SharedDeck<WagonCard>>(SharedDeck<WagonCard>::Init());
    cardsState.playersCards = PlayerCards::BatchStartHand(cardsState.gameDestinationCards, cardsState.gameWagonCards, nbPlayers);
    cardsState.gameDestinationCards->Setup(cardsState.outOfGameCards);
    cardsState.gameWagonCards->Setup();
    return cardsState;
  }
  // TODO: Parameterized constructor
  void CardsState::display(int indent)
  {
    std::string indentation = std::string(indent, '\t');
    std::cout << indentation << "~~~~~ CARDS STATE ~~~~~\n";
    if (this->gameDestinationCards)
    {
      gameDestinationCards->display(indent + 1);
    }
    if (this->gameWagonCards)
    {
      gameWagonCards->display(indent + 1);
    }
    if (this->outOfGameCards)
    {
      outOfGameCards->display(indent + 1);
    }
    if (!this->playersCards.empty())
    {
      for (const auto &playerCards : this->playersCards)
      {
        if (playerCards)
        {
          playerCards->display(indent + 1);
        }
      }
    }

    std::cout << indentation << "~~~~~~~~~~~~~~~~~~~~~\n";
  }

  CardsState CardsState::ParseFromJSON(std::string json, std::shared_ptr<mapState::MapState> mapState)
  {
    if (json.empty())
    {
      return CardsState();
    }

    Json::Value root;
    std::string error;
    if (!parseRoot(json, root, error))
    {
      return CardsState();
    }

    if (root.isString())
    {
      std::string preset = toUpper(root.asString());
      if (preset == "EUROPE" && mapState)
      {
        return CardsState::Europe(mapState->getStations(), 0);
      }
      return CardsState();
    }

    if (!root.isObject())
    {
      return CardsState();
    }

    std::string preset = readString(root, "preset");
    if (preset.empty())
    {
      preset = readString(root, "name");
    }
    if (preset.empty())
    {
      preset = readString(root, "type");
    }
    if (!preset.empty() && mapState)
    {
      std::string presetToken = toUpper(preset);
      if (presetToken == "EUROPE")
      {
        int playersCount = readInt(root, "playersCount", 0);
        if (playersCount == 0)
        {
          playersCount = readInt(root, "nbPlayers", 0);
        }
        return CardsState::Europe(mapState->getStations(), playersCount);
      }
    }

    CardsState cardsState;
    cardsState.outOfGameCards = std::make_shared<OutOfGame<DestinationCard>>();

    bool hasDeckSections = root.isMember("outOfGame")
                           || root.isMember("faceUp")
                           || root.isMember("faceDown")
                           || root.isMember("playerCards");

    if (hasDeckSections)
    {
      Json::Value empty(Json::arrayValue);
      Json::Value outOfGame = root.isMember("outOfGame") ? root["outOfGame"] : Json::Value(Json::objectValue);
      Json::Value faceUp = root.isMember("faceUp") ? root["faceUp"] : Json::Value(Json::objectValue);
      Json::Value faceDown = root.isMember("faceDown") ? root["faceDown"] : Json::Value(Json::objectValue);

      std::vector<std::shared_ptr<DestinationCard>> outDest =
          parseDestinationCardsArray(outOfGame.isObject() ? outOfGame["destinationCards"] : empty, mapState);
      std::vector<std::shared_ptr<WagonCard>> outWagons =
          parseWagonCardsArray(outOfGame.isObject() ? outOfGame["wagonCards"] : empty);

      std::vector<std::shared_ptr<DestinationCard>> faceUpDest =
          parseDestinationCardsArray(faceUp.isObject() ? faceUp["destinationCards"] : empty, mapState);
      std::vector<std::shared_ptr<WagonCard>> faceUpWagons =
          parseWagonCardsArray(faceUp.isObject() ? faceUp["wagonCards"] : empty);

      std::vector<std::shared_ptr<DestinationCard>> faceDownDest =
          parseDestinationCardsArray(faceDown.isObject() ? faceDown["destinationCards"] : empty, mapState);
      std::vector<std::shared_ptr<WagonCard>> faceDownWagons =
          parseWagonCardsArray(faceDown.isObject() ? faceDown["wagonCards"] : empty);

      cardsState.outOfGameCards = std::make_shared<OutOfGame<DestinationCard>>(outDest);
      cardsState.gameDestinationCards = std::make_shared<SharedDeck<DestinationCard>>(
          std::vector<std::shared_ptr<DestinationCard>>(), faceUpDest, faceDownDest);
      cardsState.gameWagonCards = std::make_shared<SharedDeck<WagonCard>>(
          outWagons, faceUpWagons, faceDownWagons);

      if (root.isMember("playerCards") && root["playerCards"].isArray())
      {
        for (Json::ArrayIndex i = 0; i < root["playerCards"].size(); ++i)
        {
          const Json::Value& entry = root["playerCards"][i];
          if (!entry.isObject())
          {
            continue;
          }
          std::vector<std::shared_ptr<DestinationCard>> playerDestinations =
              parseDestinationCardsArray(entry["destinationCards"], mapState);
          std::vector<std::shared_ptr<WagonCard>> playerWagons =
              parseWagonCardsArray(entry["wagonCards"]);
          std::shared_ptr<PlayerCards> hand =
              std::make_shared<PlayerCards>(playerDestinations, playerWagons);
          cardsState.playersCards.push_back(hand);
        }
      }

      return cardsState;
    }

    std::vector<std::shared_ptr<DestinationCard>> destinationCards;
    if (root.isMember("destinationCards") && root["destinationCards"].isArray() && mapState)
    {
      for (Json::ArrayIndex i = 0; i < root["destinationCards"].size(); ++i)
      {
        const Json::Value& dest = root["destinationCards"][i];
        if (!dest.isObject())
        {
          continue;
        }
        std::string from = readString(dest, "from");
        std::string to = readString(dest, "to");
        int points = readInt(dest, "points", 0);
        bool isLong = readBool(dest, "isLong", false);
        std::shared_ptr<mapState::Station> stationA = mapState->getStationByName(normalizeName(from));
        std::shared_ptr<mapState::Station> stationB = mapState->getStationByName(normalizeName(to));
        if (!stationA || !stationB)
        {
          continue;
        }
        destinationCards.push_back(std::make_shared<DestinationCard>(stationA, stationB, points, isLong));
      }
    }

    std::vector<std::shared_ptr<WagonCard>> wagonCards;
    if (root.isMember("wagonCards") && root["wagonCards"].isArray())
    {
      for (Json::ArrayIndex i = 0; i < root["wagonCards"].size(); ++i)
      {
        const Json::Value& card = root["wagonCards"][i];
        cardsState::ColorCard color = cardsState::ColorCard::UNKNOWN;
        if (card.isObject() && card.isMember("color"))
        {
          color = parseCardColorValue(card["color"]);
        }
        else
        {
          color = parseCardColorValue(card);
        }
        wagonCards.push_back(std::make_shared<WagonCard>(color));
      }
    }

    if (!destinationCards.empty())
    {
      cardsState.gameDestinationCards = std::make_shared<SharedDeck<DestinationCard>>(destinationCards);
    }
    if (!wagonCards.empty())
    {
      cardsState.gameWagonCards = std::make_shared<SharedDeck<WagonCard>>(wagonCards);
    }

    if (root.isMember("playersHands") && root["playersHands"].isArray())
    {
      for (Json::ArrayIndex i = 0; i < root["playersHands"].size(); ++i)
      {
        const Json::Value& entry = root["playersHands"][i];
        if (!entry.isObject())
        {
          continue;
        }
        std::vector<std::shared_ptr<DestinationCard>> playerDestinations;
        std::vector<std::shared_ptr<WagonCard>> playerWagons;

        if (entry.isMember("destinationCards") && entry["destinationCards"].isArray() && mapState)
        {
          for (Json::ArrayIndex d = 0; d < entry["destinationCards"].size(); ++d)
          {
            const Json::Value& dest = entry["destinationCards"][d];
            if (!dest.isObject())
            {
              continue;
            }
            std::string from = readString(dest, "from");
            std::string to = readString(dest, "to");
            int points = readInt(dest, "points", 0);
            bool isLong = readBool(dest, "isLong", false);
            std::shared_ptr<mapState::Station> stationA = mapState->getStationByName(normalizeName(from));
            std::shared_ptr<mapState::Station> stationB = mapState->getStationByName(normalizeName(to));
            if (!stationA || !stationB)
            {
              continue;
            }
            playerDestinations.push_back(std::make_shared<DestinationCard>(stationA, stationB, points, isLong));
          }
        }

        if (entry.isMember("wagonCards") && entry["wagonCards"].isArray())
        {
          for (Json::ArrayIndex w = 0; w < entry["wagonCards"].size(); ++w)
          {
            const Json::Value& card = entry["wagonCards"][w];
            cardsState::ColorCard color = cardsState::ColorCard::UNKNOWN;
            if (card.isObject() && card.isMember("color"))
            {
              color = parseCardColorValue(card["color"]);
            }
            else
            {
              color = parseCardColorValue(card);
            }
            playerWagons.push_back(std::make_shared<WagonCard>(color));
          }
        }

        std::shared_ptr<PlayerCards> hand = std::make_shared<PlayerCards>(playerDestinations, playerWagons);
        cardsState.playersCards.push_back(hand);
      }
    }

    return cardsState;
  }
}
