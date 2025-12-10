#include "Road.h"
#include <boost/graph/adjacency_list.hpp>
#include "Station.h"
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
    using RoadDetail = std::tuple<int, playersState::Player *, cardsState::ColorCard, int, bool>;
    using RoadInfo = std::pair<StationPair, RoadDetail>;

    Road::Road(int id, playersState::Player *owner, Station *stationA, Station *stationB, cardsState::ColorCard color, int length, bool isBlocked, boost::adjacency_list<>::edge_descriptor edge)
    {
        DEBUG_PRINT("Road creation started ...");
        this->id = id;
        this->owner = owner;
        this->stationA = stationA;
        this->stationB = stationB;
        this->color = color;
        this->length = length;
        this->isBlocked = isBlocked;
        this->edge = edge;
        DEBUG_PRINT("Road "<< this->id << " created !");

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
        cout << "\tRoad Details:\n";
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
        cout << "\tStation A: " << this->stationA->name << "\n";
        cout << "\tStation B: " << this->stationB->name << "\n";
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
    std::vector<Road> Road::BatchConstructor(std::vector<RoadInfo> roadsInfos, boost::adjacency_list<> *gameGraph)
    {
        
        std::vector<Road> roads;
        for (RoadInfo info : roadsInfos)
        {
            StationPair pair = info.first;
            RoadDetail detail = info.second;
            Station *stationA = pair.first;
            Station *stationB = pair.second;
            int id = std::get<0>(detail);
            playersState::Player *owner = std::get<1>(detail);
            cardsState::ColorCard color = std::get<2>(detail);
            int length = std::get<3>(detail);
            bool isBlocked = std::get<4>(detail);
            boost::adjacency_list<>::edge_descriptor edge = boost::add_edge(*(stationA->getVertex()), *(stationB->getVertex()), *gameGraph).first;
            roads.push_back(Road(id, owner, stationA, stationB, color, length, isBlocked, edge));
        }
        return roads;
    }
    RoadInfo Road::genData(Station *stationA, Station *stationB, int id, playersState::Player *owner, cardsState::ColorCard color, int length, int isBlocked)
    {
        StationPair pair = std::pair(stationA, stationB);
        RoadDetail detail = std::tuple(id, owner, color, length, isBlocked);
        return std::pair(pair, detail);
    }
    Road::~Road()
    {
         DEBUG_PRINT("\tRoad " << this->id << " Destroyed !");
    }
}