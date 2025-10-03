#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

int main() {
    // Define weight and distance properties for the edges
    typedef property<edge_weight_t, int> EdgeWeightProperty;

    // weighed directed graph with vertices and edges added
    typedef adjacency_list<vecS, vecS, bidirectionalS, no_property, EdgeWeightProperty> Graph;

    // vertices labels
    enum { A, B, C, D, E, N };
    const int num_vertices = N;
    const char* name = "ABCDE";

    // list of edges (u, v, weight)
    typedef std::tuple<int, int, int> Edge;
    Edge edge_array[] = {
        Edge(A,B,2), Edge(A,D,1), Edge(C,A,7), Edge(D,C,2),
        Edge(C,E,1), Edge(B,D,3), Edge(D,E,1)
    };
    const int num_edges = sizeof(edge_array) / sizeof(edge_array[0]);

    // make graph
    Graph g(num_vertices);
    for (int i = 0; i < num_edges; ++i) {
        add_edge(std::get<0>(edge_array[i]), std::get<1>(edge_array[i]),
                 EdgeWeightProperty(std::get<2>(edge_array[i])), g);
    }

    // print vertices
    std::cout << "Vertices:\n";
    for (int i = 0; i < num_vertices; ++i) {
        std::cout << i << ": " << name[i] << "\n";
    }

    // print edges
    std::cout << "\nEdges:\n";
    graph_traits<Graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
        int u = source(*ei, g);
        int v = target(*ei, g);
        int w = get(edge_weight, g, *ei);
        std::cout << name[u] << " -> " << name[v] << " (Weight " << w << ")\n";
    }

    // use Dijkstra'algorith to find shortest path
    std::vector<int> dist(num_vertices);
    std::vector<int> pred(num_vertices);

    dijkstra_shortest_paths(g, A,
        predecessor_map(make_iterator_property_map(pred.begin(), get(vertex_index, g))).
        distance_map(make_iterator_property_map(dist.begin(), get(vertex_index, g))));

    std::cout << "\nDistances from " << name[A] << ":\n";
    for (int i = 0; i < num_vertices; ++i) {
        std::cout << name[A] << " -> " << name[i] << " = " << dist[i] << "\n";
    }

    return 0;
}
