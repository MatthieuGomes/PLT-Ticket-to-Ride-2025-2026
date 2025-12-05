#include "MapElement.h"
#include <iostream>
using namespace std;

namespace mapState {
    MapElement::MapElement (MapState * mapState, int id, playersState::Player * owner, bool isBlocked) {
        this->id = id;
        this->owner = owner;
        this->isBlocked = isBlocked;
    }
    int MapElement::getId () {
        return this->id;
    }
    playersState::Player * MapElement::getOwner () {
        return this->owner;
    }
    void MapElement::display () {
        cout << "\tID : " << this->id << "\n";
        if (this->owner != nullptr) {
            cout << "\tOwner : " << this->owner->getName() << "\n";
        } else {
            cout << "\tOwner : None\n";
        }
    }
    bool MapElement::getBlockStatus () {
        return this->isBlocked;
    }
    MapElement::~MapElement () {
        
    }
}