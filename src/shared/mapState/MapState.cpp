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
    using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, int>;
    using FerryInfo = std::pair<StationPair, FerryDetail>;

    MapState::MapState()
    {
        DEBUG_PRINT("default MapState creation started...");
        this->gameGraph = std::make_shared<boost::adjacency_list<>>();
        std::vector<StationInfo> stations = {
            Station::genData(nullptr, "paris"),
            Station::genData(nullptr, "berlin"),
            Station::genData(nullptr, "madrid"),
            Station::genData(nullptr, "rome"),
        };
        std::vector<std::shared_ptr<Station>> stationsObject = Station::BatchConstructor(stations, this->gameGraph);
        std::vector<RoadInfo> roads = {
            Road::genData(
                stationsObject[0],
                stationsObject[1],
                1,
                nullptr,
                cardsState::ColorCard::RED,
                2),
            Road::genData(
                stationsObject[2],
                stationsObject[3],
                4,
                nullptr,
                cardsState::ColorCard::GREEN,
                6),
        };
        std::vector<TunnelInfo> tunnels = {
            Tunnel::genData(
                stationsObject[0],
                stationsObject[3],
                3,
                nullptr,
                cardsState::ColorCard::BLACK,
                4),

        };
        std::vector<FerryInfo> ferrys = {
            Ferry::genData(
                stationsObject[1],
                stationsObject[3],
                2,
                nullptr,
                cardsState::ColorCard::BLUE,
                3,
                4),
        };
        this->_MapState(this->gameGraph, stations, roads, tunnels, ferrys);
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
        this->stations.clear();
        this->roads.clear();
        if (!gameGraph)
        {
            gameGraph = std::make_shared<boost::adjacency_list<>>();
        }
        this->gameGraph = gameGraph;

        std::vector<std::shared_ptr<Station>> stationObjects = Station::BatchConstructor(stationsInfos, this->gameGraph);
        std::unordered_map<std::string, std::shared_ptr<Station>> stationLookup;
        stationLookup.reserve(stationObjects.size());

        for (const std::shared_ptr<Station> &station : stationObjects)
        {
            this->stations.push_back(station);
            stationLookup[station->getName()] = station;
        }

        auto remapStation =
            [&](const std::shared_ptr<Station> &station) -> std::shared_ptr<Station>
        {
            if (!station)
            {
                return nullptr;
            }

            auto it = stationLookup.find(station->getName());
            if (it == stationLookup.end())
            {
                DEBUG_PRINT("Unknown station: " << station->getName());
                return nullptr;
            }
            return it->second;
        };

        auto remapConnections =
            [&](const auto &infos)
        {
            using VectorType = typename std::decay<decltype(infos)>::type;
            using InfoType = typename VectorType::value_type;
            VectorType remapped;
            remapped.reserve(infos.size());
            for (const auto &info : infos)
            {
                std::shared_ptr<Station> stationA = remapStation(info.first.first);
                std::shared_ptr<Station> stationB = remapStation(info.first.second);
                if (!stationA || !stationB)
                {
                    continue;
                }
                InfoType newInfo = info;
                newInfo.first.first = stationA;
                newInfo.first.second = stationB;
                remapped.push_back(newInfo);
            }
            return remapped;
        };

        std::vector<RoadInfo> normalizedRoadInfos = remapConnections(roadsInfos);
        std::vector<TunnelInfo> normalizedTunnelInfos = remapConnections(tunnelsInfos);
        std::vector<FerryInfo> normalizedFerryInfos = remapConnections(ferrysInfos);

        std::vector<std::shared_ptr<Road>> roadObjects = Road::BatchConstructor(normalizedRoadInfos, this->gameGraph);

        std::vector<std::shared_ptr<Tunnel>> tunnelObjects = Tunnel::BatchConstructor(normalizedTunnelInfos, this->gameGraph);

        std::vector<std::shared_ptr<Ferry>> ferryObjects = Ferry::BatchConstructor(normalizedFerryInfos, this->gameGraph);

        for (const std::shared_ptr<Road> &road : roadObjects)
        {
            this->roads.push_back(road);
        }
        for (const std::shared_ptr<Tunnel> &tunnel : tunnelObjects)
        {
            this->roads.push_back(tunnel);
        }
        for (const std::shared_ptr<Ferry> &ferry : ferryObjects)
        {
            this->roads.push_back(ferry);
        }
        DEBUG_PRINT("MapState _MapState finished !");
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
    // FIXME rename getRoadsBetweenStations
    std::vector<std::shared_ptr<Road>> MapState::getRoadBetweenStations(std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB)
    {
        return Road::getRoadsBetweenStations(this->roads, stationA, stationB);
    }

    std::vector<std::shared_ptr<Road>> MapState::getClaimableRoads(int nbPlayers, std::shared_ptr<playersState::Player> player)
    {
        return Road::getClaimableRoads(this->roads, nbPlayers, player);
    }

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
