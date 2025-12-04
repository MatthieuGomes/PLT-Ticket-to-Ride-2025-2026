#include "SharedDeck.h"

#include <iostream>
#include <ostream>

#include "DestinationCard.h"
#include "WagonCard.h"
#include <algorithm> // std::shuffle
#include <random>

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

    template <  class CardType>
    void SharedDeck <CardType>::refillMainDeck() {
        if (faceDownCards->countCards() > 0) {
            std::cout << "Face-down deck is not empty. No refill needed." << std::endl;
            return;
        }

        if (trash->countCards() == 0) {
            std::cout << "Trash is empty. Cannot refill main deck." << std::endl;
            return;
        }


        using CardPtr = decltype(this->trash->removeCard(0));
        std::vector<CardPtr> temp;

        while (trash->countCards() > 0) {
            temp.push_back(trash->removeCard(trash->countCards() - 1));
        }

        // Shuffle
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(temp.begin(), temp.end(), g);

        // Add them to face-down deck
        for (auto& c : temp) {
            faceDownCards->addCard(c);
        }

        std::cout << "Main deck refilled from trash." << std::endl;
    }


    template <  class CardType>
    void SharedDeck <CardType>::trashCard (std::shared_ptr<CardType> card) {
        trash->addCard(card.get());
        std::cout << "Card trashed." << std::endl;
    }



    template class SharedDeck<DestinationCard>;
    template class SharedDeck<WagonCard>;
}