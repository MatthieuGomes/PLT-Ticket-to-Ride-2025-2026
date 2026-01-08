#include "Ferry.h"
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

namespace mapState
{
    using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
    using FerryDetail = std::tuple<int, std::shared_ptr<playersState::Player>, int, int>;
    using FerryInfo = std::pair<StationPair, FerryDetail>;
    Ferry::Ferry() : Road()
    {
        this->locomotives = -1;
    }

    Ferry::Ferry(int id, std::shared_ptr<playersState::Player> owner, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int locomotives, int length, std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edge) : Road(id, owner, stationA, stationB, RoadColor::NONE, length, edge)
    {
        DEBUG_PRINT("Parent constructor finished :");

        DEBUG_PRINT("Ferry creation ...");
        this->locomotives = locomotives;
        DEBUG_PRINT("Ferry " << this->id << " created !");
    }

    Ferry Ferry::Init(int id, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int locomotives, int length, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {
        DEBUG_PRINT("Ferry creation started ...");
        std::shared_ptr<playersState::Player> owner = nullptr;
        std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edge = std::make_shared<boost::adjacency_list<>::edge_descriptor>(
            boost::add_edge(*(stationA->getVertex().get()), *(stationB->getVertex().get()), *gameGraph).first);
        return Ferry(id, owner, stationA, stationB, locomotives, length, edge);
        DEBUG_PRINT("Ferry " << this->id << " created !");
    }

    Ferry::~Ferry()
    {
        DEBUG_PRINT("\tFerry " << this->id << " Destroyed !");
    }

    void Ferry::display(int indent)
    {
        std::string indentation(indent, '\t');
        std::cout << indentation << "-----FERRY-----" << std::endl;
        Ferry::_display(indent);
        std::cout << indentation << "----------------" << std::endl;
    }
    void Ferry::_display(int indent)
    {
        Road::_display(indent);
        std::string indentation(indent, '\t');
        std::cout << indentation << "Locomotives: " << this->locomotives << std::endl;
    }
    int Ferry::getLocomotives()
    {
        return this->locomotives;
    }
    
    std::vector<std::shared_ptr<Ferry>> Ferry::BatchConstructor(std::vector<FerryInfo> tunnelsInfos, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {
        DEBUG_PRINT("Ferry BatchConstructor started ...");
        if (!gameGraph)
        {
            throw std::invalid_argument("Ferry::BatchConstructor requires a graph");
        }
        std::vector<std::shared_ptr<Ferry>> tunnels;
        for (FerryInfo info : tunnelsInfos)
        {
            StationPair pair = info.first;
            FerryDetail detail = info.second;
            std::shared_ptr<Station> stationA = pair.first;
            std::shared_ptr<Station> stationB = pair.second;
            int id = std::get<0>(detail);
            std::shared_ptr<playersState::Player> owner = std::get<1>(detail);
            int locomotives = std::get<2>(detail);
            int length = std::get<3>(detail);
            std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edgeDescriptor =
                std::make_shared<boost::adjacency_list<>::edge_descriptor>(
                    boost::add_edge(*(stationA->getVertex().get()), *(stationB->getVertex().get()), *gameGraph).first);
            tunnels.push_back(std::make_shared<Ferry>(id, owner, stationA, stationB, locomotives, length, edgeDescriptor));
        }
        DEBUG_PRINT("Ferry BatchConstructor finished !");
        return tunnels;
    }

    FerryInfo Ferry::genData(std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int id, std::shared_ptr<playersState::Player> owner, int locomotives, int length)
    {
        StationPair pair = std::pair(stationA, stationB);
        FerryDetail detail = std::tuple(id, owner, locomotives, length);
        return std::pair(pair, detail);
    }

    FerryInfo Ferry::initData(std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int id, int locomotives, int length)
    {
        return Ferry::genData(stationA, stationB, id, nullptr, locomotives, length);
    }
    FerryInfo Ferry::genDataByName(std::vector<std::shared_ptr<Station>> stations, const std::string &stationAName, const std::string &stationBName, int id, std::shared_ptr<playersState::Player> owner, int locomotives, int length)
    {
        std::shared_ptr<Station> stationA = Station::getStationByName(stations, stationAName);
        std::shared_ptr<Station> stationB = Station::getStationByName(stations, stationBName);
        return Ferry::genData(stationA, stationB, id, owner, locomotives, length);
    }
    FerryInfo Ferry::initDataByName(std::vector<std::shared_ptr<Station>> stations, const std::string &stationAName, const std::string &stationBName, int id, int locomotives, int length)
    {
        std::shared_ptr<Station> stationA = Station::getStationByName(stations, stationAName);
        std::shared_ptr<Station> stationB = Station::getStationByName(stations, stationBName);
        return Ferry::genData(stationA, stationB, id, nullptr, locomotives, length);
    }

}
