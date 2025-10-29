#include "CardsState.h"

#include <iostream>

namespace cardsState {

  /// class CardsState - 
  CardsState::CardsState () {
  }
  void CardsState::printCardsState () {
      std::cout << " Current Cards State " << std::endl;

      // visibleDeck.displayDeck();
      // wagonDeck.displayDeck();
      // destinationDeck.displayDeck();
      // discardedDeck.displayDeck();

      // std::cout << "Number of player hands: " << playerHands.size() << std::endl;
  }
}