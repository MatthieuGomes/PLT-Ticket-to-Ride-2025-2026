#include "PlayersState.h"
#include <iostream>

using namespace std;

namespace playersState {

    // ----- Constructor -----
    PlayersState::PlayersState() {
        // Nothing to initialize since there is no player list in the header
    }

    // ----- Print Players State -----
    void PlayersState::printPlayersState() {
        cout << "======= Players State =======" << endl;

        // Since PlayersState has no player list, we create temporary test players for display
        Player p1;
        p1.setId(1);
        p1.setName("Alice");
        p1.setNbStations(2);
        p1.setNbRoads(3);

        Player p2;
        p2.setId(2);
        p2.setName("Bob");
        p2.setNbStations(1);
        p2.setNbRoads(4);

        // Display info for each player
        cout << "-----------------------------" << endl;
        cout << "Player ID      : " << p1.getId() << endl;
        cout << "Name           : " << p1.getName() << endl;
        cout << "Stations owned : " << p1.getNbStations() << endl;
        cout << "Roads claimed  : " << p1.getNbRoads() << endl;

        cout << "-----------------------------" << endl;
        cout << "Player ID      : " << p2.getId() << endl;
        cout << "Name           : " << p2.getName() << endl;
        cout << "Stations owned : " << p2.getNbStations() << endl;
        cout << "Roads claimed  : " << p2.getNbRoads() << endl;

        cout << "=============================" << endl;
    }

};
