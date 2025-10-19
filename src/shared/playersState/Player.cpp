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

  // ----- Setters -----

  void Player::setId(int id) {
    this->id = id;
  }

  void Player::setName(string name) {
    this->name = name;
  }

  void Player::setNbStations(int nbStations) {
    this->nbStations = nbStations;
  }

  void Player::setNbRoads(int nbRoads) {
    this->nbRoads = nbRoads;
  }

  // ----- Gameplay Methods -----

  void Player::drawCard(string deck) {
    wagonDeck.push_back(deck);
    cout << name << " pioche une carte : " << deck << endl;
  }

  void Player::playCard(string card) {
    auto it = find(wagonDeck.begin(), wagonDeck.end(), card);
    if (it != wagonDeck.end()) {
      wagonDeck.erase(it);
      cout << name << " joue la carte : " << card << endl;
    } else {
      cout << name << " ne possède pas la carte : " << card << endl;
    }
  }

  void Player::displayDeck() {
    cout << "Main de " << name << " :" << endl;
    if (wagonDeck.empty()) {
      cout << "  Aucune carte." << endl;
      return;
    }
    for (const auto &card : wagonDeck) {
      cout << "  - " << card << endl;
    }
  }

  void Player::claimRoad(string road) {
    nbRoads++;
    cout << name << " revendique la route : " << road << endl;
  }

  void Player::moveToStation(string station) {
    nbStations++;
    cout << name << " se déplace vers la station : " << station << endl;
  }

  void Player::updateCardState(string card) {
    cout << name << " met à jour l'état de la carte : " << card << endl;
  }

  vector<string> Player::checkAvailableMoves() {
    vector<string> moves;
    if (!wagonDeck.empty()) {
      moves.push_back("Jouer une carte");
    }
    moves.push_back("Piocher une carte");
    moves.push_back("Revendique une route");
    moves.push_back("Se déplacer vers une station");

    cout << "Mouvements disponibles pour " << name << " :" << endl;
    for (const auto &move : moves) {
      cout << "  - " << move << endl;
    }

    return moves;
  }

};
