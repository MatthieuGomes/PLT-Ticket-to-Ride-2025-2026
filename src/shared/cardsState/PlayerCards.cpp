#include "PlayerCards.h"
#include <iostream>
#include "SharedDeck.h"
#include "CardsState.h"
namespace cardsState
{
  PlayerCards::PlayerCards(std::vector<DestinationCard> *destinationCards, std::vector<WagonCard> *wagonCards)
  {
    this->destinationCards = std::make_shared<Deck<DestinationCard>>(destinationCards);
    this->wagonCards = std::make_shared<Deck<WagonCard>>(wagonCards);
  }
  void PlayerCards::display()
  {
    std::cout << " Player's Destination Cards: " << std::endl;
    destinationCards->display();
    std::cout << " Player's Wagon Cards: " << std::endl;
    wagonCards->display();
  }

  template <>
  void PlayerCards::_takeCard<DestinationCard>(SharedDeck<DestinationCard> *sharedDeck, DestinationCard *card, int number)
  {
    if ((card != nullptr && number > 0)|| (card==nullptr && number<=0))
    {
      if(card != nullptr && number > 0){
        throw std::invalid_argument("Cannot specify both a card and a number.");
      }
      else{
        throw std::invalid_argument("Must specify either a card or a number.");
      }
    }
    if (card != nullptr)
    {
      sharedDeck->drawCard(this, card, 0);
    }
    else if (number > 0)
    {
      sharedDeck->drawCard(this, nullptr, number);
    }
  }

  template <>
  void PlayerCards::_takeCard<WagonCard>(SharedDeck<WagonCard> *sharedDeck, WagonCard *card, int number)
  {
    if ((card != nullptr && number > 0)|| (card==nullptr && number<=0))
    {
      if(card != nullptr && number > 0){
        throw std::invalid_argument("Cannot specify both a card and a number.");
      }
      else{
        throw std::invalid_argument("Must specify either a card or a number.");
      }
    }
    if (card != nullptr)
    {
      sharedDeck->drawCard(this, card, 0);
    }
    else if (number > 0)
    {
      sharedDeck->drawCard(this, nullptr, number);
    }
  }

  template <>
  void PlayerCards::takeCard(CardsState * cardsState, DestinationCard *card, int number)
  {
    SharedDeck<DestinationCard> *sharedDeck= cardsState->gameDestinationCards.get();

    this->_takeCard<DestinationCard>(sharedDeck, card, number);
  }

  template <>
  void PlayerCards::takeCard(CardsState * cardsState, WagonCard *card, int number)
  {
    SharedDeck<WagonCard> *sharedDeck= cardsState->gameWagonCards.get();

    this->_takeCard<WagonCard>(sharedDeck, card, number);
  }

  template void PlayerCards::takeCard<DestinationCard>(CardsState *,DestinationCard* ,int);
  template void PlayerCards::takeCard<WagonCard>(CardsState *,WagonCard* ,int);
}


