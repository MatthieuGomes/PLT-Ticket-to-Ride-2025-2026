#include <iostream>

#include "FaceDownCards.h"
#include "DestinationCard.h"
#include "WagonCard.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState {
    // class FaceDownCards -
    template <class CardType>
    FaceDownCards<CardType>::FaceDownCards() : Deck<CardType>()
    {
    }
    template <class CardType>
    FaceDownCards<CardType>::FaceDownCards(std::vector<CardType> cards) : Deck<CardType>(cards)
    {
    }

    template<>
    FaceDownCards<WagonCard>::FaceDownCards(std::vector<std::shared_ptr<WagonCard>> cards) : Deck<WagonCard>(cards)
    {
    }

    template<>
    FaceDownCards<DestinationCard>::FaceDownCards(std::vector<std::shared_ptr<DestinationCard>> cards) : Deck<DestinationCard>(cards)
    {
    }

    template<>
    FaceDownCards<DestinationCard> FaceDownCards<DestinationCard>::Europe(std::vector<std::shared_ptr<mapState::Station>> stations){
        std::vector<std::shared_ptr<DestinationCard>> destinationCards = DestinationCard::Europe(stations);
        return FaceDownCards<DestinationCard>(destinationCards);
    }

    template<class CardType>
    void FaceDownCards<CardType>::display(int indent) {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "===== FACE DOWN CARDS =====\n";
        Deck<CardType>::_display(indent);
        std::cout << indentation << "===========================\n";
    }
    template class FaceDownCards<DestinationCard>;
    template class FaceDownCards<WagonCard>;
}
