#include "PlayersState.h"
#include <iostream>

using namespace std;

namespace playersState
{

    PlayersState::PlayersState()
    {
    }

    std::vector<std::shared_ptr<Player>> PlayersState::getPlayers()
    {
        return this->players;
    }
    void PlayersState::setPlayers(std::vector<std::shared_ptr<Player>> players)
    {
        this->players = players;
    }

    // FIXME : create a static getClaimableRoads method in dia
    std::vector<std::shared_ptr<mapState::Road>> getClaimableRoads(std::shared_ptr<mapState::MapState> map, std::shared_ptr<Player> player)
    {
        int nbPlayers = 3;/*this->players.size();*/ // FIXME : get number of players from PlayersState = create an static attribute nbPlayers
        return map->getClaimableRoads(nbPlayers, player); // FIXME : get number of players from PlayersState = create an static attribute nbPlayers
    } 
    // FIXME : Create a static isRoadBuildable method in dia
    /* static */ bool isRoadBuildable(std::shared_ptr<mapState::MapState> map, std::shared_ptr<mapState::Road> road, std::shared_ptr<Player> player)
    {
        if (!map || !road)
        {
            return false;
        }
        int nbPlayers = 3; // FIXME : get number of players from PlayersState = create an static attribute nbPlayers
        std::vector<std::shared_ptr<mapState::Road>> parallels = map->getRoadBetweenStations(road->getStationA(), road->getStationB());
        bool isClaimable = road->isClaimable(parallels, nbPlayers, player);
        if (!isClaimable)
        {
            std::cout << " Road between " << road->getStationA()->getName() << " and " << road->getStationB()->getName() << " is not claimable\n";
            return false;
        }
        cardsState::ColorCard roadColor = road->getColor();
        unsigned int roadLength = road->getLength();
        int nbWagons = player->getNbWagons();
        if (nbWagons < static_cast<int>(roadLength))
        {
            std::cout << " Not enough wagons. Needed: "
                      << roadLength << " wagons, you have: " << nbWagons << " remaining\n";
            return false;
        }
        std::vector<std::shared_ptr<cardsState::WagonCard>> handCards = player->getHand()->wagonCards->cards;
        int nbCards = handCards.size();
        if (nbCards < static_cast<int>(roadLength))
        {
            std::cout << " Not enough cards in hand. Needed: "
                      << roadLength << " cards, you have: " << nbCards << "\n";
            return false;
        }
        unsigned int correctCardCount = 0;
        for (const std::shared_ptr<cardsState::WagonCard> &card : handCards)
        {
            if (card->color == roadColor || card->color == cardsState::ColorCard::LOCOMOTIVE)
            {
                correctCardCount++;
            }
            if (roadLength == correctCardCount)
            {
                break;
            }
        }
        if (correctCardCount < roadLength)
        {
            std::cout << " Not enough cards of color " << static_cast<int>(roadColor)
                      << ". Needed: " << roadLength << ", you have: " << correctCardCount << "\n";
            return false;
        }

        return true;
    }
    void PlayersState::printPlayersState()
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
