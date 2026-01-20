#include "MapState.h"
#include "Path.h"
#include "cardsState/ColorCard.h"
#include "Station.h"
#include "Road.h"
#include "Tunnel.h"
#include "Ferry.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <memory>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <json/json.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/undirected_graph.hpp>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace mapState
{
    using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;
    using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
    using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, RoadColor, int>;
    using RoadInfo = std::pair<StationPair, RoadDetail>;
    using TunnelDetail = RoadDetail;
    using TunnelInfo = RoadInfo;
    using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, int, int>;
    using FerryInfo = std::pair<StationPair, FerryDetail>;

    namespace {

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

        std::string normalizeName(const std::string& value)
        {
            std::string trimmed = trimString(value);
            std::string lower = trimmed;
            std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            return lower;
        }

        std::string toUpper(const std::string& value)
        {
            std::string out = value;
            std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
                return static_cast<char>(std::toupper(c));
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

        bool findIsolatedStations(
            const std::vector<std::shared_ptr<Station>>& stations,
            const std::vector<std::shared_ptr<Road>>& roads,
            std::vector<std::string>& isolated)
        {
            isolated.clear();
            if (stations.empty())
            {
                return false;
            }

            std::unordered_map<std::string, int> degreeByName;
            for (const std::shared_ptr<Station>& station : stations)
            {
                if (!station)
                {
                    continue;
                }
                const std::string& name = station->getName();
                if (!name.empty())
                {
                    degreeByName[name] = 0;
                }
            }

            for (const std::shared_ptr<Road>& road : roads)
            {
                if (!road || !road->getStationA() || !road->getStationB())
                {
                    continue;
                }
                const std::string& nameA = road->getStationA()->getName();
                const std::string& nameB = road->getStationB()->getName();
                if (!nameA.empty())
                {
                    degreeByName[nameA] += 1;
                }
                if (!nameB.empty())
                {
                    degreeByName[nameB] += 1;
                }
            }

            for (const std::shared_ptr<Station>& station : stations)
            {
                if (!station)
                {
                    continue;
                }
                const std::string& name = station->getName();
                std::unordered_map<std::string, int>::const_iterator it =
                    degreeByName.find(name);
                int degree = 0;
                if (it != degreeByName.end())
                {
                    degree = it->second;
                }
                if (degree == 0)
                {
                    isolated.push_back(name);
                }
            }

            return !isolated.empty();
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

        std::shared_ptr<playersState::Player> resolveOwner(
            const Json::Value& node,
            const std::shared_ptr<playersState::PlayersState>& playersState)
        {
            if (!playersState)
            {
                return std::shared_ptr<playersState::Player>();
            }
            std::string ownerName = readString(node, "owner");
            if (ownerName.empty())
            {
                return std::shared_ptr<playersState::Player>();
            }
            return playersState->getPlayerByName(ownerName);
        }

        void parseRoadEntry(
            const Json::Value& entry,
            const std::vector<std::shared_ptr<mapState::Station>>& stationObjects,
            const std::shared_ptr<playersState::PlayersState>& playersState,
            int& nextId,
            std::vector<RoadInfo>& roadsInfos,
            std::vector<TunnelInfo>& tunnelsInfos,
            std::vector<FerryInfo>& ferrysInfos)
        {
            if (!entry.isObject())
            {
                return;
            }

            std::string fromName = readString(entry, "from");
            if (fromName.empty())
            {
                fromName = readString(entry, "stationA");
            }
            std::string toName = readString(entry, "to");
            if (toName.empty())
            {
                toName = readString(entry, "stationB");
            }
            if (fromName.empty() || toName.empty())
            {
                return;
            }

            int length = readInt(entry, "length", 0);
            bool hasId = entry.isMember("id") && entry["id"].isInt();
            int id = hasId ? entry["id"].asInt() : nextId;
            if (!hasId)
            {
                ++nextId;
            }
            else if (id >= nextId)
            {
                nextId = id + 1;
            }

            std::shared_ptr<playersState::Player> owner = resolveOwner(entry, playersState);
            std::string type = readRoadType(entry);

            if (type == "FERRY")
            {
                int locomotives = readInt(entry, "locomotives", 0);
                if (locomotives == 0)
                {
                    locomotives = readInt(entry, "locos", 0);
                }
                ferrysInfos.push_back(Ferry::genDataByName(
                    stationObjects, normalizeName(fromName), normalizeName(toName),
                    id, owner, locomotives, length));
                return;
            }

            mapState::RoadColor color = mapState::RoadColor::UNKNOWN;
            if (entry.isMember("color"))
            {
                color = parseRoadColorValue(entry["color"]);
            }

            if (type == "TUNNEL")
            {
                tunnelsInfos.push_back(Tunnel::genDataByName(
                    stationObjects, normalizeName(fromName), normalizeName(toName),
                    id, owner, color, length));
                return;
            }

            roadsInfos.push_back(Road::genDataByName(
                stationObjects, normalizeName(fromName), normalizeName(toName),
                id, owner, color, length));
        }

    } // namespace

    MapState::MapState()
    {
        DEBUG_PRINT("default MapState creation started...");
        this->gameGraph = std::make_shared<boost::adjacency_list<>>();
#ifdef DEBUG
        this->display();
#endif
        DEBUG_PRINT("Mapstate creation finished !");
    };
    // MapState from infos
    MapState::MapState(std::vector<StationInfo> stationsInfos, std::vector<RoadInfo> roadsInfos, std::vector<TunnelInfo> tunnelsInfos, std::vector<FerryInfo> ferrysInfos, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {
        DEBUG_PRINT("Parameterized MapState creation started...");
        this->gameGraph = gameGraph;
        this->fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, gameGraph);
#ifdef DEBUG
        std::cout << "Final MapState:" << std::endl;
        this->display();
        std::cout << std::endl;
#endif
        DEBUG_PRINT("Mapstate creation finished !");
    };

    // MapState from named map
    MapState MapState::NamedMapState(std::string mapName)
    {

        if (mapName == "europe")
        {
            return MapState::Europe();
        }
        if (mapName == "test")
        {
            return MapState::Test();
        }
        if (mapName == "empty" || mapName.empty())
        {
            // Return empty mapState
            return MapState();
        }

        return MapState();
    }
    //// Predefined Maps
    // Europe Map
    MapState MapState::Europe()
    {
        MapState mapState;
        std::vector<std::string> stationNames = {
            "amsterdam",
            "angora",
            "athina",
            "barcelona",
            "berlin",
            "brest",
            "brindisi",
            "bruxelles",
            "bucuresti",
            "budapest",
            "cadiz",
            "constantinople",
            "danzig",
            "dieppe",
            "edinburgh",
            "erzurum",
            "essen",
            "frankfurt",
            "kharkov",
            "kobenhavn",
            "kyiv",
            "lisboa",
            "london",
            "madrid",
            "marseille",
            "moskva",
            "münchen",
            "palermo",
            "pamplona",
            "paris",
            "petrograd",
            "riga",
            "roma",
            "rostov",
            "sarajevo",
            "sevastopol",
            "smolensk",
            "smyrna",
            "sochi",
            "sofia",
            "stockholm",
            "venizia",
            "warszawa",
            "wien",
            "wilno",
            "zagrab",
            "zürich"};
        std::vector<StationInfo> stationsInfos;
        for (const std::string &name : stationNames)
        {
            stationsInfos.push_back(Station::initData(name));
        }
        std::vector<std::shared_ptr<Station>> stationsObject = Station::BatchConstructor(stationsInfos, mapState.gameGraph);
        std::vector<RoadInfo> roadsInfos = {
            Road::initDataByName(stationsObject, "lisboa", "madrid", 1, RoadColor::PINK, 3),
            Road::initDataByName(stationsObject, "lisboa", "cadiz", 2, RoadColor::BLUE, 2),
            Road::initDataByName(stationsObject, "cadiz", "madrid", 3, RoadColor::ORANGE, 3),
            Road::initDataByName(stationsObject, "madrid", "barcelona", 4, RoadColor::YELLOW, 2),
            Road::initDataByName(stationsObject, "barcelona", "marseille", 5, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "marseille", "paris", 6, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "marseille", "pamplona", 7, RoadColor::RED, 4),
            Road::initDataByName(stationsObject, "pamplona", "paris", 8, RoadColor::GREEN, 4),
            Road::initDataByName(stationsObject, "pamplona", "paris", 9, RoadColor::BLUE, 4),
            Road::initDataByName(stationsObject, "pamplona", "brest", 10, RoadColor::PINK, 4),
            Road::initDataByName(stationsObject, "brest", "dieppe", 11, RoadColor::ORANGE, 2),
            Road::initDataByName(stationsObject, "brest", "paris", 12, RoadColor::BLACK, 3),
            Road::initDataByName(stationsObject, "paris", "frankfurt", 13, RoadColor::WHITE, 3),
            Road::initDataByName(stationsObject, "paris", "frankfurt", 14, RoadColor::ORANGE, 3),
            Road::initDataByName(stationsObject, "paris", "bruxelles", 15, RoadColor::YELLOW, 2),
            Road::initDataByName(stationsObject, "paris", "bruxelles", 16, RoadColor::RED, 2),
            Road::initDataByName(stationsObject, "paris", "dieppe", 17, RoadColor::PINK, 1),
            Road::initDataByName(stationsObject, "dieppe", "bruxelles", 18, RoadColor::GREEN, 2),
            Road::initDataByName(stationsObject, "bruxelles", "frankfurt", 19, RoadColor::BLUE, 2),
            Road::initDataByName(stationsObject, "bruxelles", "amsterdam", 20, RoadColor::BLACK, 1),
            Road::initDataByName(stationsObject, "amsterdam", "frankfurt", 21, RoadColor::WHITE, 2),
            Road::initDataByName(stationsObject, "amsterdam", "essen", 22, RoadColor::YELLOW, 3),
            Road::initDataByName(stationsObject, "essen", "berlin", 23, RoadColor::BLUE, 2),
            Road::initDataByName(stationsObject, "essen", "frankfurt", 24, RoadColor::GREEN, 2),
            Road::initDataByName(stationsObject, "frankfurt", "berlin", 25, RoadColor::BLACK, 3),
            Road::initDataByName(stationsObject, "frankfurt", "berlin", 26, RoadColor::RED, 3),
            Road::initDataByName(stationsObject, "frankfurt", "münchen", 27, RoadColor::PINK, 2),
            Road::initDataByName(stationsObject, "münchen", "wien", 28, RoadColor::ORANGE, 3),
            Road::initDataByName(stationsObject, "wien", "zagrab", 29, RoadColor::NONE, 2),
            Road::initDataByName(stationsObject, "wien", "budapest", 30, RoadColor::RED, 1),
            Road::initDataByName(stationsObject, "wien", "budapest", 31, RoadColor::WHITE, 1),
            Road::initDataByName(stationsObject, "wien", "berlin", 32, RoadColor::GREEN, 3),
            Road::initDataByName(stationsObject, "wien", "warszawa", 33, RoadColor::BLUE, 4),
            Road::initDataByName(stationsObject, "budapest", "sarajevo", 34, RoadColor::PINK, 3),
            Road::initDataByName(stationsObject, "budapest", "zagrab", 35, RoadColor::ORANGE, 2),
            Road::initDataByName(stationsObject, "zagrab", "sarajevo", 36, RoadColor::RED, 3),
            Road::initDataByName(stationsObject, "zagrab", "venizia", 37, RoadColor::NONE, 2),
            Road::initDataByName(stationsObject, "venizia", "roma", 38, RoadColor::BLACK, 2),
            Road::initDataByName(stationsObject, "roma", "brindisi", 39, RoadColor::WHITE, 2),
            Road::initDataByName(stationsObject, "sarajevo", "athina", 40, RoadColor::GREEN, 4),
            Road::initDataByName(stationsObject, "athina", "sofia", 41, RoadColor::PINK, 3),
            Road::initDataByName(stationsObject, "sofia", "constantinople", 42, RoadColor::BLUE, 3),
            Road::initDataByName(stationsObject, "constantinople", "bucuresti", 43, RoadColor::YELLOW, 3),
            Road::initDataByName(stationsObject, "bucuresti", "kyiv", 44, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "bucuresti", "sevastopol", 45, RoadColor::WHITE, 4),
            Road::initDataByName(stationsObject, "sevastopol", "rostov", 46, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "rostov", "sochi", 47, RoadColor::NONE, 2),
            Road::initDataByName(stationsObject, "rostov", "kharkov", 48, RoadColor::GREEN, 2),
            Road::initDataByName(stationsObject, "kharkov", "kyiv", 49, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "kharkov", "moskva", 50, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "moskva", "smolensk", 51, RoadColor::ORANGE, 2),
            Road::initDataByName(stationsObject, "moskva", "petrograd", 52, RoadColor::WHITE, 4),
            Road::initDataByName(stationsObject, "petrograd", "wilno", 53, RoadColor::BLUE, 4),
            Road::initDataByName(stationsObject, "petrograd", "riga", 54, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "wilno", "riga", 55, RoadColor::GREEN, 4),
            Road::initDataByName(stationsObject, "wilno", "warszawa", 56, RoadColor::RED, 3),
            Road::initDataByName(stationsObject, "wilno", "moskva", 57, RoadColor::YELLOW, 3),
            Road::initDataByName(stationsObject, "wilno", "kyiv", 58, RoadColor::NONE, 2),
            Road::initDataByName(stationsObject, "kyiv", "smolensk", 59, RoadColor::RED, 3),
            Road::initDataByName(stationsObject, "kyiv", "warszawa", 60, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "warszawa", "berlin", 61, RoadColor::PINK, 4),
            Road::initDataByName(stationsObject, "warszawa", "berlin", 62, RoadColor::YELLOW, 4),
            Road::initDataByName(stationsObject, "warszawa", "danzig", 63, RoadColor::NONE, 2),
            Road::initDataByName(stationsObject, "danzig", "berlin", 64, RoadColor::NONE, 4),
            Road::initDataByName(stationsObject, "danzig", "riga", 65, RoadColor::BLACK, 3),
            Road::initDataByName(stationsObject, "erzurum", "angora", 66, RoadColor::BLACK, 3),
            Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 67, RoadColor::WHITE, 3),
            Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 68, RoadColor::YELLOW, 3),
            Road::initDataByName(stationsObject, "edinburgh", "london", 69, RoadColor::BLACK, 4),
            Road::initDataByName(stationsObject, "edinburgh", "london", 70, RoadColor::ORANGE, 4),
        };
        std::vector<TunnelInfo> tunnelsInfos = {
            Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 71, RoadColor::BLACK, 3),
            Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 72, RoadColor::WHITE, 3),
            Tunnel::initDataByName(stationsObject, "pamplona", "barcelona", 73, RoadColor::NONE, 2),
            Tunnel::initDataByName(stationsObject, "zürich", "paris", 74, RoadColor::NONE, 3),
            Tunnel::initDataByName(stationsObject, "zürich", "münchen", 75, RoadColor::YELLOW, 2),
            Tunnel::initDataByName(stationsObject, "zürich", "marseille", 76, RoadColor::PINK, 2),
            Tunnel::initDataByName(stationsObject, "marseille", "roma", 77, RoadColor::NONE, 4),
            Tunnel::initDataByName(stationsObject, "zürich", "venizia", 78, RoadColor::GREEN, 2),
            Tunnel::initDataByName(stationsObject, "venizia", "münchen", 79, RoadColor::BLUE, 2),
            Tunnel::initDataByName(stationsObject, "petrograd", "stockholm", 80, RoadColor::NONE, 8),
            Tunnel::initDataByName(stationsObject, "budapest", "kyiv", 81, RoadColor::NONE, 6),
            Tunnel::initDataByName(stationsObject, "budapest", "bucuresti", 82, RoadColor::NONE, 4),
            Tunnel::initDataByName(stationsObject, "sofia", "bucuresti", 83, RoadColor::NONE, 2),
            Tunnel::initDataByName(stationsObject, "sofia", "sarajevo", 84, RoadColor::NONE, 2),
            Tunnel::initDataByName(stationsObject, "constantinople", "smyrna", 85, RoadColor::NONE, 2),
            Tunnel::initDataByName(stationsObject, "smyrna", "angora", 86, RoadColor::ORANGE, 3),
            Tunnel::initDataByName(stationsObject, "angora", "constantinople", 87, RoadColor::NONE, 2),
        };
        std::vector<FerryInfo> ferrysInfos = {
            Ferry::initDataByName(stationsObject, "london", "dieppe", 88, 1, 2),
            Ferry::initDataByName(stationsObject, "london", "dieppe", 89, 1, 2),
            Ferry::initDataByName(stationsObject, "london", "amsterdam", 90, 2, 2),
            Ferry::initDataByName(stationsObject, "berlin", "essen", 91, 1, 3),
            Ferry::initDataByName(stationsObject, "berlin", "essen", 92, 1, 3),
            Ferry::initDataByName(stationsObject, "sevastopol", "sochi", 93, 1, 2),
            Ferry::initDataByName(stationsObject, "sevastopol", "erzurum", 94, 2, 4),
            Ferry::initDataByName(stationsObject, "sevastopol", "constantinople", 95, 2, 4),
            Ferry::initDataByName(stationsObject, "athina", "smyrna", 96, 1, 2),
            Ferry::initDataByName(stationsObject, "athina", "brindisi", 97, 1, 4),
            Ferry::initDataByName(stationsObject, "palermo", "roma", 98, 1, 4),
            Ferry::initDataByName(stationsObject, "palermo", "brindisi", 99, 1, 3),
            Ferry::initDataByName(stationsObject, "palermo", "smyrna", 100, 2, 6),

        };
        mapState.fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, mapState.gameGraph);
        DEBUG_PRINT("Europe MapState created !");
        return mapState;
    }
    // Test Map
    MapState MapState::Test()
    {
        DEBUG_PRINT("Test MapState creation started ...");
        MapState mapState;
        std::vector<std::shared_ptr<Road>> borrowedRoads;
        std::shared_ptr<playersState::Player> stationOwnerE = std::make_shared<playersState::Player>("PlayerE", playersState::PlayerColor::BLUE, 1, 0, 0, borrowedRoads, nullptr);
        std::shared_ptr<playersState::Player> stationOwnerH = std::make_shared<playersState::Player>("PlayerH", playersState::PlayerColor::BLACK, 1, 0, 0, borrowedRoads, nullptr);
        std::shared_ptr<playersState::Player> roadOwnerAB_1 = std::make_shared<playersState::Player>("PlayerAB_1", playersState::PlayerColor::RED, 1, 0, 0, borrowedRoads, nullptr);
        std::shared_ptr<playersState::Player> roadOwnerBC_1_n_FI = std::make_shared<playersState::Player>("PlayerBC_1", playersState::PlayerColor::GREEN, 1, 0, 0, borrowedRoads, nullptr);
        std::shared_ptr<playersState::Player> roadOwnerFG = std::make_shared<playersState::Player>("PlayerFG", playersState::PlayerColor::YELLOW, 1, 0, 0, borrowedRoads, nullptr);
        std::vector<StationInfo> stationsInfos = {
            Station::initData("A"),
            Station::initData("B"),
            Station::initData("C"),
            Station::initData("D"),
            Station::genData(stationOwnerE, "E"),
            Station::initData("F"),
            Station::initData("G"),
            Station::genData(stationOwnerH, "H"),
            Station::initData("I")};
        std::vector<std::shared_ptr<Station>> stationsObject = Station::BatchConstructor(stationsInfos, mapState.gameGraph);

        std::vector<RoadInfo> roadsInfos = {
            Road::genDataByName(stationsObject, "A", "B", 1, roadOwnerAB_1, RoadColor::RED, 3),
            Road::initDataByName(stationsObject, "A", "B", 2, RoadColor::BLUE, 3),
            Road::genDataByName(stationsObject, "B", "C", 3, roadOwnerBC_1_n_FI, RoadColor::YELLOW, 2),
            Road::initDataByName(stationsObject, "B", "C", 4, RoadColor::GREEN, 2),
            Road::initDataByName(stationsObject, "B", "D", 5, RoadColor::BLACK, 4),
            Road::initDataByName(stationsObject, "B", "D", 6, RoadColor::WHITE, 4),
            Road::initDataByName(stationsObject, "B", "E", 7, RoadColor::PINK, 4),
            Road::initDataByName(stationsObject, "B", "E", 8, RoadColor::ORANGE, 4),
            Road::initDataByName(stationsObject, "B", "F", 9, RoadColor::RED, 5),
            Road::genDataByName(stationsObject, "F", "G", 10, roadOwnerFG, RoadColor::NONE, 2),
            Road::initDataByName(stationsObject, "F", "H", 11, RoadColor::GREEN, 3),
            Road::genDataByName(stationsObject, "F", "I", 12, roadOwnerBC_1_n_FI, RoadColor::RED, 4),
        };
        mapState.fillMapWithInfos(stationsInfos, roadsInfos, {}, {}, mapState.gameGraph);
        DEBUG_PRINT("Test MapState creation finished !");
        return mapState;
    }

    void MapState::fillMapWithInfos(std::vector<StationInfo> stationsInfos, std::vector<RoadInfo> roadsInfos, std::vector<TunnelInfo> tunnelsInfos, std::vector<FerryInfo> ferrysInfos, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {

        DEBUG_PRINT("MapState fillMapWithInfos started ...");
        this->gameGraph = gameGraph;

        std::vector<std::shared_ptr<Station>> stationObjects = Station::BatchConstructor(stationsInfos, this->gameGraph);
        std::vector<std::shared_ptr<Road>> roadObjects = Road::BatchConstructor(roadsInfos, this->gameGraph);
        std::vector<std::shared_ptr<Tunnel>> tunnelObjects = Tunnel::BatchConstructor(tunnelsInfos, this->gameGraph);
        std::vector<std::shared_ptr<Ferry>> ferryObjects = Ferry::BatchConstructor(ferrysInfos, this->gameGraph);
        this->stations = stationObjects;
        this->roads = roadObjects;
        this->roads.insert(this->roads.end(), tunnelObjects.begin(), tunnelObjects.end());
        this->roads.insert(this->roads.end(), ferryObjects.begin(), ferryObjects.end());

        std::vector<std::string> isolated;
        if (findIsolatedStations(this->stations, this->roads, isolated))
        {
            std::cerr << "Error: map has isolated stations: ";
            for (std::size_t i = 0; i < isolated.size(); ++i)
            {
                if (i > 0)
                {
                    std::cerr << ", ";
                }
                std::cerr << isolated[i];
            }
            std::cerr << std::endl;
            std::exit(EXIT_FAILURE);
        }
        DEBUG_PRINT("MapState fillMapWithInfos finished !");
    }

    MapState::~MapState()
    {
        DEBUG_PRINT("MapState Destroyed");
    }

    void MapState::display(int indent) const
    {
        std::string indentation(indent, '\t');
        std::cout << indentation << "~~~~~ MAP STATE ~~~~~" << std::endl;
        std::cout << indentation << "\t" << "##### STATIONS #####" << std::endl;
        for (const std::shared_ptr<Station> &station : stations)
        {
            station->display(indent + 2);
        }
        std::cout << indentation << "\t" << "#####################" << std::endl;
        std::cout << indentation << "\t" << "##### ROADS #####" << std::endl;
        for (const std::shared_ptr<Road> &road : roads)
        {
            if (typeid(*road) == typeid(Tunnel))
            {
                std::shared_ptr<Tunnel> tunnel = std::dynamic_pointer_cast<Tunnel>(road);
                tunnel->display(indent + 2);
            }
            else if (typeid(*road) == typeid(Ferry))
            {
                std::shared_ptr<Ferry> ferry = std::dynamic_pointer_cast<Ferry>(road);
                ferry->display(indent + 2);
            }
            else
            {
                road->display(indent + 2);
            }
        }
        std::cout << indentation << "\t" << "#####################" << std::endl;
        std::cout << indentation << "~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    }

    std::vector<std::shared_ptr<Station>> MapState::getStations() const
    {
        DEBUG_PRINT("MapState getStations called");
        std::vector<std::shared_ptr<Station>> stationsCopy = this->stations;
        DEBUG_PRINT("MapState getStations returning " << stationsCopy.size() << " stations");
        return this->stations;
    }

    std::vector<std::shared_ptr<Road>> MapState::getRoads() const
    {
        return this->roads;
    }

    std::shared_ptr<Station> MapState::getStationByName(const std::string &name)
    {
        return Station::getStationByName(this->stations, name);
    }

    std::vector<std::shared_ptr<Road>> MapState::getRoadsBetweenStations(std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB)
    {
        return Road::getRoadsBetweenStations(this->roads, stationA, stationB);
    }

    bool MapState::isRoadClaimable(int nbPLayers, std::shared_ptr<mapState::Road> road, std::shared_ptr<playersState::Player> player)
    {
        return road->isClaimable(this->roads, nbPLayers, player);
    }

    std::vector<std::shared_ptr<Road>> MapState::getClaimableRoads(int nbPlayers, std::shared_ptr<playersState::Player> player)
    {
        return Road::getClaimableRoads(this->roads, nbPlayers, player);
    }

    // shared helper for pathfinding with Dijkstra algorithm
    Path MapState::buildPathWithDijkstra(
        std::shared_ptr<Station> src,
        std::shared_ptr<Station> dest,
        const std::vector<std::shared_ptr<Station>> &stations,
        const std::vector<std::shared_ptr<Road>> &roads,
        bool useRoadLength)
    {
        Path path;
        path.TOTALLENGTH = 0;
        path.NUMEDGES = 0;

        if (!src || !dest)
            return path;

        // build a temporary undirected graph from provided stations and roads
        typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> TempGraph;
        TempGraph graph;
        typedef boost::graph_traits<TempGraph>::vertex_descriptor vertex_descriptor;
        typedef boost::graph_traits<TempGraph>::edge_descriptor edge_descriptor;

        // make vertices for each station and keep a name->vertex map
        std::unordered_map<std::string, vertex_descriptor> vertexByName;
        std::vector<std::shared_ptr<Station>> vertexStations;
        for (const std::shared_ptr<Station> &station : stations)
        {
            if (!station)
            {
                continue;
            }
            vertex_descriptor v = boost::add_vertex(graph);
            vertexByName[station->getName()] = v;
            std::size_t index = static_cast<std::size_t>(v);
            if (vertexStations.size() <= index)
            {
                vertexStations.resize(index + 1);
            }
            vertexStations[index] = station;
        }

        std::unordered_map<std::string, vertex_descriptor>::const_iterator srcIt =
            vertexByName.find(src->getName());
        std::unordered_map<std::string, vertex_descriptor>::const_iterator destIt =
            vertexByName.find(dest->getName());
        if (srcIt == vertexByName.end() || destIt == vertexByName.end())
        {
            return path;
        }

        // build edges and weights for temp graph from the provided road list
        std::map<edge_descriptor, int> edgeWeights;
        for (const std::shared_ptr<Road> &road : roads)
        {
            if (!road || !road->getStationA() || !road->getStationB())
            {
                continue;
            }
            const std::string &nameA = road->getStationA()->getName();
            const std::string &nameB = road->getStationB()->getName();
            std::unordered_map<std::string, vertex_descriptor>::const_iterator aIt =
                vertexByName.find(nameA);
            std::unordered_map<std::string, vertex_descriptor>::const_iterator bIt =
                vertexByName.find(nameB);
            if (aIt == vertexByName.end() || bIt == vertexByName.end())
            {
                continue;
            }
            std::pair<edge_descriptor, bool> edgeResult =
                boost::add_edge(aIt->second, bIt->second, graph);
            if (edgeResult.second)
            {
                if (useRoadLength)
                {
                    edgeWeights[edgeResult.first] = road->getLength();
                }
                else
                {
                    edgeWeights[edgeResult.first] = 1;
                }
            }
        }

        boost::associative_property_map<std::map<edge_descriptor, int>> weightMap(edgeWeights);

        std::size_t n = boost::num_vertices(graph);
        if (n == 0)
        {
            return path;
        }

        std::vector<int> distances(n, std::numeric_limits<int>::max());
        std::vector<vertex_descriptor> predecessors(n);

        boost::dijkstra_shortest_paths(
            graph,
            srcIt->second,
            boost::predecessor_map(
                boost::make_iterator_property_map(
                    predecessors.begin(),
                    boost::get(boost::vertex_index, graph)))
                .distance_map(
                    boost::make_iterator_property_map(
                        distances.begin(),
                        boost::get(boost::vertex_index, graph)))
                .weight_map(weightMap));

        vertex_descriptor current = destIt->second;
        std::size_t destIndex = static_cast<std::size_t>(current);
        if (destIndex >= distances.size())
        {
            return path;
        }
        if (distances[destIndex] == std::numeric_limits<int>::max())
        {
            return path;
        }

        // reconstruct path using the predecessor chain (roads already traversed)
        while (true)
        {
            std::size_t currentIndex = static_cast<std::size_t>(current);
            if (currentIndex < vertexStations.size() && vertexStations[currentIndex])
            {
                path.STATIONS.push_back(vertexStations[currentIndex]);
            }

            if (current == srcIt->second)
            {
                break;
            }

            vertex_descriptor pred = predecessors[currentIndex];
            if (pred == current)
            {
                break;
            }

            current = pred;
        }

        std::reverse(path.STATIONS.begin(), path.STATIONS.end());
        if (!path.STATIONS.empty())
        {
            path.NUMEDGES = int(path.STATIONS.size()) - 1;
            path.TOTALLENGTH = distances[destIndex];
        }
        return path;
    }

    // get shortest path by number of roads (sets edge weight = 1)
    Path MapState::getShortestPath(
        std::shared_ptr<Station> src,
        std::shared_ptr<Station> dest,
        std::vector<std::shared_ptr<Station>> stations,
        std::vector<std::shared_ptr<Road>> roads)
    {
        return this->buildPathWithDijkstra(src, dest, stations, roads, false);
    }

    // gets most efficient path by road length (edge weight = road length)
    Path MapState::getMostEfficientPath(
        std::shared_ptr<Station> src,
        std::shared_ptr<Station> dest,
        std::vector<std::shared_ptr<Station>> stations,
        std::vector<std::shared_ptr<Road>> roads)
    {
        return this->buildPathWithDijkstra(src, dest, stations, roads, true);
    }

    std::vector<std::shared_ptr<Station>> MapState::getAdjacentStations(std::shared_ptr<Station> station)
    {
        if (!station)
        {
            return {};
        }
        return station->getAdjacentStations(this->roads);
    }
    std::vector<std::shared_ptr<Road>> MapState::getRoadsOwnedByPlayer(std::shared_ptr<playersState::Player> player)
    {
        return Road::getRoadsOwnedByPlayer(player, this->roads);
    }
    std::vector<std::shared_ptr<Road>> MapState::getRoadsUsableByPlayer(std::shared_ptr<playersState::Player> player)
    {
        return Road::getRoadsUsableByPlayer(player, this->roads);
    }
    bool MapState::isDestinationReached(std::vector<std::shared_ptr<Road>> playerRoads, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB)
    {
        if (!stationA || !stationB)
        {
            return false;
        }
        if (stationA->getName() == stationB->getName())
        {
            return true;
        }

        // build an undirected graph from the provided roads
        std::unordered_map<std::string, std::vector<std::string>> adjacency;
        for (const std::shared_ptr<Road> &road : playerRoads)
        {
            if (!road || !road->getStationA() || !road->getStationB())
            {
                continue;
            }
            std::string nameA = road->getStationA()->getName();
            std::string nameB = road->getStationB()->getName();
            adjacency[nameA].push_back(nameB);
            adjacency[nameB].push_back(nameA);
        }

        const std::string startName = stationA->getName();
        const std::string targetName = stationB->getName();
        if (adjacency.find(startName) == adjacency.end() || adjacency.find(targetName) == adjacency.end())
        {
            return false;
        }

        // BFS to check connectivity between the two stations
        // not using boost bfs to avoid need to rebuild graph
        std::queue<std::string> toVisit;
        std::unordered_set<std::string> visited;
        toVisit.push(startName);
        visited.insert(startName);

        while (!toVisit.empty())
        {
            std::string current = toVisit.front();
            toVisit.pop();
            if (current == targetName)
            {
                return true;
            }
            std::unordered_map<std::string, std::vector<std::string>>::const_iterator it = adjacency.find(current);
            if (it == adjacency.end())
            {
                continue;
            }
            const std::vector<std::string> &neighbors = it->second;
            for (std::vector<std::string>::const_iterator neighIt = neighbors.begin();
                 neighIt != neighbors.end();
                 ++neighIt)
            {
                const std::string &neighbor = *neighIt;
                if (visited.insert(neighbor).second)
                {
                    toVisit.push(neighbor);
                }
            }
        }
        return false;
    }

    bool MapState::isDestinationReached(std::shared_ptr<playersState::Player> player, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB)
    {
        std::vector<std::shared_ptr<Road>> playerRoads = this->getRoadsUsableByPlayer(player);

        return MapState::isDestinationReached(playerRoads, stationA, stationB);
    }
    bool MapState::isDestinationReached(std::shared_ptr<playersState::Player> player, std::shared_ptr<cardsState::DestinationCard> destinationCard)
    {
        std::shared_ptr<Station> stationA = destinationCard->getstationA();
        std::shared_ptr<Station> stationB = destinationCard->getstationB();
        // @fetohiaras needs your expertise here :)
        // let's see if it works :^)
        return MapState::isDestinationReached(player, stationA, stationB);
    }
    std::string MapState::toString() const
    {
        std::ostringstream out;
        out << "===== MAP STATE =====\n";
        std::cout << "Stations:\n";
        for (const std::shared_ptr<Station> &station : stations)
        {
            station->display();
            std::cout << "------------------\n";
        }
        std::cout << "Roads:\n";
        for (const std::shared_ptr<Road> &road : roads)
        {
            road->display();
            std::cout << "------------------\n";
        }
        out << "==========================\n";
        return out.str();
    }

    MapState MapState::ParseFromJSON(std::string json, std::shared_ptr<playersState::PlayersState> playersState)
    {
        if (json.empty())
        {
            return MapState();
        }

        Json::Value root;
        std::string error;
        if (!parseRoot(json, root, error))
        {
            return MapState();
        }

        if (root.isString())
        {
            return MapState::NamedMapState(root.asString());
        }

        if (!root.isObject())
        {
            return MapState();
        }

        std::string mapName = readString(root, "mapName");
        if (mapName.empty())
        {
            mapName = readString(root, "name");
        }

        bool hasStations = root.isMember("stations") && root["stations"].isArray();
        bool hasRoads = (root.isMember("roads") && root["roads"].isArray())
                        || (root.isMember("tunnels") && root["tunnels"].isArray())
                        || (root.isMember("ferries") && root["ferries"].isArray());

        if (!mapName.empty() && !hasStations && !hasRoads)
        {
            return MapState::NamedMapState(mapName);
        }

        MapState mapState;
        mapState.gameGraph = std::make_shared<boost::adjacency_list<>>();

        std::vector<StationInfo> stationsInfos;
        if (hasStations)
        {
            for (Json::ArrayIndex i = 0; i < root["stations"].size(); ++i)
            {
                const Json::Value& entry = root["stations"][i];
                std::string name;
                std::shared_ptr<playersState::Player> owner;
                if (entry.isString())
                {
                    name = entry.asString();
                }
                else if (entry.isObject())
                {
                    name = readString(entry, "name");
                    owner = resolveOwner(entry, playersState);
                }
                if (name.empty())
                {
                    continue;
                }
                stationsInfos.push_back(Station::genData(owner, normalizeName(name)));
            }
        }

        if (stationsInfos.empty())
        {
            return mapState;
        }

        std::vector<std::shared_ptr<Station>> stationObjects =
            Station::BatchConstructor(stationsInfos, mapState.gameGraph);

        std::vector<RoadInfo> roadsInfos;
        std::vector<TunnelInfo> tunnelsInfos;
        std::vector<FerryInfo> ferrysInfos;
        int nextId = 1;

        if (root.isMember("roads") && root["roads"].isArray())
        {
            for (Json::ArrayIndex i = 0; i < root["roads"].size(); ++i)
            {
                parseRoadEntry(root["roads"][i], stationObjects, playersState, nextId,
                               roadsInfos, tunnelsInfos, ferrysInfos);
            }
        }
        if (root.isMember("tunnels") && root["tunnels"].isArray())
        {
            for (Json::ArrayIndex i = 0; i < root["tunnels"].size(); ++i)
            {
                Json::Value entry = root["tunnels"][i];
                if (entry.isObject() && !entry.isMember("type"))
                {
                    entry["type"] = "TUNNEL";
                }
                parseRoadEntry(entry, stationObjects, playersState, nextId,
                               roadsInfos, tunnelsInfos, ferrysInfos);
            }
        }
        if (root.isMember("ferries") && root["ferries"].isArray())
        {
            for (Json::ArrayIndex i = 0; i < root["ferries"].size(); ++i)
            {
                Json::Value entry = root["ferries"][i];
                if (entry.isObject() && !entry.isMember("type"))
                {
                    entry["type"] = "FERRY";
                }
                parseRoadEntry(entry, stationObjects, playersState, nextId,
                               roadsInfos, tunnelsInfos, ferrysInfos);
            }
        }

        mapState.fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, mapState.gameGraph);
        return mapState;
    }

}
