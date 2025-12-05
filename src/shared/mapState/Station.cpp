#include "Station.h"
#include <boost/graph/adjacency_list.hpp>
#include "MapState.h"
#include <iostream>
using namespace std;

struct StationData{
    int id;
    playersState::Player * owner;
    std::string name;
};


namespace mapState {
    Station::Station (MapState * mapState, int id, std::string name,playersState::Player * owner,bool isBlocked):MapElement(mapState, id, owner, isBlocked) {
        this->name = name;
        this->vertex = boost::add_vertex(mapState->gameGraph);
    }
    std::string Station::getName () {
        return this->name;
    }
    void Station::display () {
        cout << "\tMap Element Type : Station\n";
        MapElement::display();
        cout << "\tName : " << this->name << "\n";
    }

    Station::~Station () {
        // Destructor logic (if any)
    }

}