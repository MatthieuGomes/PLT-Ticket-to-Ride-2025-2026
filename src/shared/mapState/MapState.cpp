#include "MapState.h"
#include "Path.h"
#include "../cardsState/ColorCard.h"
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

namespace mapState
{

    MapState::MapState()
    {
        this->gameGraph = boost::adjacency_list<>();
        Station *paris = new Station("paris", nullptr, false, boost::add_vertex(this->gameGraph));
        Station *berlin = new Station("berlin", nullptr, false, boost::add_vertex(this->gameGraph));
        Station *madrid = new Station("madrid", nullptr, false, boost::add_vertex(this->gameGraph));
        Station *rome = new Station("rome", nullptr, false, boost::add_vertex(this->gameGraph));
        this->stations = {paris, berlin, madrid, rome};
        
        // roads
        Road *roadPB = new Road(1, nullptr, paris, berlin, cardsState::ColorCard::RED, 2, false, boost::add_edge(*paris->getVertex(), *berlin->getVertex(), this->gameGraph).first);
        Ferry *roadBR = new Ferry(2, nullptr, berlin, rome, cardsState::ColorCard::BLUE, 3, 4, false, boost::add_edge(*berlin->getVertex(), *rome->getVertex(), this->gameGraph).first);
        Tunnel *roadPM = new Tunnel(3, nullptr, paris, madrid, cardsState::ColorCard::YELLOW, 5, false, boost::add_edge(*paris->getVertex(), *madrid->getVertex(), this->gameGraph).first);
        Road *roadMR = new Road(4, nullptr, madrid, rome, cardsState::ColorCard::YELLOW, 6, false, boost::add_edge(*madrid->getVertex(), *rome->getVertex(), this->gameGraph).first);
        this->roads = {roadPB, roadBR, roadPM, roadMR};
    };
    
    void MapState::display() const
    {
        std::cout << "===== MAP STATE =====\n";
        std::cout << "Stations:\n";
        for(Station* station : stations) {
            station->display();
            std::cout << "------------------\n";
        }
        std::cout << "Roads:\n";
        for(Road* road : roads) {
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
        for(Road * road : roads) {
            Station * a = road->getStationA();
            Station * b = road->getStationB();
            if((a == u && b == v) || (a == v && b == u)) {
                return road;
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

    std::vector<Station*> MapState::getAdjacentStations(Station *station) const
    {
        std::vector<Station*> adjacentStations;
        auto vertex = station->vertex;

        auto neighbors = boost::adjacent_vertices(vertex, this->gameGraph);
        for(auto it = neighbors.first; it != neighbors.second; ++it) {
            Station* neighborStation = nullptr;
            for(Station* s : this->stations) {
                if(s->vertex == *it) {
                    neighborStation = s;
                    break;
                }
            }
            if(neighborStation != nullptr) {
                adjacentStations.push_back(neighborStation);
            }
        }
        return adjacentStations;
    }

    std::string MapState::toString() const
    {
        std::ostringstream out;
        out << "===== MAP STATE =====\n";
        std::cout << "Stations:\n";
        for(Station* station : stations) {
            station->display();
            std::cout << "------------------\n";
        }
        std::cout << "Roads:\n";
        for(Road* road : roads) {
            road->display();
            std::cout << "------------------\n";
        }
        out << "==========================\n";
        return out.str();
    }

};