#include "CardState.h"
#include <iostream>
#include <memory>

void cardState::CardState::displayAll() {
    visibleDeck.displayDeck();
    wagonDeck.displayDeck();
    destinationDeck.displayDeck();
    discardedDeck.displayDeck();
}

void cardState::CardState::onPick(std::shared_ptr<Card> card, DeckPlayer& player) {
    player.addCard(card.get());
    std::cout << "Picked card #" << card->getId() << std::endl;
}

void cardState::CardState::onDiscard(std::shared_ptr<Card> card) {
    discardedDeck.addCard(card.get());
    std::cout << "Card discarded: #" << card->getId() << std::endl;
}
