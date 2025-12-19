#include "PlayerCards.h"
#include <iostream>
#include "SharedDeck.h"
#include "CardsState.h"
namespace cardsState
{
  PlayerCards::PlayerCards(std::vector<std::shared_ptr<DestinationCard>> destinationCards, std::vector<std::shared_ptr<WagonCard>> wagonCards)
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
  void PlayerCards::_takeCard<DestinationCard>(std::shared_ptr<SharedDeck<DestinationCard>> sharedDeck, std::shared_ptr<DestinationCard> card, int number)
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
      sharedDeck->drawCard(std::make_shared<PlayerCards>(*this), card, 0);
    }
    else if (number > 0)
    {
      sharedDeck->drawCard(std::make_shared<PlayerCards>(*this), nullptr, number);
    }
  }

  template <>
  void PlayerCards::_takeCard<WagonCard>(std::shared_ptr<SharedDeck<WagonCard>> sharedDeck, std::shared_ptr<WagonCard> card, int number)
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
      sharedDeck->drawCard(std::make_shared<PlayerCards>(*this), card, 0);
    }
    else if (number > 0)
    {
      sharedDeck->drawCard(std::make_shared<PlayerCards>(*this), nullptr, number);
    }
  }

  template <>
  void PlayerCards::takeCard(std::shared_ptr<cardsState::CardsState> cardsState, std::shared_ptr<DestinationCard> card, int number)
  {
    std::shared_ptr<SharedDeck<DestinationCard>> sharedDeck= cardsState->gameDestinationCards;

    this->_takeCard<DestinationCard>(sharedDeck, card, number);
  }

  template <>
  void PlayerCards::takeCard(std::shared_ptr<cardsState::CardsState> cardsState, std::shared_ptr<WagonCard> card, int number)
  {
    std::shared_ptr<SharedDeck<WagonCard>> sharedDeck= cardsState->gameWagonCards;

    this->_takeCard<WagonCard>(sharedDeck, card, number);
  }

  template void PlayerCards::takeCard<DestinationCard>(std::shared_ptr<cardsState::CardsState>, std::shared_ptr<DestinationCard>, int);
  template void PlayerCards::takeCard<WagonCard>(std::shared_ptr<cardsState::CardsState>, std::shared_ptr<WagonCard>, int);
}



