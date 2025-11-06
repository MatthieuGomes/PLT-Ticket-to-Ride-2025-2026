#include"FaceUpCards.h"

#include <iostream>

namespace cardsState {
    // class FaceUpCards -
    template <class CardType>
    FaceUpCards<CardType>::FaceUpCards(std::vector<CardType> *cards) : Deck<CardType>(cards)
    {
    }
    template <class CardType>
    void FaceUpCards<CardType>::display()
    {
        std::cout << " This is a face up cards deck with " << this->cards.size() << " cards." << std::endl;
    }
}
