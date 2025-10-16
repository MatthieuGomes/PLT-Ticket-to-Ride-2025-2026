#include "DeckInvisible.h"
#include <iostream>
#include <algorithm>

void DeckInvisible::addCard(std::shared_ptr<Card> card) {
    cards.push_back(card);
}

std::shared_ptr<Card> DeckInvisible::removeCard() {
    if (cards.empty()) return nullptr;
    auto c = cards.back();
    cards.pop_back();
    return c;
}

void DeckInvisible::shuffle(uint16_t a1, uint16_t c1, uint16_t x01,
                            uint16_t a2, uint16_t c2, uint16_t x02) {
    LcgGenerator g1(a1, c1, x01, cards.size());
    LcgGenerator g2(a2, c2, x02, cards.size());
    for (int i = 0; i < 100; ++i)
        std::swap(cards[g1.next()], cards[g2.next()]);
}

void DeckInvisible::displayDeck() const {
    std::cout << "DeckInvisible: " << cards.size() << " cards." << std::endl;
}

bool DeckInvisible::isEmpty() const { return cards.empty(); }
