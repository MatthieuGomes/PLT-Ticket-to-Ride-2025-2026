#include "Trash.h"
#include "DestinationCard.h"
#include "WagonCard.h"

#include <iostream>
namespace cardsState {
    // class Trash -
    template <class CardType>
    Trash<CardType>::Trash(std::vector<CardType> *cards) : Deck<CardType>(cards){}

    template <class CardType>
    void Trash<CardType>::display()
    {
        std::cout << " This is a trash cards deck with " << this->cards.size() << " cards." << std::endl;
    }

    template class Trash<DestinationCard>;
    template class Trash<WagonCard>;
}