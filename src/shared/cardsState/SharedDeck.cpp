#include "SharedDeck.h"

#include <iostream>

#include "DestinationCard.h"
#include "WagonCard.h"
#include <algorithm> // std::shuffle
#include <random>
#include "PlayerCards.h"
#include <boost/smart_ptr/make_shared_object.hpp>
#include <type_traits>
#include <algorithm> // std::find, std::find_if

namespace cardsState
{
    // class SharedDeck -
    template <class CardType>
    SharedDeck<CardType>::SharedDeck(std::vector<std::shared_ptr<CardType>> trash, std::vector<std::shared_ptr<CardType>> faceUpCards, std::vector<std::shared_ptr<CardType>> faceDownCards)
    {
        this->trash = std::make_shared<Trash<CardType>>(trash);
        this->faceUpCards = std::make_shared<FaceUpCards<CardType>>(faceUpCards);
        this->faceDownCards = std::make_shared<FaceDownCards<CardType>>(faceDownCards);
    }

    template <class CardType>
    void SharedDeck<CardType>::display()
    {
        std::cout << " Shared Deck Status: " << std::endl;
        std::cout << " Trash: " << std::endl;
        this->trash->display();
        std::cout << " Face Up Cards: " << std::endl;
        this->faceUpCards->display();
        std::cout << " Face Down Cards: " << std::endl;
        this->faceDownCards->display();
    }

    template <class CardType>
    void SharedDeck<CardType>::turnCardUp()
    {
        if (faceDownCards->countCards() == 0)
        {
            std::cout << "No more face-down cards." << std::endl;
            return;
        }
        int lastIndex = faceDownCards->countCards() - 1;
        auto card = faceDownCards->removeCard(lastIndex);

        faceUpCards->addCard(card);

        std::cout << "Card moved from face-down to face-up." << std::endl;
    }

    template <class CardType>
    void SharedDeck<CardType>::refillMainDeck()
    {
        if (faceDownCards->countCards() > 0)
        {
            std::cout << "Face-down deck is not empty. No refill needed." << std::endl;
            return;
        }
        if (trash->countCards() == 0)
        {
            std::cout << "Trash is empty. Cannot refill main deck." << std::endl;
            return;
        }
        std::vector<std::shared_ptr<CardType>> temp;

        while (trash->countCards() > 0)
        {
            temp.push_back(trash->removeCard(trash->countCards() - 1));
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(temp.begin(), temp.end(), g);

        for (auto &c : temp)
        {
            faceDownCards->addCard(c);
        }

        std::cout << "Main deck refilled from trash." << std::endl;
    }
    template <class CardType>
    void cardsState::SharedDeck<CardType>::trashCard(std::shared_ptr<CardType> card)
    {
        int position = std::distance(this->faceUpCards->cards.begin(), std::find(this->faceUpCards->cards.begin(), this->faceUpCards->cards.end(), card));
        std::shared_ptr<CardType> removedCard = std::make_shared<CardType>(*this->faceUpCards->removeCard(position));
        this->trash->addCard(std::move(removedCard));
    }

    template <class CardType>
    void cardsState::SharedDeck<CardType>::drawCard(std::shared_ptr<PlayerCards> playerCards, std::shared_ptr<CardType> card, int number)
    {
        if ((card != nullptr && number > 0) || (card == nullptr && number <= 0))
        {
            throw std::invalid_argument("Cannot specify both a card and a number.");
        }
        if (card != nullptr)
        {
            auto it = std::find_if(this->faceUpCards->cards.begin(), this->faceUpCards->cards.end(),
                                   [&](std::shared_ptr<CardType> &sp)
                                   { return sp == card; });
            if (it == this->faceUpCards->cards.end())
                return;
            int pos = static_cast<int>(std::distance(this->faceUpCards->cards.begin(), it));
            std::shared_ptr<Card> removedCard = this->faceUpCards->removeCard(pos);
            if (typeid(CardType) == typeid(DestinationCard))
            {
                playerCards->destinationCards->addCard(std::dynamic_pointer_cast<DestinationCard>(removedCard));
            }
            else if (typeid(CardType) == typeid(WagonCard))
            {
                playerCards->wagonCards->addCard(std::dynamic_pointer_cast<WagonCard>(removedCard));
            }
        }
        else if (number > 0)
        {
            for (int i = 0; i < number; ++i)
            {
                std::shared_ptr<Card> removedCard = this->faceDownCards->takeLastCard();
                if (typeid(CardType) == typeid(DestinationCard))
                {
                    std::shared_ptr<DestinationCard> typedRemovedCard = std::dynamic_pointer_cast<DestinationCard>(removedCard);
                    playerCards->destinationCards->addCard(typedRemovedCard);
                }
                else if (typeid(CardType) == typeid(WagonCard))
                {
                    std::shared_ptr<WagonCard> typedRemovedCard = std::dynamic_pointer_cast<WagonCard>(removedCard);
                    playerCards->wagonCards->addCard(typedRemovedCard);
                }
            }
        }
    }

    template class SharedDeck<DestinationCard>;
    template class SharedDeck<WagonCard>;
}