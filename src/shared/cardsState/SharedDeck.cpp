#include "SharedDeck.h"
#include "DestinationCard.h"
#include "WagonCard.h"

namespace cardsState {
    // class SharedDeck -
    template <  class CardType>
    SharedDeck<CardType>::SharedDeck () {
        this->trash = std::make_shared<Trash<CardType>>(nullptr);
        this->faceUpCards = std::make_shared<FaceUpCards<CardType>>(nullptr);
        this->faceDownCards = std::make_shared<FaceDownCards<CardType>>(nullptr);
    }

    template class SharedDeck<DestinationCard>;
    template class SharedDeck<WagonCard>;
}