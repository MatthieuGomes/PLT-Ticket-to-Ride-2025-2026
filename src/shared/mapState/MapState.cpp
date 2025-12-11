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
    using StationInfo = std::tuple<playersState::Player *, bool, std::string>;
    using StationPair = std::pair<Station *, Station *>;
    using RoadDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, bool>;
    using RoadInfo = std::pair<StationPair, RoadDetail>;
    using TunnelDetail = RoadDetail;
    using TunnelInfo = RoadInfo;
    using FerryDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, int, bool>;
    using FerryInfo = std::pair<StationPair, FerryDetail>;

    MapState::MapState()
    {
        DEBUG_PRINT("default MapState creation started...");
        this->gameGraph = boost::adjacency_list<>();
        std::vector<StationInfo> stations = {
            Station::genData(nullptr, false, "paris"),
            Station::genData(nullptr, false, "berlin"),
            Station::genData(nullptr, false, "madrid"),
            Station::genData(nullptr, false, "rome"),
        };

        std::vector<Station> stationsObject = Station::BatchConstructor(stations, &this->gameGraph);
        std::vector<RoadInfo> roads = {
            Road::genData(
                &stationsObject[0],
                &stationsObject[1],
                1,
                nullptr,
                cardsState::ColorCard::RED,
                2,
                false),
            Road::genData(
                &stationsObject[2],
                &stationsObject[3],
                4,
                nullptr,
                cardsState::ColorCard::GREEN,
                6,
                false),
        };
        std::vector<TunnelInfo> tunnels = {
            Tunnel::genData(
                &stationsObject[0],
                &stationsObject[3],
                3,
                nullptr,
                cardsState::ColorCard::BLACK,
                4,
                false),

        };
        std::vector<FerryInfo> ferrys = {
            Ferry::genData(
                &stationsObject[1],
                &stationsObject[3],
                2,
                nullptr,
                cardsState::ColorCard::BLUE,
                3,
                4,
                false),
        };
        this->_MapState(stations, roads, tunnels, ferrys);
#ifdef DEBUG
        this->display();
#endif
        DEBUG_PRINT("Mapstate creation finished !");
    };
    MapState::MapState(std::vector<StationInfo> stationsInfos, std::vector<RoadInfo> roadsInfos, std::vector<TunnelInfo> tunnelsInfos, std::vector<FerryInfo> ferrysInfos)
    {
        DEBUG_PRINT("Parameterized MapState creation started...");
        this->gameGraph = boost::adjacency_list<>();
        this->_MapState(stationsInfos, roadsInfos, tunnelsInfos, ferrysInfos);
#ifdef DEBUG
        this->display();
#endif
        DEBUG_PRINT("Mapstate creation finished !");
    };

    void MapState::_MapState(std::vector<StationInfo> stationsInfos, std::vector<RoadInfo> roadsInfos, std::vector<TunnelInfo> tunnelsInfos, std::vector<FerryInfo> ferrysInfos)
    {

        DEBUG_PRINT("MapState _MapState started ...");
        std::vector<Station> stationObjects = Station::BatchConstructor(stationsInfos, &this->gameGraph);

        std::vector<Road> roadObjects = Road::BatchConstructor(roadsInfos, &this->gameGraph);

        std::vector<Tunnel> tunnelObjects = Tunnel::BatchConstructor(tunnelsInfos, &this->gameGraph);

        std::vector<Ferry> ferryObjects = Ferry::BatchConstructor(ferrysInfos, &this->gameGraph);

        for (Station station : stationObjects)
        {
            this->stations.push_back(new Station(station));
        }
        for (Road road : roadObjects)
        {
            this->roads.push_back(new Road(road));
        }
        for (Tunnel tunnel : tunnelObjects)
        {
            this->roads.push_back(new Tunnel(tunnel));
        }
        for (Ferry ferry : ferryObjects)
        {
            this->roads.push_back(new Ferry(ferry));
        }
        DEBUG_PRINT("MapState _MapState finished !");
    }

    MapState MapState::Empty()
    {
        DEBUG_PRINT("Empty MapState creation started ...");
        MapState mapState = MapState();
        mapState.stations.clear();
        mapState.roads.clear();
        mapState.gameGraph = boost::adjacency_list<>();
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
        for (Station *station : stations)
        {
            station->display();
            std::cout << "------------------\n";
        }
        std::cout << "Roads:\n";
        for (Road *road : roads)
        {
            road->display();
            std::cout << "------------------\n";
        }
        std::cout << "=====================\n";
    }

    std::vector<Station *> MapState::getStations() const
    {
        return this->stations;
    }

    std::vector<Road *> MapState::getRoads() const
    {
        return this->roads;
    }

    Station *MapState::getStationByName(const std::string &name)
    {

        for (Station *s : stations)
            if (s->name == name)
                return s;
        return nullptr;
    }

    Road *MapState::getRoadBetweenStations(Station *u, Station *v)
    {
        for (Road *r : roads)
        {
            if ((r->stationA->getName() == u->name && r->stationB->getName() == v->name) || (r->stationA->getName() == v->name && r->stationB->getName() == u->name))
            {
                return r;
            }
        }
        return nullptr;
    }

    Path MapState::findShortestPath(Station *src, Station *dest)
    {
        Path path;

        if (!src || !dest)
            return path;

        boost::adjacency_list<> &g = gameGraph;
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
            src->vertex,
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
            [this](vertex_descriptor v) -> Station *
        {
            for (Station *s : this->stations)
            {
                if (s->vertex == v)
                    return s;
            }
            return nullptr;
        };

        vertex_descriptor current = dest->vertex;
        if (distances[current] == std::numeric_limits<int>::max())
            return path;

        while (true)
        {
            Station *s = vertexToStation(current);
            if (s)
                path.STATIONS.push_back(s);

            if (current == src->vertex)
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

    std::vector<Station *> MapState::getAdjacentStations(Station *station)
    {
        if (!station)
        {
            return {};
        }
        std::vector<Station *> adjacentStations;
        for (Road *road : this->roads)
        {
#ifdef DEBUG
            road->display();
#endif
            if (road->stationA->getName() == station->getName())
            {
                adjacentStations.push_back(road->stationB);
                continue;
            }
            else if (road->stationB->getName() == station->getName())
            {
                adjacentStations.push_back(road->stationA);
                continue;
            }
        }
        return adjacentStations;
    }

    std::string MapState::toString() const
    {
        std::ostringstream out;
        out << "===== MAP STATE =====\n";
        std::cout << "Stations:\n";
        for (Station *station : stations)
        {
            station->display();
            std::cout << "------------------\n";
        }
        std::cout << "Roads:\n";
        for (Road *road : roads)
        {
            road->display();
            std::cout << "------------------\n";
        }
        out << "==========================\n";
        return out.str();
    }

};