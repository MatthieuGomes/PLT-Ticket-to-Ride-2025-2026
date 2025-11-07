#include <iostream>

#include "FaceDownCards.h"
#include "DestinationCard.h"
#include "WagonCard.h"

namespace cardsState {
    // class FaceDownCards -
    template <class CardType>
    FaceDownCards<CardType>::FaceDownCards(std::vector<CardType> *cards) : Deck<CardType>(cards)
    {
        this->className = "FaceDownCards";
    }
    template class FaceDownCards<DestinationCard>;
    template class FaceDownCards<WagonCard>;
}
