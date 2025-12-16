#include "Station.h"
#include <boost/graph/adjacency_list.hpp>
#include <iostream>

#define DEBUG_MODE false
#if DEBUG_MODE == true 
    #define DEBUG
    #define DEBUG_PRINT(x) std::cout << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

using namespace std;

struct StationData
{
    int id;
    std::shared_ptr<playersState::Player> owner;
    std::string name;
};

namespace mapState
{

    using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, bool, std::string>;

    Station::Station(std::string name, std::shared_ptr<playersState::Player> owner, bool isBlocked, boost::adjacency_list<>::vertex_descriptor vertex)
    {
        DEBUG_PRINT("Station creation started ...");
        this->owner = owner;
        this->name = name;
        this->isBlocked = isBlocked;
        this->vertex = vertex;
        DEBUG_PRINT("Station " << name << " created !");
    }
    std::string Station::getName()
    {
        return this->name;
    }
    std::shared_ptr<playersState::Player> Station::getOwner()
    {
        return this->owner;
    }
    void Station::setOwner(std::shared_ptr<playersState::Player> owner)
    {
        this->owner = owner;
    }
    void Station::display()
    {
        cout << "\tStation Details:\n";
        Station::_display();
    }

    bool Station::getBlockStatus()
    {
        return this->isBlocked;
    }

    void Station::setBlockStatus(bool isBlocked)
    {
        this->isBlocked = isBlocked;
    }

    Station::~Station()
    {
        DEBUG_PRINT("Station " << this->name << " Destroyed !");
    }

    boost::adjacency_list<>::vertex_descriptor *Station::getVertex()
    {
        return &(this->vertex);
    }

    void Station::_display()
    {
        cout << "\tName : " << this->name << "\n";
        if (this->owner != nullptr)
        {
            cout << "\tOwner : " << "test" << "\n";
        }
        else
        {
            cout << "\tOwner : None\n";
        }
        cout << "\tBlocked : " << (this->isBlocked ? "Yes" : "No") << "\n";
    }
    std::vector<std::shared_ptr<Station>> Station::BatchConstructor(std::vector<StationInfo> stationInfos, boost::adjacency_list<> *gameGraph)
    {
        DEBUG_PRINT("Station BatchConstructor started ...");
        std::vector<std::shared_ptr<Station>> stations;
        for (StationInfo info : stationInfos)
        {
            std::shared_ptr<playersState::Player> owner = std::get<0>(info);
            bool isBlocked = std::get<1>(info);
            std::string name = std::get<2>(info);
            stations.push_back(std::make_shared<Station>(name, owner, isBlocked, boost::add_vertex(*gameGraph)));
        }
        DEBUG_PRINT("Station BatchConstructor finished !");
        return stations;
    }

    StationInfo Station::genData(std::shared_ptr<playersState::Player> owner, bool isBlocked, std::string name)
    { 
        return std::tuple(owner, isBlocked, name);
    } 
}
