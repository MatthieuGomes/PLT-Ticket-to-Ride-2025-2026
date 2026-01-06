#include"FaceUpCards.h"
#include "DestinationCard.h"
#include "WagonCard.h"

#include <iostream>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState {
    // class FaceUpCards -
    template <class CardType>
    FaceUpCards<CardType>::FaceUpCards() : Deck<CardType>(){
    }
    template <class CardType>
    FaceUpCards<CardType>::FaceUpCards(std::vector<CardType>cards) : Deck<CardType>(cards){
    }

    template<>
    FaceUpCards<WagonCard>::FaceUpCards(std::vector<std::shared_ptr<WagonCard>> cards) : Deck<WagonCard>(cards){
    }   
    template<>
    FaceUpCards<DestinationCard>::FaceUpCards(std::vector<std::shared_ptr<DestinationCard>> cards) : Deck<DestinationCard>(cards){
    }

    template<class CardType>
    void FaceUpCards<CardType>::display(int indent) {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "===== FACE UP CARDS =====\n";
        Deck<CardType>::_display(indent);
        std::cout << indentation << "=========================\n";
    }
    template <class CardType>
    FaceUpCards<CardType>::FaceUpCards(std::vector<std::shared_ptr<CardType>> cards) : Deck<CardType>(cards){
        this->className = "FaceUpCards";
    }
    template class FaceUpCards<DestinationCard>;
    template class FaceUpCards<WagonCard>;
}
