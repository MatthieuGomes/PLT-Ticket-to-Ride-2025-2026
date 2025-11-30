#include "Player.h"

#include <iostream>
#include <ostream>

#include "mapState/MapState.h"

namespace playersState {
   Player::Player(int id, std::string name, cardsState::ColorCard color, int score , int nbWagons , int nbStations , int nbRoads , cardsState::PlayerCards* hand) : id(id), name(name), color(color) , score(score), nbWagons(nbWagons), nbStations(nbStations) , nbRoads(nbRoads) ,  hand(hand)
   {
   }

   int Player::getId()
   {
      return id;
   }
    void Player::setId(int id) {
       this->id= id ;
    }
    std::string Player::getName() {
       return name;
    }
    void Player::setName(std::string name) {
      this->name= name;
    }
    int Player::getScore() {
     return score;
     }
    void Player::setScore(int score) {
        this->score= score;
    }
    int Player::getNbWagons()
   {
       return nbWagons;
   }

    void Player::setNbWagons(int nbWagons)
    {
       this->nbWagons = nbWagons;
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

   cardsState::ColorCard   Player::getColor() {
       return color;
   }
   void Player::setColor(cardsState::ColorCard color) {
        this->color= color;
   }
   cardsState::PlayerCards* Player::getHand() {
     return hand;}

   void Player::setHand(cardsState::PlayerCards* hand ) {
     this->hand = hand;
  }


   void Player::addRoad() {
      nbRoads++;
   }


   void Player::addStation() {
      nbStations++;
   }

   void Player::removeTrain(int nb) {
      if (nb > nbWagons) nb = nbWagons;
      nbWagons -= nb;
   }


   void Player::addScore(int points) {
      score += points;
   }

   void Player::addWagonCard(cardsState::WagonCard* w) {
      if (hand && hand->wagonCards)
         hand->wagonCards->addCard(w);
   }


   void Player::removeWagonCard(std::vector<cardsState::WagonCard*> cards) {

      if (!hand || !hand->wagonCards)
         return;


      for (auto* card : cards) {

         auto& deckCards = hand->wagonCards->cards;


         auto it = std::find_if(
             deckCards.begin(),
             deckCards.end(),
             [&](const std::shared_ptr<cardsState::WagonCard>& ptr) {
                 return ptr.get() == card;
             }
         );

         if (it != deckCards.end()) {
            int index = it - deckCards.begin();

            hand->wagonCards->removeCard(index);
         }
      }
   }

   void  Player::addDestinationCard (cardsState::DestinationCard* d)
   {
         if (hand && hand->destinationCards)
            hand->destinationCards->addCard(d);


   }

   void Player::chooseDestinationCards()
   {
      if (!hand || !hand->destinationCards) {
         std::cout << "No destination deck available.\n";
         return;
      }

      auto& cards = hand->destinationCards->cards;

      if (cards.empty()) {
         std::cout << "No destination cards to choose from.\n";
         return;
      }

      std::cout << "\nChoose your destination cards:\n";
      for (size_t i = 0; i < cards.size(); ++i) {
         std::cout << i << " -> ";
         cards[i]->display();
      }

      std::cout << "You must keep at least 1 card.\n";
      std::cout << "How many cards do you keep? ";

      int keepCount = 1;
      std::cin >> keepCount;

      if (keepCount < 1) keepCount = 1;
      if ((size_t)keepCount > cards.size()) keepCount = cards.size();

      std::vector<std::shared_ptr<cardsState::DestinationCard>> kept;

      std::cout << "Enter indices of cards to keep:\n";
      for (int k = 0; k < keepCount; ++k) {
         int index = 0;
         std::cin >> index;

         if (index >= 0 && (size_t)index < cards.size()) {
            kept.push_back(cards[index]);
         }
      }


      cards = kept;

      std::cout << "You kept " << kept.size() << " destination cards.\n";
   }

    int Player::calculateDestinationPoints() {
       return 0;
    }


   void Player::canBuildRoad (mapState::MapState map, std::string roadId)
   {

   }
   void Player::getCompletedDestinations (mapState::MapState map)
   {

   }

   int Player::getLongestPathLength(mapState::MapState) {
      return 0;
   }
   void Player::getClaimableRoads (mapState::MapState map)
   {

   }

   void Player::displayHand()
   {
      if (!hand) {
         std::cout << "Hand not initialized.\n";
         return;
      }

      std::cout << "\n=== PLAYER HAND ===\n";

      if (hand->wagonCards) {
         std::cout << "Wagon cards:\n";
         hand->wagonCards->display();
      }

      if (hand->destinationCards) {
         std::cout << "Destination cards:\n";
         hand->destinationCards->display();
      }

      std::cout << "====================\n";
   }



    void Player::display() {
       std::cout << "player name is " << name << std::endl;
       std::cout << "player id is " << id << std::endl;
       std::cout << "player color is " << color << std::endl;
       std::cout << "player score is " << score << std::endl;
       std::cout << "player nbWagons is " << nbWagons << std::endl;
       std::cout << "player nbStations is " << nbStations << std::endl;
       std::cout << "player nbRoads is " << nbRoads << std::endl;
      hand->display();
    }

}
