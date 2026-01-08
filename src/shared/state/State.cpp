#include "State.h"
#include <iostream>

namespace state
{
    State::State()
    {
        map = mapState::MapState();
        cards = cardsState::CardsState();
        players = playersState::PlayersState();
        
        // Initialize state variables here
    }
    // rename to display
    void State::display(int indent)
    {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "¤¤¤¤¤ STATE ¤¤¤¤¤" << std::endl;
        map.display(indent + 1);
        cards.display(indent + 1);
        players.display(indent + 1);
        std::cout << indentation << "¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤" << std::endl;
        // Here @theo + @chahrazed + @yosra will implement the details to print their part of the state
    }
}