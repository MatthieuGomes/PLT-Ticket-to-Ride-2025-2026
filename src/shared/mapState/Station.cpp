#include "Station.h"
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <stdexcept>

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

    using StationInfo = std::tuple<std::shared_ptr<playersState::Player>, std::string>;

    Station::Station(std::string name, std::shared_ptr<playersState::Player> owner, std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> vertex)
    {
        DEBUG_PRINT("Station creation started ...");
        this->owner = owner;
        this->name = name;
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

    bool Station::isClaimable()
    {
        return (this->owner == nullptr);
    }

    Station::~Station()
    {
        DEBUG_PRINT("Station " << this->name << " Destroyed !");
    }

    std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> Station::getVertex()
    {
        return this->vertex;
    }

    std::vector<std::shared_ptr<Station>> Station::getAdjacentStations(std::vector<std::shared_ptr<Road>> roads)
    {
        std::vector<std::shared_ptr<Station>> adjacentStations = {};
        std::string stationName = this->name;
        for (const std::shared_ptr<Road> &road : roads)
        {
            std::shared_ptr<Station> stationA = road->getStationA();
            std::shared_ptr<Station> stationB = road->getStationB();
            if (stationA->getName() == stationName)
            {
                adjacentStations.push_back(stationB);
                continue;
            }
            else if (stationB->getName() == stationName)
            {
                adjacentStations.push_back(stationA);
                continue;
            }
        }
        return adjacentStations;
    }

    std::shared_ptr<Station> Station::getStationByName(std::vector<std::shared_ptr<Station>> stations, const std::string &name)
    {
        for (const std::shared_ptr<Station> &station : stations)
        {
            if (station->name == name)
            {
                return station;
            }
        }
        return nullptr;
    }

    void Station::_display()
    {
        cout << "\tName : " << this->name << "\n";
        if (this->owner != nullptr)
        {
            cout << "\tOwner : " << this->owner->getName() << "\n";
        }
        else
        {
            cout << "\tOwner : None\n";
        }
    }
    std::vector<std::shared_ptr<Station>> Station::BatchConstructor(std::vector<StationInfo> stationInfos, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {
        DEBUG_PRINT("Station BatchConstructor started ...");
        if (!gameGraph)
        {
            throw std::invalid_argument("Station::BatchConstructor requires a graph");
        }
        std::vector<std::shared_ptr<Station>> stations;
        for (StationInfo info : stationInfos)
        {
            std::shared_ptr<playersState::Player> owner = std::get<0>(info);
            std::string name = std::get<1>(info);
            std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> descriptor =
                std::make_shared<boost::adjacency_list<>::vertex_descriptor>(
                    boost::add_vertex(*gameGraph));
            stations.push_back(std::make_shared<Station>(name, owner, descriptor));
        }
        DEBUG_PRINT("Station BatchConstructor finished !");
        return stations;
    }

    StationInfo Station::genData(std::shared_ptr<playersState::Player> owner, std::string name)
    {
        return std::tuple(owner, name);
    }
    StationInfo Station::initData(std::string name)
    {
        return Station::genData(nullptr, name);
    }

}
