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
    void State::printState()
    {
        std::cout << "State information goes here." << std::endl;
        cards.printCardsState();
        players.printPlayersState();
        map.display();
        // Here @theo + @chahrazed + @yosra will implement the details to print their part of the state
    }
}