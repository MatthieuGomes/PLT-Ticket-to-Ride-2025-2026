#include "ExampleA.h"

#include <iostream>

#include "ChildExample.h"


namespace state {

    enum family
    { Jeanie, Debbie, Rick, N };
    const char *name[] = { "Jeanie", "Debbie", "Rick" };

    void ExampleA::setX(int x) {
        this->x = x;
    }

    ExampleA::ExampleA() {
        boost::adjacency_list<>  g(N);
        boost::add_edge(Jeanie, Debbie, g);
        boost::add_edge(Jeanie, Rick, g);

        boost::graph_traits < boost::adjacency_list <> >::vertex_iterator i, end;
        boost::graph_traits < boost::adjacency_list <> >::adjacency_iterator ai, a_end;
        boost::property_map < boost::adjacency_list <>, boost::vertex_index_t >::type
          index_map = get(boost::vertex_index, g);

        for (boost::tie(i, end) = vertices(g); i != end; ++i) {
            std::cout << name[get(index_map, *i)];
            tie(ai, a_end) = adjacent_vertices(*i, g);
            if (ai == a_end)
                std::cout << " has no children";
            else
                std::cout << " is the parent of ";
            for (; ai != a_end; ++ai) {
                std::cout << name[get(index_map, *ai)];
                if (boost::next(ai) != a_end)
                    std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }

}
