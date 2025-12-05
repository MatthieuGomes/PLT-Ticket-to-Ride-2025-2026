#include "Tunnel.h"
#include <iostream>
using namespace std;


namespace mapState
{

    Tunnel::Tunnel(MapState *mapState, int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int lenght, bool isBlocked)
        : Road(mapState, id, owner, stationA, stationB, color, lenght, isBlocked)
    {
    }

    Tunnel::~Tunnel()
    {
    }

    void Tunnel::display()
    {
        cout<< "MapElement Type: Tunnel" << endl;
        Road::display();
    }

}