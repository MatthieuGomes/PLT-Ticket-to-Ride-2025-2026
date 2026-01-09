#include "State.h"
#include <iostream>

namespace state
{
    using playersInitInfos = std::tuple<std::string, playersState::PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;
    State::State()
    {
        map = mapState::MapState();
        cards = cardsState::CardsState();
        players = playersState::PlayersState();
        
        // Initialize state variables here
    }
    State::State(std::vector<playersInitInfos> playersInfos){
        this->map = mapState::MapState();
        this->cards = cardsState::CardsState::Europe(map.getStations(), static_cast<int>(playersInfos.size()));
        this->players = playersState::PlayersState(playersInfos, cards.playersCards);
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