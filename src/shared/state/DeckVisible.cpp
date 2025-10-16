#include "DeckVisible.h"
#include <iostream>

void DeckVisible::addCard(std::shared_ptr<Card> card) {
    if (visibleCards.size() < 5)
        visibleCards.push_back(card);
}

std::shared_ptr<Card> DeckVisible::removeCard() {
    if (visibleCards.empty()) return nullptr;
    auto c = visibleCards.back();
    visibleCards.pop_back();
    return c;
}

void DeckVisible::shuffle(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t) {}

void DeckVisible::displayDeck() const {
    std::cout << "Visible Deck:" << std::endl;
    for (auto& c : visibleCards) c->display();
}
