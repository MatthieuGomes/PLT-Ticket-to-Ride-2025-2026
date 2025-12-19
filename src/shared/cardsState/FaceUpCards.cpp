#include"FaceUpCards.h"
#include "DestinationCard.h"
#include "WagonCard.h"

#include <iostream>

namespace cardsState {
    // class FaceUpCards -
    template <class CardType>
    FaceUpCards<CardType>::FaceUpCards(std::vector<CardType>cards) : Deck<CardType>(cards){
        this->className = "FaceUpCards";
    }

    template<>
    FaceUpCards<WagonCard>::FaceUpCards(std::vector<std::shared_ptr<WagonCard>> cards) : Deck<WagonCard>(cards){
        this->className = "FaceUpCards";
    }   
    template<>
    FaceUpCards<DestinationCard>::FaceUpCards(std::vector<std::shared_ptr<DestinationCard>> cards) : Deck<DestinationCard>(cards){
        this->className = "FaceUpCards";
    }   
    
    template class FaceUpCards<DestinationCard>;
    template class FaceUpCards<WagonCard>;
}
