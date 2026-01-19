#include "PlayersState.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <json/json.h>

#include "mapState/MapState.h"
#include "mapState/Tunnel.h"
#include "mapState/Ferry.h"
#include "mapState/RoadColor.h"
#include "cardsState/ColorCard.h"
// #include <nlohmann/json.hpp>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

using namespace std;

namespace {

const int kMaxPlayers = 5;

std::string trimString(const std::string& value)
{
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])))
    {
        ++start;
    }
    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])))
    {
        --end;
    }
    return value.substr(start, end - start);
}

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
    std::string trimmed = trimString(value);
    std::string lower = trimmed;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return lower;
}

bool exceedsMaxPlayers(std::size_t count)
{
    return count > static_cast<std::size_t>(kMaxPlayers);
}

bool parseIntToken(const std::string& token, int& value)
{
    if (token.empty())
    {
        return false;
    }
    std::size_t start = 0;
    int sign = 1;
    if (token[0] == '-')
    {
        sign = -1;
        start = 1;
        if (start >= token.size())
        {
            return false;
        }
    }
    int result = 0;
    for (std::size_t i = start; i < token.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(token[i])))
        {
            return false;
        }
        result = result * 10 + (token[i] - '0');
    }
    value = result * sign;
    return true;
}

bool parseRoot(const std::string& jsonText, Json::Value& root, std::string& error)
{
    error.clear();
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::istringstream input(jsonText);
    return Json::parseFromStream(builder, input, &root, &error);
}

