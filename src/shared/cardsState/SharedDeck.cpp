#include "SharedDeck.h"
namespace cardsState {
    // class SharedDeck -
    template <  class CardType>
    SharedDeck<CardType>::SharedDeck () {
        this->trash = std::make_shared<Trash<CardType>>(nullptr);
        this->faceUpCards = std::make_shared<FaceUpCards<CardType>>(nullptr);
        this->faceDownCards = std::make_shared<FaceDownCards<CardType>>(nullptr);
    }
}