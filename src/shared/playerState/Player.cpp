#include "Player.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace playerState;

// ----- Getters & Setters -----

int Player::getId() {
    return id;
}

void Player::setId(int id) {
    this->id = id;
}

std::string Player::getName() {
    return name;
}

void Player::setName(std::string name) {
    this->name = name;
}

int Player::getScore() {
    return score;
}

void Player::setScore(int score) {
    this->score = score;
}

int Player::getNbStations() {
    return nbStations;
}

void Player::setNbStations(int nbStations) {
    this->nbStations = nbStations;
}

int Player::getNbRoads() {
    return nbRoads;
}

void Player::setNbRoads(int nbRoads) {
    this->nbRoads = nbRoads;
}

