#include "Player.h"

#include <iostream>
#include <ostream>

#include "mapState/MapState.h"

using namespace cardsState;

namespace playersState {
   Player::Player(int id, std::string name, ColorCard color, int score , int nbWagons , int nbStations , int nbRoads , PlayerCards* hand) : id(id), name(name), color(color) , score(score), nbWagons(nbWagons), nbStations(nbStations) , nbRoads(nbRoads) ,  hand(hand)
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

   /*
    *
   * void Player::chooseDestinationCards()
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
    */
    int Player::calculateDestinationPoints() {

       return 0;
    }

   bool Player::canBuildRoad(mapState::MapState* map, mapState::Station* u, mapState::Station* v)
   {
      mapState::Road* road = map->getRoadBetweenStations(u, v);
      if (!road) {
         std::cout << " No road exists between "
                   << u->getName() << " and " << v->getName() << "\n";
         return false;
      }

      

      if (road->getBlockStatus()) {
         std::cout << " The road is blocked.\n";
         return false;
      }
      if (road->getOwner() != nullptr) {
         std::cout << " The road is already owned by another player.\n";
         return false;
      }
      long unsigned int length = road->getLength();
      if (nbWagons < length) {
         std::cout << " Not enough wagons. Needed: "
                   << length << " wagons, you have: " << nbWagons << " remaining\n";
         return false;
      }

      cardsState::ColorCard requiredColor = road->getColor();

      std::vector<std::shared_ptr<cardsState::WagonCard>> cardsCorrectColor;

      for (std::shared_ptr<cardsState::WagonCard> card : hand->wagonCards->cards){
         if (card->color == requiredColor || requiredColor == cardsState::ColorCard::LOCOMOTIVE){
            cardsCorrectColor.push_back(card);
         }
      }
      if (cardsCorrectColor.size() < length) {
         std::cout << " Not enough cards of color " << requiredColor
                   << ". Needed: " << length << ", you have: " << cardsCorrectColor.size() << "\n";
         return false;
      }

      std::cout << " Player can build road between "
                << u->getName() << " and " << v->getName() << "\n";

     return true;
   }

   template<class CardType>
   void Player::takeCard(cardsState::CardsState* state) {
       if constexpr (std::is_same<CardType, cardsState::WagonCard>::value) {
           if (state && state->gameWagonCards && state->gameWagonCards->faceDownCards) {
               auto card = state->gameWagonCards->faceDownCards->removeCard(state->gameWagonCards->faceDownCards->countCards() - 1);
               if (card) {
                   hand->wagonCards->cards.push_back(std::shared_ptr<cardsState::WagonCard>(card));
               }
           }
       } else if constexpr (std::is_same<CardType, cardsState::DestinationCard>::value) {
           if (state && state->gameDestinationCards && state->gameDestinationCards->faceDownCards) {
               std::vector<std::shared_ptr<cardsState::DestinationCard>> drawnCards;
               for (int i = 0; i < 3 && state->gameDestinationCards->faceDownCards->countCards() > 0; ++i) {
                   auto card = state->gameDestinationCards->faceDownCards->removeCard(state->gameDestinationCards->faceDownCards->countCards() - 1);
                   if (card) {
                       drawnCards.push_back(std::shared_ptr<cardsState::DestinationCard>(card));
                   }
               }

               if (!drawnCards.empty()) {
                   auto bestCard = *std::max_element(drawnCards.begin(), drawnCards.end(), [](const auto& a, const auto& b) {
                       return a->points < b->points;
                   });

                   hand->destinationCards->cards.push_back(bestCard);

                   for (const auto& card : drawnCards) {
                       if (card != bestCard) {
                           state->gameDestinationCards->faceDownCards->cards.insert(
                               state->gameDestinationCards->faceDownCards->cards.begin(), card
                           );
                       }
                   }
               }
           }
       } else {
           std::cerr << "Unsupported card type." << std::endl;
       }
   }

/*
   void Player::getCompletedDestinations(mapState::MapState* map)
   {
      if (!map) {
         std::cout << " Map is null.\n";
         return;
      }
      if (!hand) {
         std::cout << " Player has no hand.\n";
         return;
      }

      std::cout << "Checking completed destinations for player: "
                << this->name << "\n";

      for (const auto& destCardPtr : hand->destinationCards->cards ) {

         if (!destCardPtr) continue;
         // problème destination card city a , b  doit etre station
         mapState::Station* start = destCardPtr->start;
         mapState::Station* end   = destCardPtr->end;

         std::cout << "- Destination " << start->data->name
                   << " → " << end->data->name << " ... ";


         mapState::Path path = map->findShortestPath(start, end);

         if (path.STATIONS.size() <= 1) {
            std::cout << " Not connected.\n";
            continue;
         }


         bool ok = true;

         for (int i = 0; i < path.STATIONS.size() - 1; i++) {
            auto* u = path.STATIONS[i];
            auto* v = path.STATIONS[i + 1];


            mapState::Road* r = map->getRoad(u, v);
            if (!r) {
               ok = false;
               break;
            }

            if (r->data->isBlocked) {
               ok = false;
               break;
            }
         }

         if (ok) {
            std::cout << " COMPLETED\n";
            completedDestinations.push_back(destCardPtr);
         } else {
            std::cout << " NOT completed\n";
         }
      }
   }
*/
   int Player::getLongestPathLength(mapState::MapState* map) {
      return 0;
   }


   std::vector<mapState::Road*> Player::getClaimableRoads(mapState::MapState* map)
    {
       std::vector<mapState::Road*> claimable;

      if (!map) return claimable;
      for (mapState::Road* road : map->roads) {

         if (!road) continue;
         if (road->isBlocked) continue;
         if (road->stationA==nullptr || road->stationB==nullptr) continue;

         mapState::Station* u = nullptr;
         mapState::Station* v = nullptr;


         for (auto* station : map->stations) {
            if (!station) continue;
            if (station == road->stationA) u = station;
            else if (station == road->stationB) v = station;
            if (u && v) break;
         }

         if (!u || !v) continue;

         if (canBuildRoad(map, u, v)) {
            claimable.push_back(road);
         }
      }
       return claimable;
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

