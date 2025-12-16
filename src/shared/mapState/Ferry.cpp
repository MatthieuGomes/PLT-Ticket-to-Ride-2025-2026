#include "Ferry.h"
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
    using StationPair = std::pair<std::shared_ptr<Station>,std::shared_ptr<Station>>;
    using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int,int, bool>;
    using FerryInfo = std::pair<StationPair,FerryDetail>;
    Ferry::Ferry(int id, std::shared_ptr<playersState::Player> owner, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, cardsState::ColorCard color, int lenght, int locomotives, bool isBlocked, boost::adjacency_list<>::edge_descriptor edge) : Road(id, owner, stationA, stationB, color, lenght, isBlocked, edge)
    {
        DEBUG_PRINT("Parent constructor finished :");
        
        DEBUG_PRINT("Ferry creation ...");
        this->locomotives = locomotives;
        DEBUG_PRINT("Ferry "<< this->id << " created !");
    }
    Ferry::~Ferry()
    {
         DEBUG_PRINT("\tFerry " << this->id << " Destroyed !");
    }

    void Ferry::display()
    {
        DEBUG_PRINT("\tFerry Details:");
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
    std::vector<std::shared_ptr<Ferry>> Ferry::BatchConstructor(std::vector<FerryInfo> tunnelsInfos, boost::adjacency_list<> * gameGraph){
        DEBUG_PRINT("Ferry BatchConstructor started ...");
        std::vector<std::shared_ptr<Ferry>> tunnels;
        for(FerryInfo info : tunnelsInfos){
            StationPair pair = info.first;
            FerryDetail detail = info.second;
            std::shared_ptr<Station> stationA = pair.first;
            std::shared_ptr<Station> stationB = pair.second;
            int id  = std::get<0>(detail);
            std::shared_ptr<playersState::Player> owner = std::get<1>(detail);
            cardsState::ColorCard color = std::get<2>(detail);
            int length = std::get<3>(detail);
            int locomotives = std::get<4>(detail);
            bool isBlocked = std::get<5>(detail);
            boost::adjacency_list<>::edge_descriptor edge = boost::add_edge(*(stationA->getVertex()), *(stationB->getVertex()), *gameGraph).first;
            tunnels.push_back(std::make_shared<Ferry>(id,owner,stationA,stationB,color,length,locomotives,isBlocked,edge));
        }
        DEBUG_PRINT("Ferry BatchConstructor finished !");
        return tunnels;
    }

    FerryInfo Ferry::genData (std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int id, std::shared_ptr<playersState::Player> owner, cardsState::ColorCard color,int locomotives, int length, bool isBlocked){
        StationPair pair = std::pair(stationA,stationB);
        FerryDetail detail = std::tuple(id,owner,color,locomotives,length,isBlocked);
        return std::pair(pair,detail);
    }

}
