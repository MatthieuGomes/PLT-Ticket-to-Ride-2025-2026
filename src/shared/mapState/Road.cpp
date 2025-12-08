#include "Road.h"
#include <boost/graph/adjacency_list.hpp>
#include "Station.h"
#include <iostream>
using namespace std;

namespace mapState
{
    Road::Road(int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int length, bool isBlocked, boost::adjacency_list<>::edge_descriptor edge)
    {
        this->id = id;
        this->owner = owner;
        this->stationA = stationA;
        this->stationB = stationB;
        this->color = color;
        this->length = length;
        this->isBlocked = isBlocked;
        this->edge = edge;
    }
    int Road::getId()
    {
        return this->id;
    }
    playersState::Player *Road::getOwner()
    {
        return this->owner;
    }
    void Road::setOwner(playersState::Player *owner)
    {
        this->owner = owner;
    }
    void Road::display()
    {
        cout << "\tMap Element Type : Road\n";
        _display();
    }
    bool Road::getBlockStatus()
    {
        return this->isBlocked;
    }
    void Road::setBlockStatus(bool isBlocked)
    {
        this->isBlocked = isBlocked;
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

    void Road::_display()
    {
        cout << "\tStation A: " << this->stationA->getName() << "\n";
        cout << "\tStation B: " << this->stationB->getName() << "\n";
        cout << "\tColor: " << static_cast<int>(this->color) << "\n";
        cout << "\tLength: " << this->length << "\n";
        cout << "\tIs Blocked: " << (this->isBlocked ? "Yes" : "No") << "\n";
    }

    boost::adjacency_list<>::edge_descriptor *Road::getEdge()
    {
        return &(this->edge);
    }
    boost::adjacency_list<>::vertex_descriptor *Road::getVertexA()
    {
        return this->stationA->getVertex();
    }
    boost::adjacency_list<>::vertex_descriptor *Road::getVertexB()
    {
        return this->stationB->getVertex();
    }

    Road::~Road()
    {
    }
}