#include "Trash.h"
#include "DestinationCard.h"
#include "WagonCard.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

#include <iostream>
namespace cardsState {
    // class Trash -
    template <class CardType>
    Trash<CardType>::Trash() : Deck<CardType>(){
    }

    template <class CardType>
    Trash<CardType>::Trash(std::vector<CardType> cards) : Deck<CardType>(cards){
    }

    template<>
    Trash<WagonCard>::Trash(std::vector<std::shared_ptr<WagonCard>> cards) : Deck<WagonCard>(cards){
    }
    template<>
    Trash<DestinationCard>::Trash(std::vector<std::shared_ptr<DestinationCard>> cards) : Deck<DestinationCard>(cards){
    }

    template<class CardType>
    void Trash<CardType>::display(int indent) {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "===== TRASH =====\n";
        Deck<CardType>::_display(indent);
        std::cout << indentation << "=================\n";
    }

    template class Trash<DestinationCard>;
    template class Trash<WagonCard>;
}