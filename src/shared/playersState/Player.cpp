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

#define TEMP_NB_PLAYERS 4
namespace playersState
{
   Player::Player(int id, std::string name, ColorCard color, int score, int nbWagons, int nbStations, int nbRoads, std::shared_ptr<cardsState::PlayerCards> hand) : id(id), name(name), color(color), score(score), nbWagons(nbWagons), nbStations(nbStations), nbRoads(nbRoads), hand(hand)
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
   std::shared_ptr<cardsState::PlayerCards> Player::getHand()
   {
      return hand;
   }

   void Player::setHand(std::shared_ptr<cardsState::PlayerCards> hand)
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
      this->score += points;
   }

   int Player::calculateDestinationPoints()
   {
      int total = 0;
      for (auto dest : completedDestinations)
      {
         if (!dest)
            continue;
         total += dest->getPoints();
      }
      addScore(total);
      return total;
   }
   // FIXME : Use new isRoadBuildable method from MapState 
   // !!! point to road now, not stations + 
   // !!! warning on different types of roads (use typid to test if it's a road, a tunnel or a ferry and change the methodes accordingly)
   bool Player::canBuildRoad(std::shared_ptr<mapState::MapState> map, std::shared_ptr<mapState::Station> u, std::shared_ptr<mapState::Station> v)
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

      std::vector<std::shared_ptr<mapState::Road>> roads = map->getRoadBetweenStations(u, v);
      if (roads.empty())
      {
         return false;
         std::cout << " No road exists between "
                   << u->getName() << " and " << v->getName() << "\n";
         return false;
      }
      
      return true;
      // long unsigned int length = road->getLength();
      // if (nbWagons < length)
      // {
      //    std::cout << " Not enough wagons. Needed: "
      //              << length << " wagons, you have: " << nbWagons << " remaining\n";
      //    return false;
      // }

      // cardsState::ColorCard requiredColor = road->getColor();

      // std::vector<std::shared_ptr<cardsState::WagonCard>> cardsCorrectColor;

      // for (std::shared_ptr<cardsState::WagonCard> card : hand->wagonCards->cards)
      // {
      //    if (card->color == requiredColor || card->color == cardsState::ColorCard::LOCOMOTIVE)
      //    {
      //       cardsCorrectColor.push_back(card);
      //    }
      // }
      // if (cardsCorrectColor.size() < length)
      // {
      //    std::cout << " Not enough cards of color " << requiredColor
      //              << ". Needed: " << length << ", you have: " << cardsCorrectColor.size() << "\n";
      //    return false;
      // }

      // std::cout << " Player can build road between "
      //           << u->getName() << " and " << v->getName() << "\n";

      return true;
   }
   int Player::getLongestPathLength(std::shared_ptr<mapState::MapState> map)
   {
      if (!map)
         return 0;


      std::unordered_map<std::shared_ptr<mapState::Station>, std::vector<std::pair<std::shared_ptr<mapState::Station>, int>>> adj;
      for (std::shared_ptr<mapState::Road> road : map->roads)
      {
         if (!road)
            continue;
         if (road->getOwner() != std::make_shared<Player>(*this))
            continue;
         std::shared_ptr<mapState::Station> a = road->getStationA();
         std::shared_ptr<mapState::Station> b = road->getStationB();
         if (!a || !b)
            continue;
         adj[a].push_back({b, road->getLength()});
         adj[b].push_back({a, road->getLength()});
      }

      if (adj.empty())
         return 0;


      std::function<int(std::shared_ptr<mapState::Station>, std::unordered_set<std::shared_ptr<mapState::Station>> &)> dfs;
      dfs = [&](std::shared_ptr<mapState::Station> current, std::unordered_set<std::shared_ptr<mapState::Station>> &visited) -> int {
         int best = 0;
         auto it = adj.find(current);
         if (it == adj.end())
            return 0;
         for (std::pair<std::shared_ptr<mapState::Station>, int> p : it->second)
         {
            std::shared_ptr<mapState::Station> nbr = p.first;
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

      for (std::pair<std::shared_ptr<mapState::Station>, std::vector<std::pair<std::shared_ptr<mapState::Station>, int>>> kv : adj)
      {
         std::shared_ptr<mapState::Station> start = kv.first;
         std::unordered_set<std::shared_ptr<mapState::Station>> visited;
         visited.insert(start);
         int len = dfs(start, visited);
         if (len > longest)
            longest = len;
      }

      return longest;
   }
   // FIXME : Use new getClaimableRoads method from PlayerState !!! point to road now, not stations
   std::vector<std::shared_ptr<mapState::Road>> Player::getClaimableRoads(std::shared_ptr<mapState::MapState> map)
   {
      std::vector<std::shared_ptr<mapState::Road>> claimable={};

      if (!map)
         return claimable;
      for (std::shared_ptr<mapState::Road> road : map->roads)
      {

         if (!road)
            continue;
         if (road->getBlockStatus())
            continue;

         std::shared_ptr<mapState::Station> u = nullptr;
         std::shared_ptr<mapState::Station> v = nullptr;

         for (std::shared_ptr<mapState::Station> s : map->stations)
         {
            if (!s)
               continue;
            if (s == road->stationA)
               u = s;
            else if (s == road->stationB)
               v = s;
            if (u && v)
               break;
         }

         if (!u || !v)
            continue;

         if (canBuildRoad(map, u, v))
         {
            claimable.push_back(road);
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
   void Player::drawCard(std::shared_ptr<cardsState::CardsState> cardsState, std::shared_ptr<CardType> card, int number)
   {
      hand->takeCard<CardType>(cardsState, card, number);
   }

}
