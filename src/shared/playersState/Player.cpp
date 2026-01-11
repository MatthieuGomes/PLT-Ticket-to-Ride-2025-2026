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
   using PlayersInfos = std::tuple<std::string, PlayerColor, int, int, int, std::vector<std::shared_ptr<mapState::Road>>, std::shared_ptr<cardsState::PlayerCards>>;
   using PlayersInitInfos = std::tuple<std::string, PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;

   int Player::startScore = 0;
   int Player::startNbWagons = 45;
   int Player::startNbStations = 3;

   std::unordered_map<int,int> Player::PointsByRoadLength = {
      {1,1},
      {2,2},
      {3,4},
      {4,7},
      {5,10},
      {6,15}
   };

   PlayerColor Player::possibleColors[6] = {PlayerColor::RED,
                                            PlayerColor::BLUE,
                                            PlayerColor::GREEN,
                                            PlayerColor::BLACK,
                                            PlayerColor::YELLOW,
                                            PlayerColor::UNKNOWN};

   std::unordered_map<PlayerColor, std::string> Player::ColorsNames = {
       {PlayerColor::RED, "RED"},
       {PlayerColor::BLUE, "BLUE"},
       {PlayerColor::GREEN, "GREEN"},
       {PlayerColor::BLACK, "BLACK"},
       {PlayerColor::YELLOW, "YELLOW"},
       {PlayerColor::UNKNOWN, "UNKNOWN"}};

   Player::Player()
   {
      this->score = -1;
      this->nbWagons = -1;
      this->nbStations = -1;
      this->color = PlayerColor::UNKNOWN;
   }

   Player::Player(std::string name, PlayerColor color, int score, int nbWagons, int nbStations, std::vector<std::shared_ptr<mapState::Road>> borrowedRoads, std::shared_ptr<cardsState::PlayerCards> hand) : name(name), color(color), score(score), nbWagons(nbWagons), nbStations(nbStations), borrowedRoads(borrowedRoads), hand(hand)
   {
   }

   Player Player::Init(std::string name, PlayerColor color, std::shared_ptr<cardsState::PlayerCards> hand)
   {
      return Player(name, color, Player::startScore, Player::startNbWagons, Player::startNbStations, {}, hand);
   }

   void Player::fillFromInfos (PlayersInfos info)
   {
      this->name = std::get<0>(info);
      this->color = std::get<1>(info);
      this->score = std::get<2>(info);
      this->nbWagons = std::get<3>(info);
      this->nbStations = std::get<4>(info);
      this->borrowedRoads = std::get<5>(info);
      this->hand = std::get<6>(info);
   }

   void Player::fillFromInitInfos(PlayersInitInfos info)
   {
      this->name = std::get<0>(info);
      this->color = std::get<1>(info);
      this->hand = std::get<2>(info);
      this->score = Player::startScore;
      this->nbWagons = Player::startNbWagons;
      this->nbStations = Player::startNbStations;
   }

   Player Player::PlayerFromInfos(PlayersInfos info)
   {
      Player player;
      player.fillFromInfos(info);
      return player;
   }

   Player Player::PlayerFromInitInfos(PlayersInitInfos info)
   {
      Player player;
      player.fillFromInitInfos(info);
      return player;
   }

   std::vector<std::shared_ptr<Player>> Player::BatchFromInfos(std::vector<PlayersInfos> infos)
   {
      std::vector<std::shared_ptr<Player>> players;
      for (PlayersInfos info : infos)
      {
         players.push_back(std::make_shared<Player>(Player::PlayerFromInfos(info)));
      }
      return players;
   }
   std::vector<std::shared_ptr<Player>> Player::BatchFromInitInfos(std::vector<PlayersInitInfos> infos)
   {
      std::vector<std::shared_ptr<Player>> players;
      for (PlayersInitInfos info : infos)
      {
         players.push_back(std::make_shared<Player>(Player::PlayerFromInitInfos(info)));
      }
      return players;
   }

   std::string Player::getName()
   {
      return name;
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

   // FIXME ? What is nbroads useful for ?    delete  on va   changer avec BorrowedRoads     shared  pointer
   std::vector<std::shared_ptr<mapState::Road>> Player::getBorrowedRoads()
   {
      return borrowedRoads;
   }

   PlayerColor Player::getColor()
   {
      return color;
   }

   std::shared_ptr<cardsState::PlayerCards> Player::getHand()
   {
      return hand;
   }

   // FIXME ? Might not be usefull delete
   void Player::setHand(std::shared_ptr<cardsState::PlayerCards> hand)
   {
      this->hand = hand;
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
   // TODO:  add method to calculate final points
   int Player::calculateWagonPoints()
   {
      int totalPoints = 0;
      for (const auto& road : borrowedRoads)
      {
         int length = road->getLength();

         auto it = Player::PointsByRoadLength.find(length);

         if (it != Player::PointsByRoadLength.end())
         {
            totalPoints += it->second;
         }
         else
         {
            DEBUG_PRINT("Attention : longueur de route non prévue -> " << length);

         }
      }
      return totalPoints;
   }

   int Player::calculatePoints()
   {
      return   calculateDestinationPoints() + calculateWagonPoints();
   }

   bool Player::isDestinationReached(std::shared_ptr<mapState::MapState> map,
                                  std::shared_ptr<cardsState::DestinationCard> destinationCard)
   {
      if (!map || !destinationCard) return false;

      auto stationA = destinationCard->getstationA();
      auto stationB = destinationCard->getstationB();

      if (!stationA || !stationB) return false;

      return map->isDestinationReached(
          std::make_shared<Player>(*this),
          stationA,
          stationB
      );
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
               else if (static_cast<int>(card->getColor()) == static_cast<int>(requiredColor))
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

   std::vector<std::shared_ptr<mapState::Road>> Player::getClaimableRoads(std::shared_ptr<mapState::MapState> map)
   {
      return playersState::PlayersState::getClaimableRoads(map, std::make_shared<Player>(*this));
   }

   void Player::display(int indent)
   {
      std::string indentation(indent, '\t');
      std::cout << indentation <<"##### PLAYER #####" << std::endl;
      std::cout << indentation <<"Name: " << name << std::endl;
      std::cout << indentation <<"Color: " << ColorsNames[this->color]  << std::endl;
      std::cout << indentation <<"Score: " << score << std::endl;
      std::cout << indentation <<"NbWagons: " << nbWagons << std::endl;
      std::cout << indentation <<"NbStations: " << nbStations << std::endl;
      std::cout << indentation << "===== BORROWED ROADS ======" << std::endl;
      if (!this->borrowedRoads.empty())
      {
         for (std::shared_ptr<mapState::Road> road : this->borrowedRoads)
         {
            road->display(indent+1);
         }
      }
      else
      {
         std::cout << indentation << "\t"<< "no roads borrowed yet" << std::endl;
      }
      std::cout << indentation << "#####################" << std::endl;
      if (hand)
      {
        hand->display(indent+1);
      }
      else
      {
         std::cout << indentation << "No hand available." << std::endl;
      }
      std::cout << indentation << "#####################" << std::endl;
   }

   // TODO:  FIXME to check logic
   template <class CardType>
   void Player::drawCard(std::shared_ptr<cardsState::CardsState> cardsState, std::shared_ptr<CardType> card, int number)
   {
      hand->takeCard<CardType>(cardsState, card, number);
   }
}
