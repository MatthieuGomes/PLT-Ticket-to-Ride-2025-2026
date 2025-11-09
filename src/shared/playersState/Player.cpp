#include "Player.h"

#include <iostream>
#include <ostream>

namespace playersState {
   Player::Player(int id, std::string name, cardsState::ColorCard color, int score , int numTrains , int nbStations , int nbRoads , cardsState::PlayerCards* hand) : id(id), name(name), color(color) , score(score), numTrains(numTrains), nbStations(nbStations) , nbRoads(nbRoads) ,  hand(hand)
   {
   }
   int Player::getId()
   {
      return id;
   }
    void Player::setId(int id) {
       id= id ;
    }
    std::string Player::getName() {
       return name;
    }
    void Player::setName(std::string name) {
      name= name;
    }
    int Player::getNumTrains()
   {
       return numTrains;
   }

    void Player::setNumTrains(int numTrains)
    {
       numTrains = numTrains;
    }
    int Player::getNbStations() {
       return nbStations;
    }

    void Player::setNbStations(int nbStations) {
      nbStations = nbStations;
    }

    int Player::getNbRoads() {
      return nbRoads;
    }
    void Player::setNbRoads(int nbRoads) {
        nbRoads = nbRoads;
    }

    void Player::addCardToHand(cardsState::WagonCard* c) {

       std::cout << "addCardToHand" << std::endl;
    }

    void Player::removeCardsFromHand(std::vector<cardsState::WagonCard*> cards) {

      std::cout << "removeCardsFromHand" << std::endl;
    }

    void Player::displayHand() {
       std::cout << "displayHand" << std::endl;
    }




    int Player::calculateDestinationPoints() {
       return 0;
    }

    int Player::getLongestPathLength() {
    }

    void Player::display() {
       std::cout << "player name is " << name << std::endl;
       std::cout << "player id is " << id << std::endl;
       std::cout << "player color is " << color << std::endl;
       std::cout << "player score is " << score << std::endl;
       std::cout << "player numTrains is " << numTrains << std::endl;
       std::cout << "player nbStations is " << nbStations << std::endl;
       std::cout << "player nbRoads is " << nbRoads << std::endl;
      hand->display();
    }

}
