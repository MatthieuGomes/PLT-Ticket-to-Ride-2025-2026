#include "Player.h"
#include <iostream>
#include <algorithm>

using namespace std;

namespace playersState {

  // ----- Getters -----

  int Player::getId() {
    return id;
  }

  string Player::getName() {
    return name;
  }

  int Player::getNbStations() {
    return nbStations;
  }

  int Player::getNbRoads() {
    return nbRoads;
  }
