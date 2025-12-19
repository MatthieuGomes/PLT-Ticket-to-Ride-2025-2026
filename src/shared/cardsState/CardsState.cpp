#include "CardsState.h"
#include "PlayerCards.h"
#include "SharedDeck.h"
#include <iostream>

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
    this->gameDestinationCards = std::make_shared<SharedDeck<DestinationCard>>(std::vector<std::shared_ptr<DestinationCard>>(), std::vector<std::shared_ptr<DestinationCard>>(), std::vector<std::shared_ptr<DestinationCard>>());
    this->gameWagonCards = std::make_shared<SharedDeck<WagonCard>>(std::vector<std::shared_ptr<WagonCard>>(), std::vector<std::shared_ptr<WagonCard>>(), std::vector<std::shared_ptr<WagonCard>>());
    this->playersCards = std::vector<std::shared_ptr<PlayerCards>>{};
  }
  void CardsState::printCardsState()
  {
    std::cout << " Current Cards State " << std::endl;

    gameDestinationCards->display();
    gameWagonCards->display();

    for (const auto &playerCards : playersCards)
    {
      std::cout << "player cards:" << std::endl;
      if (playerCards)
      {
        playerCards->display();
      }
    }
  }
}