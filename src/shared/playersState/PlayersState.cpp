#include "PlayersState.h"
#include <iostream>

using namespace std;

namespace playersState {

    PlayersState::PlayersState() {


    }


    void PlayersState::printPlayersState() {

        std::cout << "\n";
        std::cout << "====================================\n";
        std::cout << "         ÉTAT DES JOUEURS           \n";
        std::cout << "====================================\n";

        if (players.empty()) {
            std::cout << "Aucun joueur dans la partie.\n";
            return;
        }

        for (std::shared_ptr<Player> player : players) {
            cout << "-----------------------------" << endl;
            player->display();


        }



    }

};
