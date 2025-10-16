#include "Trash.h"
#include <iostream>

void Trash::addCard(std::shared_ptr<Card> card) {
    discarded.push_back(card);
}

std::shared_ptr<Card> Trash::removeCard() {
    if (discarded.empty()) return nullptr;
    auto c = discarded.back();
    discarded.pop_back();
    return c;
}

void Trash::shuffle(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t) {}

void Trash::displayDeck() const {
    std::cout << "Trash contains " << discarded.size() << " cards." << std::endl;
}
