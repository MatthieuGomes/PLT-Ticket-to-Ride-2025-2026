#include "SharedDeck.h"

#include <iostream>
#include <ostream>

#include "DestinationCard.h"
#include "WagonCard.h"

namespace cardsState {
    // class SharedDeck -
    template <  class CardType>
   SharedDeck<CardType>::SharedDeck (std::vector<CardType>* trash, std::vector<CardType>* faceUpCards, std::vector<CardType>* faceDownCards){
        this->trash = std::make_shared<Trash<CardType>>(trash);
        this->faceUpCards = std::make_shared<FaceUpCards<CardType>>(faceUpCards);
        this->faceDownCards = std::make_shared<FaceDownCards<CardType>>(faceDownCards);
    }



    template <  class CardType>
    void SharedDeck<CardType>::display() {
        std::cout << " Shared Deck Status: " << std::endl;
        std::cout << " Trash: " << std::endl;
        this->trash->display();
        std::cout << " Face Up Cards: " << std::endl;
        this->faceUpCards->display();
        std::cout << " Face Down Cards: " << std::endl;
        this->faceDownCards->display();
    }

    template class SharedDeck<DestinationCard>;
    template class SharedDeck<WagonCard>;
}