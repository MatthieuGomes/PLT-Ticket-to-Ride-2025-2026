#include "PlayersState.h"
#include <iostream>

using namespace std;

namespace playersState {

    PlayersState::PlayersState() {
    }

    void PlayersState::printPlayersState() {


        for (Player* player : players) {
            cout << "-----------------------------" << endl;
            cout << "Player: " << player->getName() << endl;
            player->display();
            player->displayHand();
            player->calculateDestinationPoints();
            player->getLongestPathLength();


        }
        cout << "-----------------------------" << endl;
    }

};
