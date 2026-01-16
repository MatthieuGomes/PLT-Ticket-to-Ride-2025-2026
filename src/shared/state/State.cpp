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
    State::State(std::string mapName, std::vector<playersInitInfos> playersInfos){
        this->map = mapState::MapState::NamedMapState(mapName);
        this->cards = cardsState::CardsState::Europe(map.getStations(), static_cast<int>(playersInfos.size()));
        this->players = playersState::PlayersState(playersInfos, cards.playersCards);
    }

    State::State(std::string pathToFile)
    {
        std::string jsonContent;
        std::string jsonMap;
        std::string jsonPlayers;
        std::string jsonCards;
        this->players = playersState::PlayersState::InitFromJSON(jsonPlayers);
        this->map = mapState::MapState::ParseFromJSON(jsonMap, std::make_shared<playersState::PlayersState>(players));
        this->cards = cardsState::CardsState::ParseFromJSON(jsonCards, std::make_shared<mapState::MapState>(map));
        this->players.setupFromJSON(jsonPlayers, std::make_shared<mapState::MapState>(map), std::make_shared<cardsState::CardsState>(cards));

    }

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