const Json::Value* resolvePlayersArray(const Json::Value& root)
{
    if (root.isArray())
    {
        return &root;
    }
    if (root.isObject() && root.isMember("players") && root["players"].isArray())
    {
        return &root["players"];
    }
    return nullptr;
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

playersState::PlayerColor parsePlayerColorValue(const Json::Value& value)
{
    if (value.isInt())
    {
        return static_cast<playersState::PlayerColor>(value.asInt());
    }
    if (value.isString())
    {
        std::string token = toUpper(value.asString());
        if (token == "RED") return playersState::PlayerColor::RED;
        if (token == "BLUE") return playersState::PlayerColor::BLUE;
        if (token == "GREEN") return playersState::PlayerColor::GREEN;
        if (token == "BLACK") return playersState::PlayerColor::BLACK;
        if (token == "YELLOW") return playersState::PlayerColor::YELLOW;
    }
    return playersState::PlayerColor::UNKNOWN;
}

mapState::RoadColor parseRoadColorValue(const Json::Value& value)
{
    if (value.isInt())
    {
        return static_cast<mapState::RoadColor>(value.asInt());
    }
    if (value.isString())
    {
        std::string token = toUpper(value.asString());
        if (token == "NONE") return mapState::RoadColor::NONE;
        if (token == "RED") return mapState::RoadColor::RED;
        if (token == "BLUE") return mapState::RoadColor::BLUE;
        if (token == "GREEN") return mapState::RoadColor::GREEN;
        if (token == "BLACK") return mapState::RoadColor::BLACK;
        if (token == "YELLOW") return mapState::RoadColor::YELLOW;
        if (token == "ORANGE") return mapState::RoadColor::ORANGE;
        if (token == "PINK") return mapState::RoadColor::PINK;
        if (token == "WHITE") return mapState::RoadColor::WHITE;
    }
    return mapState::RoadColor::UNKNOWN;
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

std::string readRoadType(const Json::Value& node)
{
    std::string type = readString(node, "type");
    if (type.empty())
    {
        type = readString(node, "roadType");
    }
    if (type.empty())
    {
        return std::string("ROAD");
    }
    return toUpper(type);
}

bool isRoadUsed(const std::vector<std::shared_ptr<mapState::Road>>& usedRoads,
                const std::shared_ptr<mapState::Road>& road)
{
    for (std::size_t i = 0; i < usedRoads.size(); ++i)
    {
        if (usedRoads[i] == road)
        {
            return true;
        }
    }
    return false;
}

std::shared_ptr<mapState::Road> resolveRoadSpec(
    const Json::Value& spec,
    const std::shared_ptr<mapState::MapState>& mapState,
    std::vector<std::shared_ptr<mapState::Road>>& usedRoads)
{
    if (!mapState)
    {
        return nullptr;
    }

    int id = -1;
    bool hasId = false;
    if (spec.isInt())
    {
        id = spec.asInt();
        hasId = true;
    }
    else if (spec.isString())
    {
        if (parseIntToken(spec.asString(), id))
        {
            hasId = true;
        }
    }
    else if (spec.isObject())
    {
        id = readInt(spec, "id", -1);
        if (id >= 0)
        {
            hasId = true;
        }
    }

    if (hasId)
    {
        std::vector<std::shared_ptr<mapState::Road>> roads = mapState->getRoads();
        for (std::size_t i = 0; i < roads.size(); ++i)
        {
            if (roads[i] && roads[i]->getId() == id)
            {
                return roads[i];
            }
        }
        if (!spec.isObject())
        {
            return nullptr;
        }
    }
    else if (!spec.isObject())
    {
        return nullptr;
    }

    std::string fromName = readString(spec, "from");
    if (fromName.empty())
    {
        fromName = readString(spec, "stationA");
    }
    std::string toName = readString(spec, "to");
    if (toName.empty())
    {
        toName = readString(spec, "stationB");
    }
    if (fromName.empty() || toName.empty())
    {
        return nullptr;
    }

    std::shared_ptr<mapState::Station> stationA = mapState->getStationByName(normalizeName(fromName));
    std::shared_ptr<mapState::Station> stationB = mapState->getStationByName(normalizeName(toName));
    if (!stationA || !stationB)
    {
        return nullptr;
    }

    std::string type = readRoadType(spec);
    int length = readInt(spec, "length", -1);
    mapState::RoadColor color = mapState::RoadColor::UNKNOWN;
    if (spec.isMember("color"))
    {
        color = parseRoadColorValue(spec["color"]);
    }
    int locomotives = readInt(spec, "locomotives", -1);
    if (locomotives < 0)
    {
        locomotives = readInt(spec, "locos", -1);
    }
    int slot = readInt(spec, "slot", -1);

    std::vector<std::shared_ptr<mapState::Road>> candidates =
        mapState->getRoadsBetweenStations(stationA, stationB);

    std::vector<std::shared_ptr<mapState::Road>> filtered;
    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        std::shared_ptr<mapState::Road> road = candidates[i];
        bool isTunnel = (std::dynamic_pointer_cast<mapState::Tunnel>(road) != nullptr);
        bool isFerry = (std::dynamic_pointer_cast<mapState::Ferry>(road) != nullptr);

        if (type == "TUNNEL" && !isTunnel)
        {
            continue;
        }
        if (type == "FERRY" && !isFerry)
        {
            continue;
        }
        if (type == "ROAD" && (isTunnel || isFerry))
        {
            continue;
        }

        if (length > 0 && road->getLength() != length)
        {
            continue;
        }

        if (type == "FERRY")
        {
            std::shared_ptr<mapState::Ferry> ferry = std::dynamic_pointer_cast<mapState::Ferry>(road);
            if (ferry && locomotives >= 0 && ferry->getLocomotives() != locomotives)
            {
                continue;
            }
        }
        else if (color != mapState::RoadColor::UNKNOWN && road->getColor() != color)
        {
            continue;
        }

        if (!isRoadUsed(usedRoads, road))
        {
            filtered.push_back(road);
        }
    }

    if (filtered.empty())
    {
        return nullptr;
    }

    if (slot > 0 && slot <= static_cast<int>(filtered.size()))
    {
        return filtered[static_cast<std::size_t>(slot - 1)];
    }

    return filtered[0];
}

} // namespace

namespace playersState
{
    using PlayersInfos = std::tuple<std::string, PlayerColor, int, int, int,std::vector<std::shared_ptr<mapState::Road>>, std::shared_ptr<cardsState::PlayerCards>>;
    using PlayersInitInfos = std::tuple<std::string, PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

    int PlayersState::nbPlayers = 0;

    PlayersState::PlayersState()
    {
    }

