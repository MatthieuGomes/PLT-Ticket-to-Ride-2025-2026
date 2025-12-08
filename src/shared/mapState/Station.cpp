#include "Station.h"
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
using namespace std;

struct StationData
{
    int id;
    playersState::Player *owner;
    std::string name;
};

namespace mapState
{

    Station::Station(std::string name, playersState::Player *owner, bool isBlocked, boost::adjacency_list<>::vertex_descriptor vertex)
    {
        this->owner = owner;
        this->name = name;
        this->isBlocked = isBlocked;
        this->vertex = vertex;
    }
    std::string Station::getName()
    {
        return this->name;
    }
    playersState::Player *Station::getOwner()
    {
        return this->owner;
    }
    void Station::setOwner(playersState::Player *owner)
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
        // Destructor logic (if any)
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

}