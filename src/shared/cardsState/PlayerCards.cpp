#include "PlayerCards.h"
#include <iostream>
#include "SharedDeck.h"
namespace cardsState {
  PlayerCards::PlayerCards(std::vector<DestinationCard>* destinationCards, std::vector<WagonCard>* wagonCards) {
      this->destinationCards = std::make_shared<Deck<DestinationCard>>(destinationCards);
      this->wagonCards = std::make_shared<Deck<WagonCard>>(wagonCards);
  }
  void PlayerCards::display() {
        std::cout << " Player's Destination Cards: " << std::endl;
        destinationCards->display();
        std::cout << " Player's Wagon Cards: " << std::endl;
         wagonCards->display();
  }

    template<class CardType> void PlayerCards::takeCard(int number) {
    cardsState::SharedDeck<CardType>::drawCard(number, this);

  }
    template<class CardType> void PlayerCards::takeCard(CardType* card ) {
    cardsState::SharedDeck<CardType>::drawCard(card, this);

  }
}


