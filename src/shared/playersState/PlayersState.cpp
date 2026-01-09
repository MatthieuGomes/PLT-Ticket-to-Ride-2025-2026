#include "PlayersState.h"
#include <iostream>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

using namespace std;

namespace playersState
{
    using playersInfos = std::tuple<std::string, PlayerColor, int, int, int,int, std::shared_ptr<cardsState::PlayerCards>>;
    using playersInitInfos = std::tuple<std::string, PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

    int PlayersState::nbPlayers = 0;

    PlayersState::PlayersState()
    {
    }

    PlayersState::PlayersState(std::vector<std::shared_ptr<Player>> players)
    {
        this->players = players;
        PlayersState::nbPlayers = static_cast<int>(players.size());   
    }

    PlayersState::PlayersState(std::vector<playersInfos> infos)
    {
        this->players = Player::BatchFromInfos(infos);
        PlayersState::nbPlayers = static_cast<int>(this->players.size());   
    }

    PlayersState PlayersState::InitFromInfos(std::vector<playersInitInfos> infos)
    {
        PlayersState state;
        state.players = Player::BatchFromInitInfos(infos);
        PlayersState::nbPlayers = static_cast<int>(state.players.size());   
        return state;
    }

    PlayersState::PlayersState(std::vector<playersInitInfos> infos, std::vector<std::shared_ptr<cardsState::PlayerCards>> hands)
    {
        this->players = Player::BatchFromInitInfos(infos);
        PlayersState::nbPlayers = static_cast<int>(this->players.size());  
        this->setupPlayersHand(hands);
    }

    std::vector<std::shared_ptr<Player>> PlayersState::getPlayers()
    {
        return this->players;
    }
    void PlayersState::setPlayers(std::vector<std::shared_ptr<Player>> players)
    {
        this->players = players;
        PlayersState::nbPlayers = static_cast<int>(players.size());
    }

    void PlayersState::setupPlayersHand(std::vector<std::shared_ptr<cardsState::PlayerCards>> hands)
    {
        for (size_t i = 0; i < this->players.size(); i++)
        {
            this->players[i]->setHand(hands[i]);
        }
    }

    std::vector<std::shared_ptr<mapState::Road>> PlayersState::getClaimableRoads(std::shared_ptr<mapState::MapState> map, std::shared_ptr<Player> player)
    {
        return map->getClaimableRoads(PlayersState::nbPlayers, player); 
    } 
    bool PlayersState::isRoadClaimable(std::shared_ptr<mapState::MapState> map, std::shared_ptr<mapState::Road> road, std::shared_ptr<Player> player)
    {
        return map->isRoadClaimable(PlayersState::nbPlayers, road, player);
    }
    void PlayersState::display(int indent)
    {
        std::string indentation(indent, '\t');
        std::cout << indentation << "~~~~~~ PLAYERS STATE ~~~~~~\n";
        std::cout << indentation << "\t" << "##### PLAYERS #####\n";
        if (players.empty())
        {
            std::cout << indentation << "\t\t"<< "No players in the game." << std::endl;
        }
        
        for (std::shared_ptr<Player> player : players)
        {
            player->display(indent+2);
        }
        std::cout << indentation << "\t" << "#######################\n";
        std::cout << indentation << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }

};
