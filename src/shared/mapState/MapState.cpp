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

#define DEBUG_MODE true
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
        this->display();
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
        mapState.fillMapWithInfos(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos, mapState.gameGraph);
        return mapState;
    }
    // Test Map
    MapState MapState::Test()
    {
        DEBUG_PRINT("Test MapState creation started ...");
        MapState mapState;
        std::shared_ptr<playersState::Player> stationOwnerE = std::make_shared<playersState::Player>("PlayerE", playersState::PlayerColor::BLUE, 1, 0, 0, 0, nullptr);
        std::shared_ptr<playersState::Player> stationOwnerH = std::make_shared<playersState::Player>("PlayerH", playersState::PlayerColor::BLACK, 1, 0, 0, 0, nullptr);
        std::shared_ptr<playersState::Player> roadOwnerAB_1 = std::make_shared<playersState::Player>("PlayerAB_1", playersState::PlayerColor::RED, 1, 0, 0, 0, nullptr);
        std::shared_ptr<playersState::Player> roadOwnerBC_1_n_FI = std::make_shared<playersState::Player>("PlayerBC_1", playersState::PlayerColor::GREEN, 1, 0, 0, 0, nullptr);
        std::shared_ptr<playersState::Player> roadOwnerFG = std::make_shared<playersState::Player>("PlayerFG", playersState::PlayerColor::YELLOW, 1, 0, 0, 0, nullptr);
        std::vector<StationInfo> stationsInfos = {
            Station::initData("A"),
            Station::initData("B"),
            Station::initData("C"),
            Station::initData("D"),
            Station::genData(stationOwnerE, "E"),
            Station::initData("F"),
            Station::initData("G"),
            Station::genData(stationOwnerH, "H"),
            Station::initData("I")
        };
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
        DEBUG_PRINT("MapState fillMapWithInfos finished !");
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

    bool MapState::isRoadClaimable(int nbPLayers, std::shared_ptr<mapState::Road> road, std::shared_ptr<playersState::Player> player)
    {
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
