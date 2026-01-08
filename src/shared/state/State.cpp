#include "State.h"
#include <iostream>

namespace state
{
    State::State()
    {
        cards = cardsState::CardsState();
        players = playersState::PlayersState();
        map = mapState::MapState();
        // Initialize state variables here
    }
    // rename to display
    void State::display()
    {
        std::string indentation = std::string(0, '\t');
        std::cout << "¤¤¤¤¤ STATE ¤¤¤¤¤" << std::endl;
        map.display();
        cards.display(1);
        players.display();
        std::cout << "¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤" << std::endl;
        // Here @theo + @chahrazed + @yosra will implement the details to print their part of the state
    }
}