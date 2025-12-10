#include "Tunnel.h"
#include <iostream>

#define DEBUG_MODE false
#if DEBUG_MODE == true 
    #define DEBUG
    #define DEBUG_PRINT(x) std::cout << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

using namespace std;

namespace mapState
{
    using StationPair = std::pair<Station *, Station *>;
    using TunnelDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, bool>;
    using TunnelInfo = std::pair<StationPair, TunnelDetail>;

    Tunnel::Tunnel(int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int lenght, bool isBlocked, boost::adjacency_list<>::edge_descriptor edge)
        : Road(id, owner, stationA, stationB, color, lenght, isBlocked, edge)
    {
        DEBUG_PRINT("Parent constructor finished : Tunnel "<< this->id <<" created !");
    }

    Tunnel::~Tunnel()
    {
        DEBUG_PRINT("\tTunnel " << this->id << " Destroyed !");
    }

    void Tunnel::display()
    {
        cout << "\tTunnel Details:\n";
        Tunnel::_display();
    }

    void Tunnel::_display()
    {
        Road::_display();
    }
    std::vector<Tunnel> Tunnel::BatchConstructor(std::vector<TunnelInfo> tunnelsInfos, boost::adjacency_list<> *gameGraph)
    {
        DEBUG_PRINT("Tunnel BatchConstructor started ...");
        
        std::vector<Tunnel> tunnels;
        for (TunnelInfo info : tunnelsInfos)
        {
            StationPair pair = info.first;
            TunnelDetail detail = info.second;
            Station *stationA = pair.first;
            Station *stationB = pair.second;
            int id = std::get<0>(detail);
            playersState::Player *owner = std::get<1>(detail);
            cardsState::ColorCard color = std::get<2>(detail);
            int length = std::get<3>(detail);
            bool isBlocked = std::get<4>(detail);
            boost::adjacency_list<>::edge_descriptor edge = boost::add_edge(*(stationA->getVertex()), *(stationB->getVertex()), *gameGraph).first;
            tunnels.push_back(Tunnel(id, owner, stationA, stationB, color, length, isBlocked, edge));
        }
        DEBUG_PRINT("Tunnel BatchConstructor finished !");
        
        return tunnels;
    }

    TunnelInfo Tunnel::genData(Station *stationA, Station *stationB, int id, playersState::Player *owner, cardsState::ColorCard color, int length, bool isBlocked)
    {
        return Road::genData(stationA, stationB, id, owner, color, length, isBlocked);
    }

}