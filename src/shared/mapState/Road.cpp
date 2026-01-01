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
#define UNDEFINED_ID -1

using namespace std;

namespace mapState
{
    using StationPair = std::pair<std::shared_ptr<Station>, std::shared_ptr<Station>>;
    using RoadDetail = std::tuple<int, std::shared_ptr<playersState::Player>, cardsState::ColorCard, int, bool>;
    using RoadInfo = std::pair<StationPair, RoadDetail>;

    Road::Road(int id, std::shared_ptr<playersState::Player> owner, std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, cardsState::ColorCard color, int length, bool isBlocked, std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edge)
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
        DEBUG_PRINT("Road " << this->id << " created !");
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
    bool Road::getBlockStatus()
    {
        return this->isBlocked;
    }
    void Road::setBlockStatus(bool isBlocked)
    {
        this->isBlocked = isBlocked;
    }

    std::shared_ptr<Station> Road::getStationA()
    {
        return this->stationA;
    }
    std::shared_ptr<Station> Road::getStationB()
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

    bool Road::isClaimable(std::vector<std::shared_ptr<Road>> roads, int nbPlayers, std::shared_ptr<playersState::Player> player)
    {
        bool claimable = this->owner == nullptr && !this->isBlocked;
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
            if (road->getOwner()->getId() == player->getId())
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
            cardsState::ColorCard color = std::get<2>(detail);
            int length = std::get<3>(detail);
            bool isBlocked = std::get<4>(detail);
            std::shared_ptr<boost::adjacency_list<>::edge_descriptor> edgeDescriptor =
                std::make_shared<boost::adjacency_list<>::edge_descriptor>(
                    boost::add_edge(*(stationA->getVertex().get()), *(stationB->getVertex().get()), *gameGraph).first);
            roads.push_back(std::make_shared<Road>(id, owner, stationA, stationB, color, length, isBlocked, edgeDescriptor));
        }
        return roads;
    }
    RoadInfo Road::genData(std::shared_ptr<Station> stationA, std::shared_ptr<Station> stationB, int id, std::shared_ptr<playersState::Player> owner, cardsState::ColorCard color, int length, int isBlocked)
    {
        StationPair pair = std::pair(stationA, stationB);
        RoadDetail detail = std::tuple(id, owner, color, length, isBlocked);
        return std::pair(pair, detail);
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
