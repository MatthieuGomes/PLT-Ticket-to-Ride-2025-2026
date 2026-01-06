#include "Player.h"

#include <iostream>
#include <ostream>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "PlayersState.h"

#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "mapState/Station.h"
#include "mapState/Tunnel.h"
#include "mapState/Ferry.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

using namespace cardsState;
using namespace mapState;

namespace playersState
{
   using playersInfos = std::tuple<std::string, PlayerColor, int, int, int, int, std::shared_ptr<cardsState::PlayerCards>>;
   using playersInitInfos = std::tuple<std::string, PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

   int Player::startScore = 0;
   int Player::startNbWagons = 45;
   int Player::startNbStations = 3;

   PlayerColor Player::possibleColors[6] = {PlayerColor::RED,
                                            PlayerColor::BLUE,
                                            PlayerColor::GREEN,
                                            PlayerColor::BLACK,
                                            PlayerColor::YELLOW,
                                            PlayerColor::UNKNOWN};

   Player::Player()
   {
      this->score = -1;
      this->nbWagons = -1;
      this->nbStations = -1;
      this->nbRoads = -1;
      this->color = PlayerColor::RED; // Temporary
   }

   Player::Player(std::string name, PlayerColor color, int score, int nbWagons, int nbStations, int nbRoads, std::shared_ptr<cardsState::PlayerCards> hand) : name(name), color(color), score(score), nbWagons(nbWagons), nbStations(nbStations), nbRoads(nbRoads), hand(hand)
   {
   }

   Player Player::Init(std::string name, PlayerColor color, std::shared_ptr<cardsState::PlayerCards> hand)
   {
      return Player(name, color, Player::startScore, Player::startNbWagons, Player::startNbStations, 0, hand);
   }

   void Player::fillFromInfos(playersInfos info)
   {
      this->name = std::get<0>(info);
      this->color = std::get<1>(info);
      this->score = std::get<2>(info);
      this->nbWagons = std::get<3>(info);
      this->nbStations = std::get<4>(info);
      this->nbRoads = std::get<5>(info);
      this->hand = std::get<6>(info);
   }

   void Player::fillFromInitInfos(playersInitInfos info)
   {
      this->name = std::get<0>(info);
      this->color = std::get<1>(info);
      this->hand = std::get<2>(info);
      this->score = Player::startScore;
      this->nbWagons = Player::startNbWagons;
      this->nbStations = Player::startNbStations;
      this->nbRoads = 0;
   }

   Player Player::PlayerFromInfos(playersInfos info)
   {
      Player player;
      player.fillFromInfos(info);
      return player;
   }

   Player Player::PlayerFromInitInfos(playersInitInfos info)
   {
      Player player;
      player.fillFromInitInfos(info);
      return player;
   }

   std::vector<std::shared_ptr<Player>> Player::BatchFromInfos(std::vector<playersInfos> infos)
   {
      std::vector<std::shared_ptr<Player>> players;
      for (playersInfos info : infos)
      {
         players.push_back(std::make_shared<Player>(Player::PlayerFromInfos(info)));
      }
      return players;
   }
   std::vector<std::shared_ptr<Player>> Player::BatchFromInitInfos(std::vector<playersInitInfos> infos)
   {
      std::vector<std::shared_ptr<Player>> players;
      for (playersInitInfos info : infos)
      {
         players.push_back(std::make_shared<Player>(Player::PlayerFromInitInfos(info)));
      }
      return players;
   }

   std::string Player::getName()
   {
      return name;
   }
   // FIXME ? Might not be usefull
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

   // FIXME ? What is nbroads useful for ?
   int Player::getNbRoads()
   {
      return nbRoads;
   }
   void Player::setNbRoads(int nbRoads)
   {
      this->nbRoads = nbRoads;
   }

   PlayerColor Player::getColor()
   {
      return color;
   }
   // FIXME ? Might not be usefull
   void Player::setColor(PlayerColor color)
   {
      this->color = color;
   }

   std::shared_ptr<cardsState::PlayerCards> Player::getHand()
   {
      return hand;
   }

   // FIXME ? Might not be usefull
   void Player::setHand(std::shared_ptr<cardsState::PlayerCards> hand)
   {
      this->hand = hand;
   }
   // FIXME ? Might not be usefull
   void Player::addRoad()
   {
      nbRoads++;
   }
   // what ?
   void Player::addStation()
   {
      nbStations++;
   }

   void Player::removeTrain(int num)
   {
      if (num > this->nbWagons)
         throw std::invalid_argument("Cannot remove more wagons than available.");
      this->nbWagons = this->nbWagons - num;
   }
   // TODO : add tests
   void Player::removeStation(int nb)
   {
      if (nb > this->nbStations)
         throw std::invalid_argument("Cannot remove more stations than available.");
      this->nbStations -= nb;
   }

   void Player::addScore(int points)
   {
      this->score += points;
   }

   // Missing a lot of logic here
   int Player::calculateDestinationPoints()
   {
      int total = 0;
      for (std::shared_ptr<cardsState::DestinationCard> dest : completedDestinations)
      {
         total += dest->getPoints();
      }
      addScore(total);
      return total;
   }

