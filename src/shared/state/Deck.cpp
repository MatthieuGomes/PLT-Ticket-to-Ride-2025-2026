//
// Created by chahrazedbaatout on 10/10/25.
//
#include "Deck.h"

#include <random>
#include "Card.h"
void DeckVisible::addCard(Card* card) { cards.push_back(card); }
void DeckVisible::removeCard(Card* card) {
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
}
void DeckVisible::shuffle() { std::shuffle(cards.begin(), cards.end(), std::mt19937(std::random_device()())); }
void DeckVisible::displayDeck() const {
    std::cout << "\n[DeckVisible]:\n";
    for (auto c : cards) c->display();
}

void DeckInvisible::addCard(Card* card) { cards.push_back(card); }
void DeckInvisible::removeCard(Card* card) {
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
}
void DeckInvisible::shuffle() { std::shuffle(cards.begin(), cards.end(), std::mt19937(std::random_device()())); }
void DeckInvisible::displayDeck() const {
    std::cout << "\n[DeckInvisible]:\n";
    for (auto c : cards) c->display();
}

void DeckPlayer::addCard(Card* card) { cards.push_back(card); }
void DeckPlayer::removeCard(Card* card) {
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
}
void DeckPlayer::shuffle() { std::shuffle(cards.begin(), cards.end(), std::mt19937(std::random_device()())); }
void DeckPlayer::displayDeck() const {
    std::cout << "\n[DeckPlayer]:\n";
    for (auto c : cards) c->display();
}

void Trash::addCard(Card* card) { cards.push_back(card); }
void Trash::removeCard(Card* card) {
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
}
void Trash::shuffle() {}
void Trash::displayDeck() const {
    std::cout << "\n[Trash Deck]:\n";
    for (auto c : cards) c->display();
}
