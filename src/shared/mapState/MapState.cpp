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
#include <type_traits>
#include <memory>

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
    using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int>;
    using RoadInfo = std::pair<StationPair, RoadDetail>;
    using TunnelDetail = RoadDetail;
    using TunnelInfo = RoadInfo;
    using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, int, int>;
    using FerryInfo = std::pair<StationPair, FerryDetail>;

    MapState::MapState()
    {
        DEBUG_PRINT("default MapState creation started...");
        std::shared_ptr<boost::adjacency_list<>> gameGraph = std::make_shared<boost::adjacency_list<>>();
        this->namedMapState("default", gameGraph);
#ifdef DEBUG
        this->display();
#endif
        DEBUG_PRINT("Mapstate creation finished !");
    };

    MapState::MapState(std::shared_ptr<boost::adjacency_list<>> gameGraph, std::vector<StationInfo> stationsInfos, std::vector<RoadInfo> roadsInfos, std::vector<TunnelInfo> tunnelsInfos, std::vector<FerryInfo> ferrysInfos)
    {
        DEBUG_PRINT("Parameterized MapState creation started...");
        this->_MapState(gameGraph, stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos);
#ifdef DEBUG
        this->display();
#endif
        DEBUG_PRINT("Mapstate creation finished !");
    };
    void MapState::_MapState(std::shared_ptr<boost::adjacency_list<>> gameGraph, std::vector<StationInfo> stationsInfos, std::vector<RoadInfo> roadsInfos, std::vector<TunnelInfo> tunnelsInfos, std::vector<FerryInfo> ferrysInfos)
    {

        DEBUG_PRINT("MapState _MapState started ...");
        this->gameGraph = gameGraph;

        std::vector<std::shared_ptr<Station>> stationObjects = Station::BatchConstructor(stationsInfos, gameGraph);
        
        std::vector<std::shared_ptr<Road>> roadObjects = Road::BatchConstructor(roadsInfos, this->gameGraph);

        std::vector<std::shared_ptr<Tunnel>> tunnelObjects = Tunnel::BatchConstructor(tunnelsInfos, this->gameGraph);

        std::vector<std::shared_ptr<Ferry>> ferryObjects = Ferry::BatchConstructor(ferrysInfos, this->gameGraph);
        this->stations = stationObjects;
        this->roads = roadObjects;
        this->roads.insert(this->roads.end(), tunnelObjects.begin(), tunnelObjects.end());
        this->roads.insert(this->roads.end(), ferryObjects.begin(), ferryObjects.end());
        DEBUG_PRINT("MapState _MapState finished !");
    }

    void MapState::namedMapState(std::string mapName, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {
        DEBUG_PRINT("Named MapState creation started...");
        std::vector<StationInfo> stationsInfos;
        std::vector<std::shared_ptr<Station>> stationsObject;
        std::vector<RoadInfo> roadsInfos;
        std::vector<TunnelInfo> tunnelsInfos;
        std::vector<FerryInfo> ferrysInfos;
        if (mapName == "default")
        {
            stationsInfos = {
                Station::initData("paris"),
                Station::initData("berlin"),
                Station::initData("madrid"),
                Station::initData("rome"),
            };
            stationsObject = Station::BatchConstructor(stationsInfos, gameGraph);
            roadsInfos = {
                Road::initData(
                    stationsObject[0],
                    stationsObject[1],
                    1,
                    cardsState::ColorCard::RED,
                    2),
                Road::initData(
                    stationsObject[2],
                    stationsObject[3],
                    4,
                    cardsState::ColorCard::GREEN,
                    6),
            };
            tunnelsInfos = {
                Tunnel::initData(
                    stationsObject[0],
                    stationsObject[3],
                    3,
                    cardsState::ColorCard::BLACK,
                    4),

            };
            ferrysInfos = {
                Ferry::initData(
                    stationsObject[1],
                    stationsObject[3],
                    2,
                    3,
                    4),
            };
        }
        else if (mapName == "empty")
        {
            *this = MapState::Empty();
            return;
        }
        else if (mapName == "europe")
        {
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
            for (const std::string &name : stationNames)
            {
                stationsInfos.push_back(Station::initData(name));
            }
            stationsObject = Station::BatchConstructor(stationsInfos, gameGraph);
            roadsInfos = {
                Road::initDataByName(stationsObject, "lisboa", "madrid", 1, cardsState::ColorCard::PINK, 3),
                Road::initDataByName(stationsObject, "lisboa", "cadiz", 2, cardsState::ColorCard::BLUE, 2),
                Road::initDataByName(stationsObject, "cadiz", "madrid", 3, cardsState::ColorCard::ORANGE, 3),
                Road::initDataByName(stationsObject, "madrid", "barcelona", 4, cardsState::ColorCard::YELLOW, 2),
                Road::initDataByName(stationsObject, "barcelona", "marseille", 5, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "marseille", "paris", 6, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "marseille", "pamplona", 7, cardsState::ColorCard::RED, 4),
                Road::initDataByName(stationsObject, "pamplona", "paris", 8, cardsState::ColorCard::GREEN, 4),
                Road::initDataByName(stationsObject, "pamplona", "paris", 9, cardsState::ColorCard::BLUE, 4),
                Road::initDataByName(stationsObject, "pamplona", "brest", 10, cardsState::ColorCard::PINK, 4),
                Road::initDataByName(stationsObject, "brest", "dieppe", 11, cardsState::ColorCard::ORANGE, 2),
                Road::initDataByName(stationsObject, "brest", "paris", 12, cardsState::ColorCard::BLACK, 3),
                Road::initDataByName(stationsObject, "paris", "frankfurt", 13, cardsState::ColorCard::WHITE, 3),
                Road::initDataByName(stationsObject, "paris", "frankfurt", 14, cardsState::ColorCard::ORANGE, 3),
                Road::initDataByName(stationsObject, "paris", "bruxelles", 15, cardsState::ColorCard::YELLOW, 2),
                Road::initDataByName(stationsObject, "paris", "bruxelles", 16, cardsState::ColorCard::RED, 2),
                Road::initDataByName(stationsObject, "paris", "dieppe", 17, cardsState::ColorCard::PINK, 1),
                Road::initDataByName(stationsObject, "dieppe", "bruxelles", 18, cardsState::ColorCard::GREEN, 2),
                Road::initDataByName(stationsObject, "bruxelles", "frankfurt", 19, cardsState::ColorCard::BLUE, 2),
                Road::initDataByName(stationsObject, "bruxelles", "amsterdam", 20, cardsState::ColorCard::BLACK, 1),
                Road::initDataByName(stationsObject, "amsterdam", "frankfurt", 21, cardsState::ColorCard::WHITE, 2),
                Road::initDataByName(stationsObject, "amsterdam", "essen", 22, cardsState::ColorCard::YELLOW, 3),
                Road::initDataByName(stationsObject, "essen", "berlin", 23, cardsState::ColorCard::BLUE, 2),
                Road::initDataByName(stationsObject, "essen", "frankfurt", 24, cardsState::ColorCard::GREEN, 2),
                Road::initDataByName(stationsObject, "frankfurt", "berlin", 25, cardsState::ColorCard::BLACK, 3),
                Road::initDataByName(stationsObject, "frankfurt", "berlin", 26, cardsState::ColorCard::RED, 3),
                Road::initDataByName(stationsObject, "frankfurt", "münchen", 27, cardsState::ColorCard::PINK, 2),
                Road::initDataByName(stationsObject, "münchen", "wien", 28, cardsState::ColorCard::ORANGE, 3),
                Road::initDataByName(stationsObject, "wien", "zagrab", 29, cardsState::ColorCard::NONE, 2),
                Road::initDataByName(stationsObject, "wien", "budapest", 30, cardsState::ColorCard::RED, 1),
                Road::initDataByName(stationsObject, "wien", "budapest", 31, cardsState::ColorCard::WHITE, 1),
                Road::initDataByName(stationsObject, "wien", "berlin", 32, cardsState::ColorCard::GREEN, 3),
                Road::initDataByName(stationsObject, "wien", "warszawa", 33, cardsState::ColorCard::BLUE, 4),
                Road::initDataByName(stationsObject, "budapest", "sarajevo", 34, cardsState::ColorCard::PINK, 3),
                Road::initDataByName(stationsObject, "budapest", "zagrab", 35, cardsState::ColorCard::ORANGE, 2),
                Road::initDataByName(stationsObject, "zagrab", "sarajevo", 36, cardsState::ColorCard::RED, 3),
                Road::initDataByName(stationsObject, "zagrab", "venizia", 37, cardsState::ColorCard::NONE, 2),
                Road::initDataByName(stationsObject, "venizia", "roma", 38, cardsState::ColorCard::BLACK, 2),
                Road::initDataByName(stationsObject, "roma", "brindisi", 39, cardsState::ColorCard::WHITE, 2),
                Road::initDataByName(stationsObject, "sarajevo", "athina", 40, cardsState::ColorCard::GREEN, 4),
                Road::initDataByName(stationsObject, "athina", "sofia", 41, cardsState::ColorCard::PINK, 3),
                Road::initDataByName(stationsObject, "sofia", "constantinople", 42, cardsState::ColorCard::BLUE, 3),
                Road::initDataByName(stationsObject, "constantinople", "bucuresti", 43, cardsState::ColorCard::YELLOW, 3),
                Road::initDataByName(stationsObject, "bucuresti", "kyiv", 44, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "bucuresti", "sevastopol", 45, cardsState::ColorCard::WHITE, 4),
                Road::initDataByName(stationsObject, "sevastopol", "rostov", 46, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "rostov", "sochi", 47, cardsState::ColorCard::NONE, 2),
                Road::initDataByName(stationsObject, "rostov", "kharkov", 48, cardsState::ColorCard::GREEN, 2),
                Road::initDataByName(stationsObject, "kharkov", "kyiv", 49, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "kharkov", "moskva", 50, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "moskva", "smolensk", 51, cardsState::ColorCard::ORANGE, 2),
                Road::initDataByName(stationsObject, "moskva", "petrograd", 52, cardsState::ColorCard::WHITE, 4),
                Road::initDataByName(stationsObject, "petrograd", "wilno", 53, cardsState::ColorCard::BLUE, 4),
                Road::initDataByName(stationsObject, "petrograd", "riga", 54, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "wilno", "riga", 55, cardsState::ColorCard::GREEN, 4),
                Road::initDataByName(stationsObject, "wilno", "warszawa", 56, cardsState::ColorCard::RED, 3),
                Road::initDataByName(stationsObject, "wilno", "moskva", 57, cardsState::ColorCard::YELLOW, 3),
                Road::initDataByName(stationsObject, "wilno", "kyiv", 58, cardsState::ColorCard::NONE, 2),
                Road::initDataByName(stationsObject, "kyiv", "smolensk", 59, cardsState::ColorCard::RED, 3),
                Road::initDataByName(stationsObject, "kyiv", "warszawa", 60, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "warszawa", "berlin", 61, cardsState::ColorCard::PINK, 4),
                Road::initDataByName(stationsObject, "warszawa", "berlin", 62, cardsState::ColorCard::YELLOW, 4),
                Road::initDataByName(stationsObject, "warszawa", "danzig", 63, cardsState::ColorCard::NONE, 2),
                Road::initDataByName(stationsObject, "danzig", "berlin", 64, cardsState::ColorCard::NONE, 4),
                Road::initDataByName(stationsObject, "danzig", "riga", 65, cardsState::ColorCard::BLACK, 3),
                Road::initDataByName(stationsObject, "erzurum", "angora", 66, cardsState::ColorCard::BLACK, 3),
                Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 67, cardsState::ColorCard::WHITE, 3),
                Road::initDataByName(stationsObject, "stockholm", "kobenhavn", 68, cardsState::ColorCard::YELLOW, 3),
                Road::initDataByName(stationsObject, "edinburgh", "london", 69, cardsState::ColorCard::BLACK, 4),
                Road::initDataByName(stationsObject, "edinburgh", "london", 70, cardsState::ColorCard::ORANGE, 4),
            };
            tunnelsInfos = {
                Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 71, cardsState::ColorCard::BLACK, 3),
                Tunnel::initDataByName(stationsObject, "madrid", "pamplona", 72, cardsState::ColorCard::WHITE, 3),
                Tunnel::initDataByName(stationsObject, "pamplona", "barcelona", 73, cardsState::ColorCard::NONE, 2),
                Tunnel::initDataByName(stationsObject, "zürich", "paris", 74, cardsState::ColorCard::NONE, 3),
                Tunnel::initDataByName(stationsObject, "zürich", "münchen", 75, cardsState::ColorCard::YELLOW, 2),
                Tunnel::initDataByName(stationsObject, "zürich", "marseille", 76, cardsState::ColorCard::PINK, 2),
                Tunnel::initDataByName(stationsObject, "marseille", "roma", 77, cardsState::ColorCard::NONE, 4),
                Tunnel::initDataByName(stationsObject, "zürich", "venizia", 78, cardsState::ColorCard::GREEN, 2),
                Tunnel::initDataByName(stationsObject, "venizia", "münchen", 79, cardsState::ColorCard::BLUE, 2),
                Tunnel::initDataByName(stationsObject, "petrograd", "stockholm", 80, cardsState::ColorCard::NONE, 8),
                Tunnel::initDataByName(stationsObject, "budapest", "kyiv", 81, cardsState::ColorCard::NONE, 6),
                Tunnel::initDataByName(stationsObject, "budapest", "bucuresti", 82, cardsState::ColorCard::NONE, 4),
                Tunnel::initDataByName(stationsObject, "sofia", "bucuresti", 83, cardsState::ColorCard::NONE, 2),
                Tunnel::initDataByName(stationsObject, "sofia", "sarajevo", 84, cardsState::ColorCard::NONE, 2),
                Tunnel::initDataByName(stationsObject, "constantinople", "smyrna", 85, cardsState::ColorCard::NONE, 2),
                Tunnel::initDataByName(stationsObject, "smyrna", "angora", 86, cardsState::ColorCard::ORANGE, 3),
                Tunnel::initDataByName(stationsObject, "angora", "constantinople", 87, cardsState::ColorCard::NONE, 2),
            };
            ferrysInfos = {
                Ferry::initDataByName(stationsObject, "london", "dieppe", 88, 1, 2),
                Ferry::initDataByName(stationsObject, "london", "dieppe", 89, 1, 2),
                Ferry::initDataByName(stationsObject, "london", "amsterdam", 90, 2, 2),
                Ferry::initDataByName(stationsObject, "berlin", "kobenhavn", 91, 1, 3),
                Ferry::initDataByName(stationsObject, "berlin", "kobenhavn", 92, 1, 3),
                Ferry::initDataByName(stationsObject, "sevastopol", "sochi", 93, 1, 2),
                Ferry::initDataByName(stationsObject, "sevastopol", "erzurum", 94, 2, 4),
                Ferry::initDataByName(stationsObject, "sevastopol", "constantinople", 95, 2, 4),
                Ferry::initDataByName(stationsObject, "athina", "smyrna", 96, 1, 2),
                Ferry::initDataByName(stationsObject, "athina", "brindisi", 97, 1, 4),
                Ferry::initDataByName(stationsObject, "palermo", "roma", 98, 1, 4),
                Ferry::initDataByName(stationsObject, "palermo", "brindisi", 99, 1, 3),
                Ferry::initDataByName(stationsObject, "palermo", "smyrna", 100, 2, 6),

            };
        }
        else
        {
            DEBUG_PRINT("Unknown map name: " << mapName << ". Creating empty MapState.");
            *this = MapState::Empty();
            return;
        }
        this->_MapState(gameGraph, stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos);
    }

    MapState MapState::Empty()
    {
        DEBUG_PRINT("Empty MapState creation started ...");
        MapState mapState = MapState();
        mapState.stations.clear();
        mapState.roads.clear();
        mapState.gameGraph = std::make_shared<boost::adjacency_list<>>();
        DEBUG_PRINT("Empty MapState creation finished !");
        return mapState;
    }

    MapState::~MapState()
    {
        DEBUG_PRINT("MapState Destroyed");
    }

    void MapState::display() const
    {
        std::cout << "===== MAP STATE =====\n";
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
        std::cout << "=====================\n";
    }

    std::vector<std::shared_ptr<Station>> MapState::getStations() const
    {
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

    bool MapState::isRoadClaimable(int nbPLayers, std::shared_ptr<mapState::Road> road, std::shared_ptr<playersState::Player> player){
        return road->isClaimable(this->roads, nbPLayers, player);
    }

    std::vector<std::shared_ptr<Road>> MapState::getClaimableRoads(int nbPlayers, std::shared_ptr<playersState::Player> player)
    {
        return Road::getClaimableRoads(this->roads, nbPlayers, player);
    }

    // FIXME : Make the algorithm more clear + make the test WORK
    Path MapState::findShortestPath(std::shared_ptr<Station> src, std::shared_ptr<Station> dest)
    {
        Path path;

        if (!src || !dest || !this->gameGraph)
            return path;

        boost::adjacency_list<> &g = *this->gameGraph;
        using vertex_descriptor =
            boost::graph_traits<boost::adjacency_list<>>::vertex_descriptor;
        using edge_descriptor =
            boost::graph_traits<boost::adjacency_list<>>::edge_descriptor;

        std::size_t n = boost::num_vertices(g);

        std::vector<int> distances(n, std::numeric_limits<int>::max());
        std::vector<vertex_descriptor> predecessors(n);

        auto weightMap =
            boost::make_static_property_map<edge_descriptor>(1);

        boost::dijkstra_shortest_paths(
            g,
            *(src->vertex.get()),
            boost::predecessor_map(
                boost::make_iterator_property_map(
                    predecessors.begin(),
                    boost::get(boost::vertex_index, g)))
                .distance_map(
                    boost::make_iterator_property_map(
                        distances.begin(),
                        boost::get(boost::vertex_index, g)))
                .weight_map(weightMap));

        auto vertexToStation =
            [this](vertex_descriptor v) -> std::shared_ptr<Station>
        {
            for (const std::shared_ptr<Station> &s : this->stations)
            {
                if (*(s->vertex.get()) == v)
                    return s;
            }
            return nullptr;
        };

        vertex_descriptor current = *(dest->vertex.get());
        if (distances[current] == std::numeric_limits<int>::max())
            return path;

        while (true)
        {
            std::shared_ptr<Station> s = vertexToStation(current);
            if (s)
                path.STATIONS.push_back(s);

            if (current == *(src->vertex.get()))
                break;

            vertex_descriptor pred = predecessors[current];
            if (pred == current)
                break;

            current = pred;
        }

        std::reverse(path.STATIONS.begin(), path.STATIONS.end());
        path.NUMEDGES = int(path.STATIONS.size()) - 1;
        path.TOTALLENGTH = path.NUMEDGES;
        return path;
    }

    std::vector<std::shared_ptr<Station>> MapState::getAdjacentStations(std::shared_ptr<Station> station)
    {
        if (!station)
        {
            return {};
        }
        return station->getAdjacentStations(this->roads);
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

};
