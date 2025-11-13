#include "MapState.h"
#include "Path.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp> 
#include <boost/graph/undirected_graph.hpp>

namespace mapState {

  MapState::MapState(){
      this->gameGraph = boost::adjacency_list<>();

      StationData* parisData  = new StationData{"Paris",  1};
      StationData* berlinData = new StationData{"Berlin", 2};
      StationData* madridData = new StationData{"Madrid", 3};
      StationData* romeData   = new StationData{"Rome",   4};

      Station* paris  = new Station{boost::add_vertex(gameGraph), parisData};
      Station* berlin = new Station{boost::add_vertex(gameGraph), berlinData};
      Station* madrid = new Station{boost::add_vertex(gameGraph), madridData};
      Station* rome   = new Station{boost::add_vertex(gameGraph), romeData};

      Stations = {paris, berlin, madrid, rome};

      // roads
      RoadData* p_b = new RoadData{"P-B", "red",    4, false, {parisData,  berlinData}};
      RoadData* b_r = new RoadData{"B-R", "blue",   3, false, {berlinData, romeData}};
      RoadData* p_m = new RoadData{"P-M", "yellow", 5, false, {parisData,  madridData}};
      RoadData* m_r = new RoadData{"M-R", "green",  6, false, {madridData, romeData}};

      Road* roadPB = new Road{boost::add_edge(paris->vertex,  berlin->vertex, gameGraph).first, p_b};
      Road* roadBR = new Road{boost::add_edge(berlin->vertex, rome->vertex, gameGraph).first, b_r};
      Road* roadPM = new Road{boost::add_edge(paris->vertex,  madrid->vertex, gameGraph).first, p_m};
      Road* roadMR = new Road{boost::add_edge(madrid->vertex, rome->vertex, gameGraph).first, m_r};

      Roads = {roadPB, roadBR, roadPM, roadMR};

  };
    void MapState::printMapState () const {
        std::cout << " Current Map State " << std::endl;
        std::cout << " Stations: " << std::endl;
        for (const Station* station : Stations) {
            std::cout << "  - " << station->data->name << " (ID: " << station->data->ID << ")" << std::endl;
        }
        std::cout << " Roads: " << std::endl;
        for (const Road* road : Roads) {
            std::cout << "  - " << road->data->ID << " [Color: " << road->data->color
                        << ", Length: " << road->data->length
                        << ", Blocked: " << (road->data->isBlocked ? "Yes" : "No") << "]"
                        << " connecting " << road->data->endpoints[0]->name
                        << " and " << road->data->endpoints[1]->name << std::endl;
        }
    }

    std::vector<Station*> MapState::listStations() const {
      return Stations;
    }

    std::vector<Road*> MapState::listRoads() const {
      return Roads;
    }

    Station* MapState::getStationByName(const std::string& name) {
      for (auto* s : Stations)
          if (s->data->name == name)
              return s;
      return nullptr;
    }

    Road* MapState::getRoad(Station* u, Station* v) {
      for (auto* r : Roads) {
          auto ep = r->data->endpoints;
          if ((ep[0] == u->data && ep[1] == v->data) ||
              (ep[0] == v->data && ep[1] == u->data))
              return r;
      }
      return nullptr;
    }

    Path MapState::findShortestPath(Station* src, Station* dest) {
        Path path;

        if (!src || !dest) return path;

        boost::adjacency_list<>& g = gameGraph;
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
                    boost::get(boost::vertex_index, g)
                )
            ).distance_map(
                boost::make_iterator_property_map(
                    distances.begin(),
                    boost::get(boost::vertex_index, g)
                )
            ).weight_map(weightMap)
        );

        auto vertexToStation =
            [this](vertex_descriptor v) -> Station* {
                for (Station* s : this->Stations) {
                    if (s->vertex == v) return s;
                }
                return nullptr;
            };

        vertex_descriptor current = dest->vertex;
        if (distances[current] == std::numeric_limits<int>::max())
            return path;

        while (true) {
            Station* s = vertexToStation(current);
            if (s) path.STATIONS.push_back(s);

            if (current == src->vertex) break;

            vertex_descriptor pred = predecessors[current];
            if (pred == current) break;

            current = pred;
        }

        std::reverse(path.STATIONS.begin(), path.STATIONS.end());
        path.NUMEDGES = int(path.STATIONS.size()) - 1;
        path.TOTALLENGTH = path.NUMEDGES;
        return path;
    }

    std::string MapState::printToString() const { 
      std::ostringstream out;
      out << "===== MAP STRUCTURE =====\n";

      out << "Stations (" << Stations.size() << "):\n";
      for (const auto* s : Stations) {
          out << "  - " << std::setw(8) << s->data->name 
              << " (ID: " << s->data->ID << ")\n";
      }

      out << "\nRoads (" << Roads.size() << "):\n";
      for (const auto* r : Roads) {
          auto* d = r->data;
          out << "  - " << std::setw(6) << d->ID
              << " | " << d->endpoints[0]->name << " <-> " << d->endpoints[1]->name
              << " | Color: " << d->color
              << " | Length: " << d->length
              << (d->isBlocked ? " | BLOCKED" : "")
              << "\n";
      }

      out << "\nConnections:\n";
      for (const auto* s : Stations) {
          out << "  " << s->data->name << " -> ";
          out << "\n";
      }

      out << "==========================\n";
      return out.str();
    }


};