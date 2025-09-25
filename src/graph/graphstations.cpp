#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

// attributes

// station (vertex)
struct Station {
    int id;             // ID único (map to name in dictonary)
    std::string name;   // stsation name
};

// road types
enum class RoadType { Normal, Tunnel, Ferry };

// road (edge) types
struct Road {
    std::string color;   
    int length;          
    int owner;           // -1 if unowned or player owner id
    bool blocked;        // if blocked (used at full capacity)
    RoadType type;       // Normal, Tunnel, Ferry
    int locomotive;      // numbe of locomotives for ferry
};

// graph definition
// store properties directly on vertices and edges
typedef boost::adjacency_list<
    boost::vecS,             // edge container
    boost::vecS,             // vertex container
    boost::undirectedS,      // bidirectional roads
    Station,                 // vertex properties
    Road                     // edge propertiess
> TicketToRideGraph;

// functions

void shortestPath(TicketToRideGraph& g,
                  TicketToRideGraph::vertex_descriptor start,
                  TicketToRideGraph::vertex_descriptor goal) {
    auto n_ver = num_vertices(g);

    std::vector<int> dist(n_ver);
    std::vector<TicketToRideGraph::vertex_descriptor> pred(n_ver);

    auto weightmap = get(&Road::length, g);

    // Dijkstra
    dijkstra_shortest_paths(
        g, start,
        predecessor_map(boost::make_iterator_property_map(pred.begin(), get(boost::vertex_index, g))).
        distance_map(boost::make_iterator_property_map(dist.begin(), get(boost::vertex_index, g))).
        weight_map(weightmap)
    );

    // rebuild path
    std::vector<TicketToRideGraph::vertex_descriptor> path;
    for (auto v = goal; v != start; v = pred[v]) {
        path.push_back(v);
        if (pred[v] == v) { // no path
            std::cout << "No path found!\n";
            return;
        }
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    // print path
    std::cout << "Shortest path from " << g[start].name
              << " to " << g[goal].name << ":\n";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << g[path[i]].name;
        if (i < path.size() - 1) std::cout << " -> ";
    }
    std::cout << "\n";

    // distance in weight
    std::cout << "Distance (sum of lengths): " << dist[goal] << "\n";

    // distance in routes traversed
    std::cout << "Nuber of roads traversed: " << path.size() - 1 << "\n";
}

int main() {
    // create graph
    TicketToRideGraph g;

    // add stations
    auto v1 = add_vertex({0, "Paris"}, g);
    auto v2 = add_vertex({1, "Bruxelles"}, g);
    auto v3 = add_vertex({2, "Amsterdam"}, g);
    auto v4 = add_vertex({3, "London"}, g);
    auto v5 = add_vertex({4, "Essen"}, g);
    auto v6 = add_vertex({5, "Dieppe"}, g);
    auto v7 = add_vertex({6, "Frankfurt"}, g);
    

    // add roads
    add_edge(v1, v2, Road{"yellow", 2, -1, false, RoadType::Normal, 0}, g); //par-bxl
    add_edge(v1, v2, Road{"red", 2, -1, false, RoadType::Normal, 0}, g);
    add_edge(v2, v3, Road{"black", 1, -1, false, RoadType::Normal, 0}, g); //bxl-ams
    add_edge(v3, v4, Road{"any", 2, -1, false, RoadType::Ferry, 2}, g); // ams-lon
    add_edge(v3, v5, Road{"yellow", 3, -1, false, RoadType::Normal, 2}, g); //ams-ess
    add_edge(v5, v7, Road{"green", 2, -1, false, RoadType::Normal, 2}, g); //ess-fra
    add_edge(v1, v6, Road{"yellow", 1, -1, false, RoadType::Normal, 2}, g); //par-die
    add_edge(v6, v4, Road{"any", 2, -1, false, RoadType::Ferry, 2}, g); //die-lon
    add_edge(v2, v7, Road{"blue", 2, -1, false, RoadType::Normal, 2}, g); //brx-fra
    add_edge(v1, v7, Road{"white", 3, -1, false, RoadType::Normal, 2}, g); //par-fra
    add_edge(v1, v7, Road{"orange", 3, -1, false, RoadType::Normal, 2}, g);


    // iterate vertices
    std::cout << "Stations:\n";
    for (auto vp = vertices(g); vp.first != vp.second; ++vp.first) {
        auto v = *vp.first;
        std::cout << "ID=" << g[v].id << " Name=" << g[v].name << "\n";
    }

    // iterate edges
    std::cout << "\nRoads:\n";
    for (auto ep = edges(g); ep.first != ep.second; ++ep.first) {
        auto e = *ep.first;
        auto u = source(e, g);
        auto v = target(e, g);

        std::cout << g[u].name << " <-> " << g[v].name
                  << " | color=" << g[e].color
                  << " | length=" << g[e].length
                  << " | owner=" << g[e].owner
                  << " | blocked=" << g[e].blocked
                  << " | typeo=" << (g[e].type == RoadType::Normal ? "Normal" :
                                    g[e].type == RoadType::Tunnel ? "Tunnel" : "Ferry")
                  << " | isLoco=" << g[e].locomotive
                  << "\n";
    }

    // use dijkstra to find shortest path
    // Paris -> London
    shortestPath(g, v1, v4);

    // Paris -> Frankfurt
    shortestPath(g, v1, v7);

    return 0;
}
