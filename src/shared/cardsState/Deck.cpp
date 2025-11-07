#include "Deck.h"
#include "Card.h"
#include "DestinationCard.h"
#include "WagonCard.h"

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

    }
}