   bool Player::isRoadBuildable(std::shared_ptr<mapState::MapState> map, std::shared_ptr<mapState::Road> road)
   {
      if (!playersState::PlayersState::isRoadClaimable(map, road, std::make_shared<Player>(*this)))
      {
         DEBUG_PRINT("Road is not claimable.\n");
         return false;
      }
      if (typeid(*road) == typeid(mapState::Tunnel))
      {
         std::shared_ptr<mapState::Tunnel> tunnel = std::dynamic_pointer_cast<mapState::Tunnel>(road);
         // FIXME : implement Tunnel specific logic here BUT first implement the cards functions to make it possible
         return true;
      }
      else
      {
         int length = road->getLength();
         if (this->nbWagons < length)
         {
            DEBUG_PRINT("Not enough wagons. Needed: "
                        << length << " wagons, you have: " << this->nbWagons << " remaining\n");
            return false;
         }
         RoadColor requiredColor = road->getColor();

         std::vector<std::shared_ptr<cardsState::WagonCard>> cardsCorrectColor;
         std::vector<std::shared_ptr<cardsState::WagonCard>> handCards = this->hand->wagonCards->cards;
         std::vector<std::shared_ptr<cardsState::WagonCard>> handCardsCopy = std::vector<std::shared_ptr<cardsState::WagonCard>>(handCards);

         if (RoadColor::NONE == requiredColor)
         {
            std::vector<std::shared_ptr<cardsState::WagonCard>> longestSet;
            std::unordered_map<cardsState::ColorCard, std::vector<std::shared_ptr<cardsState::WagonCard>>> colorMap;
            for (std::shared_ptr<cardsState::WagonCard> card : handCards)
            {
               cardsState::ColorCard color = card->getColor();
               if (colorMap.find(color) == colorMap.end())
               {
                  colorMap[color] = std::vector<std::shared_ptr<cardsState::WagonCard>>();
                  handCardsCopy.erase(std::remove(handCardsCopy.begin(), handCardsCopy.end(), card), handCardsCopy.end());
               }
               colorMap[color].push_back(card);
            }

            // TODO : get rid of it
            std::vector<cardsState::ColorCard> colorsOrder = {
                cardsState::ColorCard::RED,
                cardsState::ColorCard::BLUE,
                cardsState::ColorCard::GREEN,
                cardsState::ColorCard::WHITE,
                cardsState::ColorCard::BLACK,
                cardsState::ColorCard::YELLOW,
                cardsState::ColorCard::PINK,
                cardsState::ColorCard::ORANGE,
                cardsState::ColorCard::LOCOMOTIVE};

            for (cardsState::ColorCard color : colorsOrder /*cardsState::DestinationCard::possibleColors*/)
            {
               std::vector<std::shared_ptr<cardsState::WagonCard>> cardsOfOneColor = colorMap[color];
               if (static_cast<int>(cardsOfOneColor.size()) > static_cast<int>(longestSet.size()))
               {
                  longestSet = cardsOfOneColor;
               }
            }
            cardsCorrectColor = longestSet;
            handCardsCopy = std::vector<std::shared_ptr<cardsState::WagonCard>>(handCards);
            for (std::shared_ptr<cardsState::WagonCard> card : cardsCorrectColor)
            {
               handCardsCopy.erase(std::remove(handCardsCopy.begin(), handCardsCopy.end(), card), handCardsCopy.end());
            }
         }
         else
         {
            for (std::shared_ptr<cardsState::WagonCard> card : handCards)
            {
               if (static_cast<int>(cardsCorrectColor.size()) >= length)
               {
                  break;
               }
               else if (card->getColor() == requiredColor)
               {
                  cardsCorrectColor.push_back(card);
                  handCardsCopy.erase(std::remove(handCardsCopy.begin(), handCardsCopy.end(), card), handCardsCopy.end());
               }
            }
         }
         for (std::shared_ptr<cardsState::WagonCard> card : handCards)
         {
            if (static_cast<int>(cardsCorrectColor.size()) >= length)
            {
               break;
            }
            if (card->color == cardsState::ColorCard::LOCOMOTIVE)
            {
               cardsCorrectColor.push_back(card);
               handCardsCopy.erase(std::remove(handCardsCopy.begin(), handCardsCopy.end(), card), handCardsCopy.end());
            }
         }
         if (static_cast<int>(cardsCorrectColor.size()) < length)
         {
            DEBUG_PRINT(" Not enough cards of color " << requiredColor
                                                      << ". Needed: " << length << ", you have: " << cardsCorrectColor.size() << "\n");
            return false;
         }
         if (typeid(*road) == typeid(mapState::Ferry))
         {
            std::shared_ptr<mapState::Ferry> ferry = std::dynamic_pointer_cast<mapState::Ferry>(road);
            int requiredLocomotives = ferry->getLocomotives();
            std::vector<std::shared_ptr<cardsState::WagonCard>> remainingLocomotiveCards;
            for (std::shared_ptr<cardsState::WagonCard> card : handCardsCopy)
            {
               if (card->color == cardsState::ColorCard::LOCOMOTIVE)
               {
                  remainingLocomotiveCards.push_back(card);
               }
            }
            if (static_cast<int>(remainingLocomotiveCards.size()) < requiredLocomotives)
            {
               DEBUG_PRINT(" Not enough locomotive cards. Needed: " << requiredLocomotives
                                                                    << ", you have: " << remainingLocomotiveCards.size() << "\n");
               return false;
            }
            return true;
         }
         else
         {
            return true;
         }
      }
   }
   // TODO : Why is it here ?
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
      dfs = [&](std::shared_ptr<mapState::Station> current, std::unordered_set<std::shared_ptr<mapState::Station>> &visited) -> int
      {
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
   std::vector<std::shared_ptr<mapState::Road>> Player::getClaimableRoads(std::shared_ptr<mapState::MapState> map)
   {
      return playersState::PlayersState::getClaimableRoads(map, std::make_shared<Player>(*this));
   }

   void Player::display()
   {
      std::cout << "player name is " << name << std::endl;
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
