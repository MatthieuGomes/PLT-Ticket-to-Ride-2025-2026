#include "CardsState.h"
#include "PlayerCards.h"
#include "SharedDeck.h"
#include "OutOfGame.h"
#include <iostream>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState
{
  class DestinationCard;
  class WagonCard;
  class PlayerCards;
}

namespace cardsState
{

  /// class CardsState -
  CardsState::CardsState()
  {
  }

  CardsState CardsState::Europe(std::vector<std::shared_ptr<mapState::Station>> stations)
  {
    CardsState cardsState;
    cardsState.outOfGameCards = std::make_shared<OutOfGame<DestinationCard>>();
    cardsState.gameDestinationCards = std::make_shared<SharedDeck<DestinationCard>>(SharedDeck<DestinationCard>::Europe(stations));
    cardsState.gameWagonCards = std::make_shared<SharedDeck<WagonCard>>(SharedDeck<WagonCard>::Init());
    cardsState.playersCards = PlayerCards::BatchStartHand(cardsState.gameDestinationCards, cardsState.gameWagonCards);
    cardsState.gameDestinationCards->Setup(cardsState.outOfGameCards);
    cardsState.gameWagonCards->Setup();
    return cardsState;
  }
  // TODO: Parameterized constructor
  void CardsState::display(int indent)
  {
    std::string indentation = std::string(indent, '\t');
    std::cout << indentation << "~~~~~ CARDS STATE ~~~~~\n";
    if (this->gameDestinationCards)
    {
      gameDestinationCards->display(indent + 1);
    }
    if (this->gameWagonCards)
    {
      gameWagonCards->display(indent + 1);
    }
    if (this->outOfGameCards)
    {
      outOfGameCards->display(indent + 1);
    }
    if (!this->playersCards.empty())
    {
      for (const auto &playerCards : this->playersCards)
      {
        if (playerCards)
        {
          playerCards->display(indent + 1);
        }
      }
    }

    std::cout << indentation << "~~~~~~~~~~~~~~~~~~~~~\n";
  }
}