#include "CardState.h"
#include <iostream>

void CardState::displayAll() const {
    visibleDeck.displayDeck();
    wagonDeck.displayDeck();
    destinationDeck.displayDeck();
    discardedDeck.displayDeck();
}

void CardState::onPick(std::shared_ptr<Card> card, DeckPlayer& player) {
    player.addCard(card);
    std::cout << "Picked card #" << card->getId() << std::endl;
}

void CardState::onDiscard(std::shared_ptr<Card> card) {
    discardedDeck.addCard(card);
    std::cout << "Card discarded: #" << card->getId() << std::endl;
}
