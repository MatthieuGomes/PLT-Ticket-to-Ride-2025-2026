#include "Ferry.h"
#include <iostream>
using namespace std;

namespace mapState
{

    Ferry::Ferry(MapState *mapState, int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int lenght, int locomotives, bool isBlocked) : Road(mapState, id, owner, stationA, stationB, color, lenght, isBlocked)
    {
        this->locomotives = locomotives;
    }
    Ferry::~Ferry()
    {
    }

    void Ferry::display()
    {
        cout << "MapElement Type: Ferry" << endl;
        Road::display();
    }
    int Ferry::getLocomotives()
    {
        return this->locomotives;
    }

}