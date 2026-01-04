#include "PlayersState.h"
#include <iostream>

using namespace std;

namespace playersState
{
    PlayersState::PlayersState()
    {
        this->players = {};
        PlayersState::nbPlayers = static_cast<int>(players.size());   
    }

    PlayersState::PlayersState(std::vector<std::shared_ptr<Player>> players)
    {
        this->players = players;
        PlayersState::nbPlayers = static_cast<int>(players.size());   
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

    std::vector<std::shared_ptr<mapState::Road>> PlayersState::getClaimableRoads(std::shared_ptr<mapState::MapState> map, std::shared_ptr<Player> player)
    {
        return map->getClaimableRoads(PlayersState::nbPlayers, player); 
    } 
    bool PlayersState::isRoadClaimable(std::shared_ptr<mapState::MapState> map, std::shared_ptr<mapState::Road> road, std::shared_ptr<Player> player)
    {
        return map->isRoadClaimable(PlayersState::nbPlayers, road, player);
    }
    void PlayersState::display()
    {

        std::cout << "\n";
        std::cout << "====================================\n";
        std::cout << "         ÉTAT DES JOUEURS           \n";
        std::cout << "====================================\n";

        if (players.empty())
        {
            std::cout << "Aucun joueur dans la partie.\n";
            return;
        }

        for (std::shared_ptr<Player> player : players)
        {
            cout << "-----------------------------" << endl;
            player->display();
        }
    }

};
