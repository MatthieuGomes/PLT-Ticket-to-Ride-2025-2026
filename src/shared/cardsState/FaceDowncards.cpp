#include <iostream>

#include "FaceDownCards.h"
#include "DestinationCard.h"
#include "WagonCard.h"

namespace cardsState {
    // class FaceDownCards -
    template <class CardType>
    FaceDownCards<CardType>::FaceDownCards(std::vector<CardType> *cards) : Deck<CardType>(cards)
    {
    }
    template <class CardType>
    void FaceDownCards<CardType>::display()
    {

        Deck<CardType>::display();
    }

    template class FaceDownCards<DestinationCard>;
    template class FaceDownCards<WagonCard>;
}
