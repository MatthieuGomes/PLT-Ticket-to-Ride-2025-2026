#include "Trash.h"
#include "DestinationCard.h"
#include "WagonCard.h"

#include <iostream>
namespace cardsState {
    // class Trash -
    template <class CardType>
    Trash<CardType>::Trash(std::vector<CardType> cards) : Deck<CardType>(cards){
        this->className = "Trash";
    }

    template<>
    Trash<WagonCard>::Trash(std::vector<std::shared_ptr<WagonCard>> cards) : Deck<WagonCard>(cards){
        this->className = "Trash";
    }
    template<>
    Trash<DestinationCard>::Trash(std::vector<std::shared_ptr<DestinationCard>> cards) : Deck<DestinationCard>(cards){
        this->className = "Trash";
    }

    template class Trash<DestinationCard>;
    template class Trash<WagonCard>;
}