#include "DeckPlayer.h"
#include <iostream>

void DeckPlayer::addCard(std::shared_ptr<Card> card) {
    playerCards.push_back(card);
}

std::shared_ptr<Card> DeckPlayer::removeCard() {
    if (playerCards.empty()) return nullptr;
    auto c = playerCards.back();
    playerCards.pop_back();
    return c;
}

void DeckPlayer::shuffle(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t) {}

void DeckPlayer::displayDeck() const {
    std::cout << "Player Hand:" << std::endl;
    for (auto& c : playerCards) c->display();
}
