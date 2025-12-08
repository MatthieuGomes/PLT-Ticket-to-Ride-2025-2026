#include "Tunnel.h"
#include <iostream>
using namespace std;


namespace mapState
{

    Tunnel::Tunnel(int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int lenght, bool isBlocked, boost::adjacency_list<>::edge_descriptor edge)
        : Road(id, owner, stationA, stationB, color, lenght, isBlocked, edge)
    {
    }

    Tunnel::~Tunnel()
    {
        cout << "\tTunnel Details:\n";
    }

    void Tunnel::display()
    {
        Tunnel::_display();
    }

    void Tunnel::_display()
    {
        Road::_display();
    }

}