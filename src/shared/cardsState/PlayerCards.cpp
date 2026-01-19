#include "PlayerCards.h"
#include <iostream>
#include "SharedDeck.h"
#include "CardsState.h"
#include "playersState/PlayersState.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState
{
  int PlayerCards::startingWagonCardsCount = 4;
  int PlayerCards::startingLongDestinationCardsCount = 1;
  int PlayerCards::startingShortDestinationCardsCount = 3;

  PlayerCards::PlayerCards()
  {
  }

  PlayerCards::PlayerCards(std::vector<std::shared_ptr<DestinationCard>> destinationCards, std::vector<std::shared_ptr<WagonCard>> wagonCards)
  {
    this->destinationCards = std::make_shared<Deck<DestinationCard>>(destinationCards);
    this->wagonCards = std::make_shared<Deck<WagonCard>>(wagonCards);
  }

  PlayerCards PlayerCards::StartHand(std::shared_ptr<SharedDeck<DestinationCard>> sharedDestinationDecks, std::shared_ptr<SharedDeck<WagonCard>> sharedWagonDecks)
  {
    std::vector<std::shared_ptr<DestinationCard>> startingDestinationCards;
    std::vector<std::shared_ptr<WagonCard>> startingWagonCards;
    std::shared_ptr<FaceDownCards<DestinationCard>> sharedDestinationDeck = sharedDestinationDecks->faceDownCards;
    std::shared_ptr<FaceDownCards<WagonCard>> sharedWagonDeck = sharedWagonDecks->faceDownCards;
    for (int i = 0; i < PlayerCards::startingLongDestinationCardsCount; ++i)
    {
      int cardPosition = 0;
      for (std::shared_ptr<DestinationCard> card : sharedDestinationDeck->getCards())
      {
        if (card->isLong)
        {
          startingDestinationCards.push_back(sharedDestinationDeck->removeCard(cardPosition));
          break;
        }
        cardPosition++;
      }
    }
    for (int i = 0; i < PlayerCards::startingShortDestinationCardsCount; ++i)
    {
      int cardPosition = 0;
      for (std::shared_ptr<DestinationCard> card : sharedDestinationDeck->getCards())
      {
        if (!card->isLong)
        {
          startingDestinationCards.push_back(sharedDestinationDeck->removeCard(cardPosition));
          break;
        }
        cardPosition++;
      }
    }
    for (int i = 0; i < PlayerCards::startingWagonCardsCount; ++i)
    {
      std::shared_ptr<WagonCard> card = sharedWagonDeck->removeCard(0);
      startingWagonCards.push_back(card);
    }
    return PlayerCards(startingDestinationCards, startingWagonCards);
  }

  std::vector<std::shared_ptr<PlayerCards>> PlayerCards::BatchStartHand(std::shared_ptr<SharedDeck<DestinationCard>> sharedDestinationDecks, std::shared_ptr<SharedDeck<WagonCard>> sharedWagonDecks, int nbPLayers)
  {
    std::vector<std::shared_ptr<PlayerCards>> playersHands;
    for (int i = 0; i < nbPLayers; ++i)
    {
      PlayerCards playerHand = PlayerCards::StartHand(sharedDestinationDecks, sharedWagonDecks);
      playersHands.push_back(std::make_shared<PlayerCards>(playerHand));
    }
    return playersHands;}

  template <>
  void PlayerCards::_takeCard<DestinationCard>(std::shared_ptr<SharedDeck<DestinationCard>> sharedDeck, std::shared_ptr<DestinationCard> card, int number)
  {
    if ((card != nullptr && number > 0) || (card == nullptr && number <= 0))
    {
      if (card != nullptr && number > 0)
      {
        throw std::invalid_argument("Cannot specify both a card and a number.");
      }
      else
      {
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
    if ((card != nullptr && number > 0) || (card == nullptr && number <= 0))
    {
      if (card != nullptr && number > 0)
      {
        throw std::invalid_argument("Cannot specify both a card and a number.");
      }
      else
      {
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
    std::shared_ptr<SharedDeck<DestinationCard>> sharedDeck = cardsState->gameDestinationCards;

    this->_takeCard<DestinationCard>(sharedDeck, card, number);
  }

  template <>
  void PlayerCards::takeCard(std::shared_ptr<cardsState::CardsState> cardsState, std::shared_ptr<WagonCard> card, int number)
  {
    std::shared_ptr<SharedDeck<WagonCard>> sharedDeck = cardsState->gameWagonCards;

    this->_takeCard<WagonCard>(sharedDeck, card, number);
  }

  void PlayerCards::display(int indent)
  {
    std::string indentation = std::string(indent, '\t');
    std::cout << indentation << "##### PLAYER CARDS #####\n";
    if (this->destinationCards)
    {
      std::cout << indentation << "\t" << "===== DESTINATION CARDS =====\n";
      this->destinationCards->_display(indent + 1);
      std::cout << indentation << "\t" << "=============================\n";
    }
    if (this->wagonCards)
    {
      std::cout << indentation << "\t" << "===== WAGON CARDS =====\n";
      this->wagonCards->_display(indent + 1);
      std::cout << indentation << "\t" << "=======================\n";
    }
    std::cout << indentation << "########################\n";
  }

  template void PlayerCards::takeCard<DestinationCard>(std::shared_ptr<cardsState::CardsState>, std::shared_ptr<DestinationCard>, int);
  template void PlayerCards::takeCard<WagonCard>(std::shared_ptr<cardsState::CardsState>, std::shared_ptr<WagonCard>, int);
}
