#include "Player.h"

namespace playersState {
   Player::Player(int id, std::string name, cardsState::ColorCard color, cardsState::PlayerCards hand) : id(id), name(name), color(color), hand(hand)
   {
   }
    void Player::setId(int id) {
    }
    std::string Player::getName() {
    }
    void Player::setName(std::string name) {
    }
    int Player::getNumTrains()
    {}

    void Player::setNumTrains(int numTrains)
    {
    }
    int Player::getNbStations() {
    }

    void Player::setNbStations(int nbStations) {
    }

    int Player::getNbRoads() {
    }
    void Player::setNbRoads(int nbRoads) {
    }

    void Player::addCardToHand(cardsState::WagonCard* c) {
    }

    void Player::removeCardsFromHand(std::vector<cardsState::WagonCard*> cards) {
    }

    void Player::displayHand() {
    }

    void Player::addDestination(cardsState::DestinationCard* d) {
    }

    std::vector<cardsState::DestinationCard*> Player::getDestinations() {
    }

    int Player::calculateDestinationPoints() {
    }

    int Player::getLongestPathLength() {
    }

    void Player::display() {
    }

}