#include "Road.h"
#include <boost/graph/adjacency_list.hpp>
#include "MapState.h"
#include "Station.h"
#include <iostream>
using namespace std;

namespace mapState
{
    struct RoadData
    {
        int id;
        playersState::Player *owner;
        Station *stationA;
        Station *stationB;
        cardsState::ColorCard color;
        int length;
        bool isBlocked;
    };

    Road::Road(MapState *mapState, int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int lenght, bool isBlocked) : MapElement(mapState, id, owner, isBlocked)
    {
        this->stationA = stationA;
        this->stationB = stationB;
        this->color = color;
        this->length = length;
        this->isBlocked = isBlocked;
        this->edge = boost::add_edge(stationA->vertex, stationB->vertex, mapState->gameGraph).first;
    }

    Station *Road::getStationA()
    {
        return this->stationA;
    }
    Station *Road::getStationB()
    {
        return this->stationB;
    }
    cardsState::ColorCard Road::getColor()
    {
        return this->color;
    }  
    int Road::getLength()
    {
        return this->length;
    }
    bool Road::getBlockStatus()
    {
        return this->isBlocked;
    } 

    void Road::display(){
        cout << "\tMap Element Type : Road\n";
        cout << "\tStation A: " << this->stationA->getName() << "\n";
        cout << "\tStation B: " << this->stationB->getName() << "\n";
        cout << "\tColor: " << static_cast<int>(this->color) << "\n";
        cout << "\tLength: " << this->length << "\n";
        cout << "\tIs Blocked: " << (this->isBlocked ? "Yes" : "No") << "\n";
    }

    Road::~Road()
    {
    }
}