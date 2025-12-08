#include "Ferry.h"
#include <iostream>
using namespace std;

namespace mapState
{

    Ferry::Ferry(int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int lenght, int locomotives, bool isBlocked, boost::adjacency_list<>::edge_descriptor edge) : Road(id, owner, stationA, stationB, color, lenght, isBlocked, edge)
    {
        this->locomotives = locomotives;
    }
    Ferry::~Ferry()
    {
    }

    void Ferry::display()
    {
        cout << "\tFerry Details:\n";
        Ferry::_display();
    }
    int Ferry::getLocomotives()
    {
        return this->locomotives;
    }
    void Ferry::_display()
    {   
        Road::_display();
        cout << "\tNumber of Locomotives: " << this->locomotives << "\n";
    }

}