    PlayersState::PlayersState(std::vector<std::shared_ptr<Player>> players)
    {
        if (exceedsMaxPlayers(players.size()))
        {
            std::cerr << "Error: too many players (" << players.size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        this->players = players;
        PlayersState::nbPlayers = static_cast<int>(players.size());   
    }

    PlayersState::PlayersState(std::vector<PlayersInfos> infos)
    {
        if (exceedsMaxPlayers(infos.size()))
        {
            std::cerr << "Error: too many players (" << infos.size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        this->players = Player::BatchFromInfos(infos);
        PlayersState::nbPlayers = static_cast<int>(this->players.size());   
    }

    PlayersState PlayersState::InitFromInfos(std::vector<PlayersInitInfos> infos)
    {
        PlayersState state;
        if (exceedsMaxPlayers(infos.size()))
        {
            std::cerr << "Error: too many players (" << infos.size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        state.players = Player::BatchFromInitInfos(infos);
        PlayersState::nbPlayers = static_cast<int>(state.players.size());   
        return state;}

    PlayersState::PlayersState(std::vector<PlayersInitInfos> infos, std::vector<std::shared_ptr<cardsState::PlayerCards>> hands)
    {
        if (exceedsMaxPlayers(infos.size()))
        {
            std::cerr << "Error: too many players (" << infos.size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        this->players = Player::BatchFromInitInfos(infos);
        PlayersState::nbPlayers = static_cast<int>(this->players.size());  
        this->setupPlayersHand(hands);
    }

    std::vector<std::shared_ptr<Player>> PlayersState::getPlayers()
    {
        return this->players;
    }
    void PlayersState::setPlayers(std::vector<std::shared_ptr<Player>> players)
    {
        if (exceedsMaxPlayers(players.size()))
        {
            std::cerr << "Error: too many players (" << players.size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        this->players = players;
        PlayersState::nbPlayers = static_cast<int>(players.size());
    }

    void PlayersState::setupPlayersHand(std::vector<std::shared_ptr<cardsState::PlayerCards>> hands)
    {
        for (size_t i = 0; i < this->players.size(); i++)
        {
            this->players[i]->setHand(hands[i]);
        }
    }

    std::vector<std::shared_ptr<mapState::Road>> PlayersState::getClaimableRoads(std::shared_ptr<mapState::MapState> map, std::shared_ptr<Player> player)
    {
        return map->getClaimableRoads(PlayersState::nbPlayers, player); 
    } 
    bool PlayersState::isRoadClaimable(std::shared_ptr<mapState::MapState> map, std::shared_ptr<mapState::Road> road, std::shared_ptr<Player> player)
    {
        return map->isRoadClaimable(PlayersState::nbPlayers, road, player);
    }
    void PlayersState::display(int indent)
    {
        std::string indentation(indent, '\t');
        std::cout << indentation << "~~~~~~ PLAYERS STATE ~~~~~~\n";
        std::cout << indentation << "\t" << "##### PLAYERS #####\n";
        if (players.empty())
        {
            std::cout << indentation << "\t\t"<< "No players in the game." << std::endl;
        }
        
        for (std::shared_ptr<Player> player : players)
        {
            player->display(indent+2);
        }
        std::cout << indentation << "\t" << "#######################\n";
        std::cout << indentation << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }

    std::shared_ptr<Player> PlayersState::getPlayerByName(std::string name)
    {
        for (const auto &player : this->players)
        {
            if (player->name == name)
            {
                return player;
            }
        }
        return nullptr;
    }

    PlayersState PlayersState::InitFromJSON(std::string json)
    {
        Json::Value root;
        std::string error;
        if (!parseRoot(json, root, error))
        {
            return PlayersState();
        }

        const Json::Value* playersArray = resolvePlayersArray(root);
        if (!playersArray)
        {
            return PlayersState();
        }
        if (exceedsMaxPlayers(playersArray->size()))
        {
            std::cerr << "Error: too many players (" << playersArray->size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::vector<PlayersInfos> infos;
        for (Json::ArrayIndex i = 0; i < playersArray->size(); ++i)
        {
            const Json::Value& entry = (*playersArray)[i];
            if (!entry.isObject())
            {
                continue;
            }

            std::string name = readString(entry, "name");
            if (name.empty())
            {
                continue;
            }

            playersState::PlayerColor color = playersState::PlayerColor::UNKNOWN;
            if (entry.isMember("color"))
            {
                color = parsePlayerColorValue(entry["color"]);
            }

            int score = readInt(entry, "score", Player::startScore);
            int wagons = readInt(entry, "wagons", Player::startNbWagons);
            if (wagons == Player::startNbWagons)
            {
                wagons = readInt(entry, "nbWagons", Player::startNbWagons);
            }
            int stations = readInt(entry, "stations", Player::startNbStations);
            if (stations == Player::startNbStations)
            {
                stations = readInt(entry, "nbStations", Player::startNbStations);
            }

            std::vector<std::shared_ptr<mapState::Road>> borrowedRoads;
            std::shared_ptr<cardsState::PlayerCards> hand;
            infos.push_back(std::make_tuple(name, color, score, wagons, stations, borrowedRoads, hand));
        }

        return PlayersState(infos);
    }
    void PlayersState::setupFromJSON(std::string json, std::shared_ptr<mapState::MapState> mapState, std::shared_ptr<cardsState::CardsState> cardState)
    {
        Json::Value root;
        std::string error;
        if (!parseRoot(json, root, error))
        {
            return;
        }

        const Json::Value* playersArray = resolvePlayersArray(root);
        if (!playersArray)
        {
            return;
        }
        if (exceedsMaxPlayers(playersArray->size()))
        {
            std::cerr << "Error: too many players (" << playersArray->size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::vector<std::shared_ptr<cardsState::PlayerCards>> parsedHands;
        parsedHands.resize(players.size());
        bool parsedLegacyHands = false;

        for (Json::ArrayIndex i = 0; i < playersArray->size(); ++i)
        {
            const Json::Value& entry = (*playersArray)[i];
            if (!entry.isObject())
            {
                continue;
            }

            std::string name = readString(entry, "name");
            std::shared_ptr<Player> player = getPlayerByName(name);
            if (!player && i < players.size())
            {
                player = players[i];
            }
            if (!player)
            {
                continue;
            }

            std::vector<std::shared_ptr<mapState::Road>> usedRoads;

            // Owned roads (legacy format)
            if (entry.isMember("roads") && entry["roads"].isArray())
            {
                for (Json::ArrayIndex r = 0; r < entry["roads"].size(); ++r)
                {
                    std::shared_ptr<mapState::Road> road = resolveRoadSpec(entry["roads"][r], mapState, usedRoads);
                    if (road)
                    {
                        player->borrowedRoads.push_back(road);
                        road->setOwner(player);
                        usedRoads.push_back(road);
                    }
                }
            }
            // Borrowed roads (by id or legacy object format)
            if (entry.isMember("borrowedRoads") && entry["borrowedRoads"].isArray())
            {
                for (Json::ArrayIndex r = 0; r < entry["borrowedRoads"].size(); ++r)
                {
                    std::shared_ptr<mapState::Road> road = resolveRoadSpec(entry["borrowedRoads"][r], mapState, usedRoads);
                    if (road)
                    {
                        player->borrowedRoads.push_back(road);
                        usedRoads.push_back(road);
                    }
                }
            }

            // Stations ownership
            if (entry.isMember("stationsOwned") && entry["stationsOwned"].isArray() && mapState)
            {
                for (Json::ArrayIndex s = 0; s < entry["stationsOwned"].size(); ++s)
                {
                    if (!entry["stationsOwned"][s].isString())
                    {
                        continue;
                    }
                    std::string stationName = normalizeName(entry["stationsOwned"][s].asString());
                    std::shared_ptr<mapState::Station> station = mapState->getStationByName(stationName);
                    if (station)
                    {
                        station->setOwner(player);
                    }
                }
            }

            // Hand setup (new JSON uses hand index into cardsState::playersCards)
            bool handSet = false;
            if (entry.isMember("hand") && entry["hand"].isInt() && cardState)
            {
                int handIndex = entry["hand"].asInt();
                if (handIndex >= 0 && handIndex < static_cast<int>(cardState->playersCards.size()))
                {
                    player->setHand(cardState->playersCards[static_cast<std::size_t>(handIndex)]);
                    handSet = true;

                    if (root.isObject()
                        && root.isMember("cards")
                        && root["cards"].isObject()
                        && root["cards"].isMember("playerCards")
                        && root["cards"]["playerCards"].isArray()
                        && handIndex < static_cast<int>(root["cards"]["playerCards"].size()))
                    {
                        const Json::Value& handEntry = root["cards"]["playerCards"][handIndex];
                        if (handEntry.isObject()
                            && handEntry.isMember("destinationCards")
                            && handEntry["destinationCards"].isArray()
                            && player->getHand()
                            && player->getHand()->destinationCards)
                        {
                            const std::vector<std::shared_ptr<cardsState::DestinationCard>>& handCards =
                                player->getHand()->destinationCards->getCards();
                            for (Json::ArrayIndex d = 0; d < handEntry["destinationCards"].size(); ++d)
                            {
                                const Json::Value& dest = handEntry["destinationCards"][d];
                                if (!dest.isObject())
                                {
                                    continue;
                                }
                                if (!readBool(dest, "completed", false))
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
                                std::string fromKey = normalizeName(from);
                                std::string toKey = normalizeName(to);
                                for (std::size_t c = 0; c < handCards.size(); ++c)
                                {
                                    if (!handCards[c])
                                    {
                                        continue;
                                    }
                                    std::shared_ptr<mapState::Station> stationA =
                                        handCards[c]->getstationA();
                                    std::shared_ptr<mapState::Station> stationB =
                                        handCards[c]->getstationB();
                                    if (!stationA || !stationB)
                                    {
                                        continue;
                                    }
                                    if (normalizeName(stationA->getName()) == fromKey
                                        && normalizeName(stationB->getName()) == toKey)
                                    {
                                        DEBUG_PRINT("Completed destination: " << stationA->getName()
                                                     << " -> " << stationB->getName());
                                        player->completedDestinations.push_back(handCards[c]);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Hand setup (legacy destinationCards/wagonCards arrays)
            std::vector<std::shared_ptr<cardsState::DestinationCard>> destinationCards;
            if (!handSet && entry.isMember("destinationCards") && entry["destinationCards"].isArray() && mapState)
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
                    std::shared_ptr<cardsState::DestinationCard> card =
                        std::make_shared<cardsState::DestinationCard>(stationA, stationB, points, isLong);
                    destinationCards.push_back(card);
                    if (readBool(dest, "completed", false))
                    {
                        player->completedDestinations.push_back(card);
                    }
                }
            }

            std::vector<std::shared_ptr<cardsState::WagonCard>> wagonCards;
            if (!handSet && entry.isMember("wagonCards") && entry["wagonCards"].isArray())
            {
                for (Json::ArrayIndex w = 0; w < entry["wagonCards"].size(); ++w)
                {
                    const Json::Value& cardValue = entry["wagonCards"][w];
                    cardsState::ColorCard color = cardsState::ColorCard::UNKNOWN;
                    if (cardValue.isObject() && cardValue.isMember("color"))
                    {
                        color = parseCardColorValue(cardValue["color"]);
                    }
                    else
                    {
                        color = parseCardColorValue(cardValue);
                    }
                    std::shared_ptr<cardsState::WagonCard> card = std::make_shared<cardsState::WagonCard>(color);
                    wagonCards.push_back(card);
                }
            }

            if (!destinationCards.empty() || !wagonCards.empty())
            {
                std::shared_ptr<cardsState::PlayerCards> hand =
                    std::make_shared<cardsState::PlayerCards>(destinationCards, wagonCards);
                player->setHand(hand);
                if (i < parsedHands.size())
                {
                    parsedHands[i] = hand;
                }
                parsedLegacyHands = true;
            }
        }

        if (cardState && parsedLegacyHands)
        {
            if (cardState->playersCards.size() < parsedHands.size())
            {
                cardState->playersCards.resize(parsedHands.size());
            }
            for (std::size_t i = 0; i < parsedHands.size(); ++i)
            {
                if (parsedHands[i])
                {
                    cardState->playersCards[i] = parsedHands[i];
                }
            }
        }
    }

};
