#include "Deck.h"

#include <iostream>

#include "Card.h"
#include "DestinationCard.h"
#include "WagonCard.h"
#include <boost/type_index.hpp>
#include "mapState/Station.h"
#include <random>
#include <algorithm> // std::shuffle

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState
{
    using DestinationCardInfos = std::tuple<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>, int, bool>;
    using WagonCardInfos = cardsState::ColorCard;
    // class Deck -

    template <class CardType>
    Deck<CardType>::Deck()
    {
    }
    template <>
    Deck<WagonCard>::Deck(std::vector<ColorCard> wagonCardsInfos)
    {
        for (WagonCardInfos wagonInfo : wagonCardsInfos)
        {
            this->cards.push_back(std::make_shared<WagonCard>(wagonInfo));
        }
    }
    template <>
    Deck<DestinationCard>::Deck(std::vector<DestinationCardInfos> destinationCardsInfos)
    {
        for (DestinationCardInfos destinationInfo : destinationCardsInfos)
        {
            this->cards.push_back(std::make_shared<DestinationCard>(std::get<0>(destinationInfo), std::get<1>(destinationInfo), std::get<2>(destinationInfo), std::get<3>(destinationInfo)));
        }
    }
    template <class CardType>
    Deck<CardType>::Deck(std::vector<CardType> cards)
    {
        if (!cards.empty())
        {
            for (CardType card : cards)
            {
                this->cards.push_back(std::make_shared<CardType>(card));
            }
        }
        else
        {
            this->cards = {};
        }
    }

    template <class CardType>
    Deck<CardType>::Deck(std::vector<std::shared_ptr<CardType>> cards)
    {
        if (!cards.empty())
        {
            for (std::shared_ptr<CardType> card : cards)
            {
                this->cards.push_back(std::move(card));
            }
        }
        else
        {
            this->cards = {};
        }
    }

    //TODO : add method to select cards of only one color + count them

    template <class CardType>
    std::vector<std::shared_ptr<CardType>> Deck<CardType>::getCards()
    {
        return cards;
    }
    template <class CardType>
    int Deck<CardType>::countCards()
    {
        int nb = 0;
        for (std::shared_ptr<CardType> card : this->cards)
        {
            if (card)
            {
                nb++;
            }
        }
        return nb;
    }

    template <class CardType>
    void Deck<CardType>::addCard(std::shared_ptr<CardType> card)
    {

        this->cards.push_back(std::move(card));
    }
    template <class CardType>
    std::shared_ptr<CardType> Deck<CardType>::removeCard(int position)
    {
        if (position < 0 || position >= static_cast<int>(cards.size()))
        {
            return nullptr; // or throw an exception
        }
        std::shared_ptr<CardType> removedCard = std::move(cards[position]);
        cards.erase(cards.begin() + position);
        return removedCard;
    }
    template <class CardType>
    void Deck<CardType>::shuffle()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(this->cards.begin(), this->cards.end(), g);
    }
    template <class CardType>
    std::vector<std::shared_ptr<CardType>> Deck<CardType>::shuffle(std::vector<std::shared_ptr<CardType>> cards)
    {
        std::vector<std::shared_ptr<CardType>> cardsCopy = std::vector<std::shared_ptr<CardType>>(cards);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cardsCopy.begin(), cardsCopy.end(), g);
        return cardsCopy;
    }

    template <class CardType>
    void Deck<CardType>::moveCardTo(std::shared_ptr<Deck<CardType>> originDeck, std::shared_ptr<Deck<CardType>> destinationDeck, int cardPosition)
    {
        {
            if (!originDeck || !destinationDeck)
                return;

            std::shared_ptr<CardType> card = originDeck->removeCard(cardPosition);
            if (card)
            {
                destinationDeck->addCard(card);
            }
        }
    }
    template <class CardType>
    void Deck<CardType>::putCardBack(CardType destinationcard)
    {
        auto ptr = std::make_shared<CardType>(std::move(destinationcard));
        if (this->cards.empty())
        {
            this->cards.push_back(ptr);
        }
        else if (this->cards.size() == 1 && this->cards[0] == nullptr)
        {
            this->cards[0] = ptr;
        }
        else
        {
            this->cards.insert(this->cards.begin(), ptr);
        }
    }
    template <class CardType>
    std::shared_ptr<CardType> Deck<CardType>::takeLastCard()
    {
        if (cards.size() > 0)
        {
            return removeCard(static_cast<int>(cards.size()) - 1);
        }
        return nullptr;
    }

    template <class CardType>
    void Deck<CardType>::display(int indent)
    {
        std::cout << "===== DECK =====\n";
        this->_display(indent);
        std::cout << "================\n";
    }
    template <class CardType>
    void Deck<CardType>::_display(int indent)
    {

        for (const std::shared_ptr<CardType> &card : this->cards)
        {
            if (card)
            {
                static_cast<std::shared_ptr<CardType>>(card)->display(indent + 1);
            }
        }
    }

    template <class CardType>
    Deck<CardType>::~Deck()
    {
        cards.clear();
    }
    template class Deck<DestinationCard>;
    template class Deck<WagonCard>;
}