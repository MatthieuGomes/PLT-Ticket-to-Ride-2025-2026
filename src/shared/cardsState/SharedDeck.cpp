#include "SharedDeck.h"

#include <iostream>

#include "DestinationCard.h"
#include "WagonCard.h"
#include "OutOfGame.h"
#include <algorithm> // std::shuffle
#include <random>
#include "PlayerCards.h"
#include <boost/smart_ptr/make_shared_object.hpp>
#include <type_traits>
#include <algorithm> // std::find, std::find_if

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState
{
    // class SharedDeck -
    template <class CardType>
    SharedDeck<CardType>::SharedDeck()
    {
    }

    template <class CardType>
    SharedDeck<CardType>::SharedDeck(std::vector<std::shared_ptr<CardType>> trash, std::vector<std::shared_ptr<CardType>> faceUpCards, std::vector<std::shared_ptr<CardType>> faceDownCards)
    {
        this->trash = std::make_shared<Trash<CardType>>(trash);
        this->faceUpCards = std::make_shared<FaceUpCards<CardType>>(faceUpCards);
        this->faceDownCards = std::make_shared<FaceDownCards<CardType>>(faceDownCards);
    }

    template <class CardType>
    SharedDeck<CardType>::SharedDeck(std::vector<std::shared_ptr<CardType>> faceDownCards)
    {
        this->faceDownCards = std::make_shared<FaceDownCards<CardType>>(faceDownCards);
    }

    template <>
    SharedDeck<WagonCard> SharedDeck<WagonCard>::Init()
    {
        std::vector<std::shared_ptr<WagonCard>> faceDownCards = WagonCard::StartCards();
        return SharedDeck<WagonCard>(faceDownCards);
    }

    template <>
    SharedDeck<DestinationCard> SharedDeck<DestinationCard>::Europe(std::vector<std::shared_ptr<mapState::Station>> stations)
    {
        SharedDeck<DestinationCard> sharedDeck;
        std::vector<std::shared_ptr<DestinationCard>> destinationCards = DestinationCard::Europe(stations);
        std::vector<std::shared_ptr<DestinationCard>> faceDownCards = destinationCards;
        sharedDeck.faceDownCards = std::make_shared<FaceDownCards<DestinationCard>>(faceDownCards);
        return sharedDeck;
    }
    template <>
    void SharedDeck<DestinationCard>::Setup(std::shared_ptr<OutOfGame<DestinationCard>> outOfGameCards)
    {
        std::shared_ptr<FaceDownCards<DestinationCard>> faceDownCards = this->faceDownCards;        int position = 0;
        for (std::shared_ptr<DestinationCard> card : faceDownCards->getCards())
        {
            if (card->isLong)
            {
                outOfGameCards->addCard(faceDownCards->removeCard(position));
                continue;
            }
            position++;
        }
    }

    template <>
    void SharedDeck<WagonCard>::Setup()
    {
        std::shared_ptr<FaceDownCards<WagonCard>> faceDownCards = this->faceDownCards;
        for (int i = 0; i < WagonCard::faceUpCardsCount; i++)
        {   
            this->turnCardUp();
        }
    }

    template <class CardType>
    void SharedDeck<CardType>::display(int indent)
    {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "###### SHARED DECK ######\n";
        if(this->trash){
            this->trash->display(indent+1);
        }
        if(this->faceUpCards){
            this->faceUpCards->display(indent+1);
        }
        if(this->faceDownCards){
            this->faceDownCards->display(indent+1);
        }
        std::cout << "########################\n";
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
        if(this->faceUpCards == nullptr){
            this->faceUpCards = std::make_shared<FaceUpCards<CardType>>();
        }
        this->faceUpCards->addCard(card);

    }

    template <class CardType>
    void SharedDeck<CardType>::refillMainDeck()
    {
        if (faceDownCards->countCards() > 0)
        {
            return;
        }
        if (trash->countCards() == 0)
        {
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