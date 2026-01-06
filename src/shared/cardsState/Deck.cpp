#include "Deck.h"

#include <iostream>

#include "Card.h"
#include "DestinationCard.h"
#include "WagonCard.h"
#include <boost/type_index.hpp>
#include "mapState/Station.h"


using DestinationCardInfos = std::tuple<std::shared_ptr<mapState::Station>,std::shared_ptr<mapState::Station>, int>;
using WagonCardInfos = cardsState::ColorCard;

namespace cardsState
{
    // class Deck -

    template <>
    Deck<WagonCard>::Deck(std::vector<ColorCard> wagonCardsInfos)
    {
        this->className = "Deck";
        for (WagonCardInfos wagonInfo : wagonCardsInfos)
        {
            this->cards.push_back(std::make_shared<WagonCard>(wagonInfo));
        }
    }
    template <>
    Deck<DestinationCard>::Deck(std::vector<DestinationCardInfos> destinationCardsInfos)
    {
        this->className = "Deck";
        for (DestinationCardInfos destinationInfo : destinationCardsInfos)
        {
            this->cards.push_back(std::make_shared<DestinationCard>(std::get<0>(destinationInfo), std::get<1>(destinationInfo), std::get<2>(destinationInfo)));
        }
    }
    template <class CardType>
    Deck<CardType>::Deck(std::vector<CardType> cards)
    {
        this->className = "Deck";
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
        this->className = "Deck";
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
    // TODO : functions to select only cards of a given color
    template  <class CardType>
    std::vector<std::shared_ptr<CardType>> Deck<CardType>::getCards() {
        return cards;
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
    Deck<CardType>::~Deck()
    {
        cards.clear();
    }
    template<class CardType>
    void Deck<CardType>::display() {
        std::cout<<" This is a "<< this->className <<" of "<< this->getCardTypeName() <<" with " << this->countCards()<<" cards."<< std::endl;
        for (const std::shared_ptr<CardType> &card: this->cards) {
            if (card) {

                static_cast<std::shared_ptr<CardType>>(card)->display();
            }
        }

    }
    template<class CardType>
    int Deck<CardType>::countCards() {
        int nb=0;
        for (std::shared_ptr<CardType> card: this->cards) {
            if (card) {
                nb++;
            }
        }
        return nb;
    }
    
    template <class CardType>
    std::string Deck<CardType>::getCardTypeName() {
        std::string typeName_full = boost::typeindex::type_id<CardType>().pretty_name();
        std::string typeName_short = typeName_full.substr(typeName_full.find_last_of("::") + 1);
        return typeName_short;
    }

    template <class CardType>
    void Deck<CardType>::moveCardTo (std::shared_ptr<Deck<CardType>> originDeck, std::shared_ptr<Deck<CardType>> destinationDeck, int cardPosition) {
        {
            if (!originDeck || !destinationDeck) return;

            std::shared_ptr<CardType> card = originDeck->removeCard(cardPosition);
            if (card) {
                destinationDeck->addCard(card);
            }
        }

    }
    template <class CardType>
    void Deck<CardType>::putCardBack (CardType destinationcard) {
        auto ptr = std::make_shared<CardType>(std::move(destinationcard));
        if (cards.size() == 1 && cards[0] == nullptr) {
            cards[0] = ptr;
        } else {
            cards.insert(cards.begin(), ptr);
        }

    }
    template <class CardType>
    std::shared_ptr<CardType> Deck<CardType>::takeLastCard() {
        if (cards.size() > 0) {
            return removeCard(static_cast<int>(cards.size()) - 1);
        }
        return nullptr;
    }



    template class Deck<DestinationCard>;
    template class Deck<WagonCard>;
}