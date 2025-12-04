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

    template <  class CardType>
    void SharedDeck<CardType>::turnCardUp() {
        if (faceDownCards->countCards() == 0) {
            std::cout << "No more face-down cards." << std::endl;
            return;
        }
        int lastIndex = faceDownCards->countCards() - 1;
        auto card = faceDownCards->removeCard(lastIndex);

        faceUpCards->addCard(card);

        std::cout << "Card moved from face-down to face-up." << std::endl;
    }
    /*
    void trashfromcdarduptocarddown
    void refillMaindeck
    void
    //turncardup
    //donner au player
    //remove from my shareddeck
    //ajouter au deck de player
    */
    template class SharedDeck<DestinationCard>;
    template class SharedDeck<WagonCard>;
}