#include <iostream>

#include "FaceDownCards.h"
namespace cardsState {
    // class FaceDownCards -
    template <class CardType>
    FaceDownCards<CardType>::FaceDownCards(std::vector<CardType> *cards) : Deck<CardType>(cards)
    {
    }
    template <class CardType>
    void FaceDownCards<CardType>::display()
    {
        std::cout << " This is a face down cards deck with " << this->cards.size() << " cards." << std::endl;
    }
}
