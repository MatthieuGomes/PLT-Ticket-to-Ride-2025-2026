#include "PlayerCards.h"
#include <iostream>

namespace cardsState {
  PlayerCards::PlayerCards(std::vector<DestinationCard>* destinationCards, std::vector<WagonCard>* wagonCards) {
      this->destinationCards = std::make_shared<Deck<DestinationCard>>(destinationCards);
      this->wagonCards = std::make_shared<Deck<WagonCard>>(wagonCards);
  }
  void PlayerCards::display() {
        std::cout << " Player's Destination Cards: " << std::endl;
        for (const auto& card : this->destinationCards->cards) {
            if (card) {
                card->display();
            }
        }
        std::cout << " Player's Wagon Cards: " << std::endl;
        for (const auto& card : this->wagonCards->cards) {
            if (card) {
                card->display();
            }
        }
  }
}
