#include "Player.h"

#include <iostream>
#include <ostream>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "mapState/MapState.h"

using namespace cardsState;
using namespace mapState;
namespace playersState
{
   Player::Player(int id, std::string name, ColorCard color, int score, int nbWagons, int nbStations, int nbRoads, PlayerCards *hand) : id(id), name(name), color(color), score(score), nbWagons(nbWagons), nbStations(nbStations), nbRoads(nbRoads), hand(hand)
   {
   }

   int Player::getId()
   {
      return id;
   }
   void Player::setId(int id)
   {
      this->id = id;
   }
   std::string Player::getName()
   {
      return name;
   }
   void Player::setName(std::string name)
   {
      this->name = name;
   }
   int Player::getScore()
   {
      return score;
   }
   void Player::setScore(int score)
   {
      this->score = score;
   }
   int Player::getNbWagons()
   {
      return nbWagons;
   }

   void Player::setNbWagons(int nbWagons)
   {
      this->nbWagons = nbWagons;
   }
   int Player::getNbStations()
   {
      return nbStations;
   }

   void Player::setNbStations(int nbStations)
   {
      this->nbStations = nbStations;
   }

   int Player::getNbRoads()
   {
      return nbRoads;
   }
   void Player::setNbRoads(int nbRoads)
   {
      this->nbRoads = nbRoads;
   }

   cardsState::ColorCard Player::getColor()
   {
      return color;
   }
   void Player::setColor(cardsState::ColorCard color)
   {
      this->color = color;
   }
   cardsState::PlayerCards *Player::getHand()
   {
      return hand;
   }

   void Player::setHand(cardsState::PlayerCards *hand)
   {
      this->hand = hand;
   }

   void Player::addRoad()
   {
      nbRoads++;
   }

   void Player::addStation()
   {
      nbStations++;
   }

   void Player::removeTrain(int nb)
   {
      if (nb > nbWagons)
         nb = nbWagons;
      nbWagons -= nb;
   }

   void Player::addScore(int points)
   {
      score += points;
   }

   int Player::calculateDestinationPoints()
   {
      int total = 0;
      for (auto *dest : completedDestinations)
      {
         if (!dest)
            continue;
         total += dest->getPoints();
      }
      addScore(total);
      return total;
   }

   bool Player::canBuildRoad(mapState::MapState *map, mapState::Station *u, mapState::Station *v)
   {
      if (!map || !u || !v)
      {
         return false;
      }

      std::shared_ptr<mapState::Station> sharedU = map->getStationByName(u->getName());
      std::shared_ptr<mapState::Station> sharedV = map->getStationByName(v->getName());
      if (!sharedU || !sharedV)
      {
         return false;
      }

      std::shared_ptr<mapState::Road> road = map->getRoadBetweenStations(sharedU, sharedV);
      if (!road)
      {
         std::cout << " No road exists between "
                   << u->getName() << " and " << v->getName() << "\n";
         return false;
      }

      if (road->getBlockStatus())
      {
         std::cout << " The road is blocked.\n";
         return false;
      }
      if (road->getOwner() != nullptr)
      {
         std::cout << " The road is already owned by another player.\n";
         return false;
      }
      long unsigned int length = road->getLength();
      if (nbWagons < length)
      {
         std::cout << " Not enough wagons. Needed: "
                   << length << " wagons, you have: " << nbWagons << " remaining\n";
         return false;
      }

      cardsState::ColorCard requiredColor = road->getColor();

      std::vector<std::shared_ptr<cardsState::WagonCard>> cardsCorrectColor;

      for (std::shared_ptr<cardsState::WagonCard> card : hand->wagonCards->cards)
      {
         if (card->color == requiredColor || requiredColor == cardsState::ColorCard::LOCOMOTIVE)
         {
            cardsCorrectColor.push_back(card);
         }
      }
      if (cardsCorrectColor.size() < length)
      {
         std::cout << " Not enough cards of color " << requiredColor
                   << ". Needed: " << length << ", you have: " << cardsCorrectColor.size() << "\n";
         return false;
      }

      std::cout << " Player can build road between "
                << u->getName() << " and " << v->getName() << "\n";

      return true;
   }

   int Player::getLongestPathLength(mapState::MapState *map)
   {
      if (!map)
         return 0;


      std::unordered_map<mapState::Station *, std::vector<std::pair<mapState::Station *, int>>> adj;
      for (auto *road : map->roads)
      {
         if (!road)
            continue;
         if (road->getOwner() != this)
            continue;
         mapState::Station *a = road->getStationA();
         mapState::Station *b = road->getStationB();
         if (!a || !b)
            continue;
         adj[a].push_back({b, road->getLength()});
         adj[b].push_back({a, road->getLength()});
      }

      if (adj.empty())
         return 0;


      std::function<int(mapState::Station *, std::unordered_set<mapState::Station *> &)> dfs;
      dfs = [&](mapState::Station *current, std::unordered_set<mapState::Station *> &visited) -> int {
         int best = 0;
         auto it = adj.find(current);
         if (it == adj.end())
            return 0;
         for (auto &p : it->second)
         {
            mapState::Station *nbr = p.first;
            int w = p.second;
            if (visited.find(nbr) != visited.end())
               continue;
            visited.insert(nbr);
            int cand = w + dfs(nbr, visited);
            if (cand > best)
               best = cand;
            visited.erase(nbr);
         }
         return best;
      };

      int longest = 0;

      for (auto &kv : adj)
      {
         mapState::Station *start = kv.first;
         std::unordered_set<mapState::Station *> visited;
         visited.insert(start);
         int len = dfs(start, visited);
         if (len > longest)
            longest = len;
      }

      return longest;
   }

   std::vector<mapState::Road *> Player::getClaimableRoads(mapState::MapState *map)
   {
      std::vector<mapState::Road *> claimable;

      if (!map)
         return claimable;
      for (const std::shared_ptr<mapState::Road> &road : map->roads)
      {

         if (!road)
            continue;
         if (road->getBlockStatus())
            continue;

         mapState::Station *u = nullptr;
         mapState::Station *v = nullptr;

         for (const std::shared_ptr<mapState::Station> &s : map->stations)
         {
            if (!s)
               continue;
            if (s == road->stationA)
               u = s.get();
            else if (s == road->stationB)
               v = s.get();
            if (u && v)
               break;
         }

         if (!u || !v)
            continue;

         if (canBuildRoad(map, u, v))
         {
            claimable.push_back(road.get());
         }
      }
      return claimable;
   }

   void Player::displayHand()
   {
      if (!hand)
      {
         std::cout << "Hand not initialized.\n";
         return;
      }

      std::cout << "\n=== PLAYER HAND ===\n";

      if (hand->wagonCards)
      {
         std::cout << "Wagon cards:\n";
         hand->wagonCards->display();
      }

      if (hand->destinationCards)
      {
         std::cout << "Destination cards:\n";
         hand->destinationCards->display();
      }

      std::cout << "====================\n";
   }

   void Player::display()
   {
      std::cout << "player name is " << name << std::endl;
      std::cout << "player id is " << id << std::endl;
      std::cout << "player color is " << color << std::endl;
      std::cout << "player score is " << score << std::endl;
      std::cout << "player nbWagons is " << nbWagons << std::endl;
      std::cout << "player nbStations is " << nbStations << std::endl;
      std::cout << "player nbRoads is " << nbRoads << std::endl;
      hand->display();
   }

   template <class CardType>
   void Player::drawCard(cardsState::CardsState* cardsState, CardType *card, int number)
   {
      hand->takeCard<CardType>(cardsState, card, number);
   }

}
