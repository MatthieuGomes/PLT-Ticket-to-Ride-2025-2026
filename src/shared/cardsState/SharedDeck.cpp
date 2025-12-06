#include "SharedDeck.h"

#include <iostream>
#include <ostream>

#include "DestinationCard.h"
#include "WagonCard.h"
#include <algorithm> // std::shuffle
#include <random>
#include "PlayerCards.h"
#include <type_traits>
#include <boost/smart_ptr/make_shared_object.hpp>

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
        std::vector< std::shared_ptr<CardType> > temp;

        while (trash->countCards() > 0) {
            temp.push_back(trash->removeCard(trash->countCards() - 1));
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(temp.begin(), temp.end(), g);

        for (auto& c : temp) {
            faceDownCards->addCard(c);
        }

        std::cout << "Main deck refilled from trash." << std::endl;
    }
    template <  class CardType>
    void cardsState::SharedDeck<CardType>::trashCard(std::shared_ptr<CardType> card) {
        int position = std::distance(this->faceUpCards->cards.begin(),std::find(this->faceUpCards->cards.begin(), this->faceUpCards->cards.end(), card));
        std::shared_ptr<CardType> removedCard = this->faceUpCards->removeCard(position);
        this->trash->addCard(removedCard);
    }
    // a function that take the card wagon  passed in parameter  from the visible deck and add it in the cardsplayer

    template <>
    void cardsState::SharedDeck<WagonCard>::drawCard(std::shared_ptr<WagonCard> card, PlayerCards* player)
    {
        int pos = std::distance(this->faceUpCards->cards.begin(), std::find(this->faceUpCards->cards.begin(),
                            this->faceUpCards->cards.end(),
                            card));
        std::shared_ptr<WagonCard> removedCard = this->faceUpCards->removeCard(pos);

        player->wagonCards->addCard(removedCard);
        std::cout << "WagonCard piochée depuis les cartes visibles !" << std::endl;
    }
    template <class CardType>
void cardsState::SharedDeck<CardType>::drawCard(int number, PlayerCards* player)
    {
        for (int i = 0; i < number; ++i) {
            if (this->faceDownCards->countCards() == 0) {
                this->refillMainDeck();
                if (this->faceDownCards->countCards() == 0) break;
            }
            std::shared_ptr<CardType> card = this->faceDownCards->takeLastCard();
            if constexpr (std::is_same_v<CardType, WagonCard>) {
                player->wagonCards->addCard(card);
            }
            else if constexpr (std::is_same_v<CardType, DestinationCard>) {
                player->destinationCards->addCard(card);
            }
        }
    }

    template class SharedDeck<DestinationCard>;
    template class SharedDeck<WagonCard>;
}
