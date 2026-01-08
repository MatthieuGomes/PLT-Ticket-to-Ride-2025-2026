#include "Road.h"
#include <boost/graph/adjacency_list.hpp>
#include "Station.h"
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
    using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, RoadColor, int>;
    using RoadInfo = std::pair<StationPair, RoadDetail>;

    
    RoadColor Road::possibleColors[10] = {
        RoadColor::UNKNOWN,
        RoadColor::RED,
        RoadColor::BLUE,
        RoadColor::GREEN,
        RoadColor::WHITE,
        RoadColor::BLACK,
        RoadColor::YELLOW,
        RoadColor::PINK,
        RoadColor::ORANGE,
        RoadColor::NONE
    };

    std::unordered_map<RoadColor, std::string> Road::ColorsNames = {
        {RoadColor::UNKNOWN, "UNKNOWN"},
        {RoadColor::RED, "RED"},
        {RoadColor::BLUE, "BLUE"},
        {RoadColor::GREEN, "GREEN"},
        {RoadColor::BLACK, "BLACK"},
        {RoadColor::YELLOW, "YELLOW"},
        {RoadColor::ORANGE, "ORANGE"},
        {RoadColor::PINK, "PINK"},
        {RoadColor::WHITE, "WHITE"},
        {RoadColor::NONE, "NONE"}
    };


    Road::Road() {
        this->id = -1;
        this->color = RoadColor::UNKNOWN;
        this->length = -1;
    }
    Road::Road(int id, std::shared_ptr<playersState::Player> owner, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, RoadColor color, int length, std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edge)
    {
        DEBUG_PRINT("Road creation started ...");
        this->id = id;
        this->owner = owner;
        this->stationA = stationA;
        this->stationB = stationB;
        this->color = color;
        this->length = length;
        this->edge = edge;
        DEBUG_PRINT("Road " << this->id << " created !");
    }
    Road Road::Init(int id, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, RoadColor color, int length, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {
        DEBUG_PRINT("Road creation started ...");
        std::shared_ptr<playersState::Player> owner = nullptr;
        std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edge = std::make_shared<boost::adjacency_list<>::edge_descriptor>(
            boost::add_edge(*(stationA->getVertex().get()), *(stationB->getVertex().get()), *gameGraph).first);
        DEBUG_PRINT("Road " << id << " created !");
        return Road(id, owner, stationA, stationB, color, length, edge);
    }
    int Road::getId()
    {
        return this->id;
    }
    std::shared_ptr<playersState::Player> Road::getOwner()
    {
        return this->owner;
    }
    void Road::setOwner(std::shared_ptr<playersState::Player> owner)
    {
        this->owner = owner;
    }
    void Road::display()
    {
        cout << "\tRoad Details:\n";
        _display();
    }

    std::shared_ptr<Station> Road::getStationA()
    {
        return this->stationA;
    }
    std::shared_ptr<Station> Road::getStationB()
    {
        return this->stationB;
    }
    RoadColor Road::getColor()
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
        cout << "\tOwner: " << (this->owner ? this->owner->name : "None") << "\n";
    }

    bool Road::isClaimable(std::vector<std::shared_ptr<Road>> roads, int nbPlayers, std::shared_ptr<playersState::Player> player)
    {
        bool claimable = this->owner == nullptr;
        if (!claimable)
        {
            return false;
        }
        std::vector<std::shared_ptr<Road>> doubleRoads = Road::getRoadsBetweenStations(roads, this->stationA, this->stationB);
        for (const std::shared_ptr<Road> &road : doubleRoads)
        {
            if (road->id == this->id)
            {
                continue;
            }
            if (road->getOwner() != nullptr && road->getOwner()->getColor() == player->getColor())
            {
                return false;
            }
            if (nbPlayers <= 3 && road->getOwner() != nullptr)
            {
                return false;
            }
        }
        return claimable;
    }

    std::vector<std::shared_ptr<Road>> Road::getClaimableRoads(std::vector<std::shared_ptr<Road>> roads, int nbPlayers, std::shared_ptr<playersState::Player> player)
    {
        std::vector<std::shared_ptr<Road>> claimable;
        for (const std::shared_ptr<Road> &road : roads)
        {
            if (road->isClaimable(roads, nbPlayers, player))
            {
                claimable.push_back(road);
            }
        }
        return claimable;
    }

    std::shared_ptr<boost::adjacency_list<>::edge_descriptor> Road::getEdge()
    {
        return this->edge;
    }
    std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> Road::getVertexA()
    {
        return this->stationA->getVertex();
    }
    std::shared_ptr<boost::adjacency_list<>::vertex_descriptor> Road::getVertexB()
    {
        return this->stationB->getVertex();
    }
    std::vector<std::shared_ptr<Road>> Road::BatchConstructor(std::vector<RoadInfo> roadsInfos, std::shared_ptr<boost::adjacency_list<>> gameGraph)
    {
        if (!gameGraph)
        {
            throw std::invalid_argument("Road::BatchConstructor requires a graph");
        }
        std::vector<std::shared_ptr<Road>> roads;
        for (RoadInfo info : roadsInfos)
        {
            StationPair pair = info.first;
            RoadDetail detail = info.second;
            std::shared_ptr<Station> stationA = pair.first;
            std::shared_ptr<Station> stationB = pair.second;
            int id = std::get<0>(detail);
            std::shared_ptr<playersState::Player> owner = std::get<1>(detail);
            RoadColor color = std::get<2>(detail);
            int length = std::get<3>(detail);
            std::shared_ptr<std::size_t> stationAVertex = stationA->getVertex();
            std::shared_ptr<std::size_t> stationBVertex = stationB->getVertex(); 
            std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edgeDescriptor =
                std::make_shared<boost::adjacency_list<>::edge_descriptor>(
                    boost::add_edge(*(stationA->getVertex().get()), *(stationB->getVertex().get()), *gameGraph).first);
            roads.push_back(std::make_shared<Road>(id, owner, stationA, stationB, color, length, edgeDescriptor));
        }
        return roads;
    }
    RoadInfo Road::genData(std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int id, std::shared_ptr<playersState::Player> owner, RoadColor color, int length)
    {
        StationPair pair = std::pair(stationA, stationB);
        RoadDetail detail = std::tuple(id, owner, color, length);
        return std::pair(pair, detail);
    }
    RoadInfo Road::initData(std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int id, RoadColor color, int length)
    {
        return Road::genData(stationA, stationB, id, nullptr, color, length);
    }
    RoadInfo Road::genDataByName(std::vector<std::shared_ptr<Station>> stations, const std::string &stationAName, const std::string &stationBName, int id, std::shared_ptr<playersState::Player> owner, RoadColor color, int length)
    {
        std::shared_ptr<Station> stationA = Station::getStationByName(stations, stationAName);
        std::shared_ptr<Station> stationB = Station::getStationByName(stations, stationBName);
        return Road::genData(stationA, stationB, id, owner, color, length);
    }
    RoadInfo Road::initDataByName(std::vector<std::shared_ptr<Station>> stations, const std::string &stationAName, const std::string &stationBName, int id, RoadColor color, int length)
    {
        std::shared_ptr<Station> stationA = Station::getStationByName(stations, stationAName);
        std::shared_ptr<Station> stationB = Station::getStationByName(stations, stationBName);
        return Road::genData(stationA, stationB, id, nullptr, color, length);
    }
    std::vector<std::shared_ptr<Road>> Road::getRoadsBetweenStations(std::vector<std::shared_ptr<Road>> roads, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB)
    {
        std::vector<std::shared_ptr<Road>> matchingRoads = {};
        if (!(!stationA || !stationB))
        {
            for (const std::shared_ptr<Road> &road : roads)
            {
                bool stationAAMatch = (road->stationA->getName() == stationA->getName());
                bool stationABMatch = (road->stationB->getName() == stationA->getName());
                bool stationBAMatch = (road->stationA->getName() == stationB->getName());
                bool stationBBMatch = (road->stationB->getName() == stationB->getName());
                if ((stationAAMatch && stationBBMatch) || (stationABMatch && stationBAMatch))
                {
                    matchingRoads.push_back(road);
                }
            }
        }

        return matchingRoads; // Return nullptr if no matching road is found
    }
    Road::~Road()
    {
        DEBUG_PRINT("\tRoad " << this->id << " Destroyed !");
    }
}
