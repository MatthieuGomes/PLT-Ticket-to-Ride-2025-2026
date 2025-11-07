#include "CardsState.h"
#include "PlayerCards.h"
#include "SharedDeck.h"
#include <iostream>

namespace cardsState {
  class DestinationCard;
  class WagonCard;
  class PlayerCards;
}

namespace cardsState {

  /// class CardsState - 
  CardsState::CardsState () {
      this->gameDestinationCards = std::make_shared<SharedDeck<DestinationCard>>();
      this->gameWagonCards = std::make_shared<SharedDeck<WagonCard>>();
      this->playersCards = std::vector<std::shared_ptr<PlayerCards>>{};
  }
  void CardsState::printCardsState () {
      std::cout << " Current Cards State " << std::endl;
  }
}