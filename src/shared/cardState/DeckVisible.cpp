#include "DeckVisible.h"
#include <iostream>

void state::DeckVisible::addCard(std::shared_ptr<Card> card) {
    if (visibleCards.size() < 5)
        visibleCards.push_back(card);
}

std::shared_ptr<state::Card> state::DeckVisible::removeCard() {
    if (visibleCards.empty()) return nullptr;
    auto c = visibleCards.back();
    visibleCards.pop_back();
    return c;
}

void state::DeckVisible::shuffle(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t) {}

void state::DeckVisible::displayDeck()  {
    std::cout << "Visible Deck:" << std::endl;
    for (auto& c : visibleCards) c->display();
}
