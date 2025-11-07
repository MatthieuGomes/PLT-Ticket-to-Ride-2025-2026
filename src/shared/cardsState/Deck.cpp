#include "Deck.h"

#include <iostream>

#include "Card.h"
#include "DestinationCard.h"
#include "WagonCard.h"
#include <boost/type_index.hpp>


namespace cardsState
{
    // class Deck -
    template <class CardType>
    Deck<CardType>::Deck(std::vector<CardType> *cards)
    {
        if (cards)
        {
            for (const auto &card : *cards)
            {
                this->cards.push_back(std::make_shared<CardType>(card));
            }
        }
        else
        {
            this->cards = {nullptr};
        }
    }

    template <class CardType>
    void Deck<CardType>::addCard(CardType *card)
    {
        this->cards.push_back(std::shared_ptr<CardType>(card));
    }
    template <class CardType>
    CardType *Deck<CardType>::removeCard(int position)
    {
        if (position < 0 || position >= static_cast<int>(cards.size()))
        {
            return nullptr; // or throw an exception
        }
        CardType * removedCard = std::move(cards[position]).get();
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
        std::cout<<" This is a "<< boost::typeindex::type_id<decltype(this)>().pretty_name() <<" cards deck of "<<boost::typeindex::type_id<CardType>()<<" with " << this->countCards()<<" cards."<< std::endl;
        for (const std::shared_ptr<CardType> &card: this->cards) {
            if (card) {

                static_cast<std::shared_ptr<CardType>>(card)->display();
            }
        }

    }
    template<class CardType>
   int Deck<CardType>::countCards() {
        int nb=0;
        for (const std::shared_ptr<CardType> &card: this->cards) {
            if (card) {
                nb++;

            }

        }
        return nb;
    }
    
    template class Deck<DestinationCard>;
    template class Deck<WagonCard>;
}