#include "OutOfGame.h"
#include <iostream>

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState {
    // class OutOfGame -
    template <class CardType>
    OutOfGame<CardType>::OutOfGame() : Deck<CardType>()
    {
    }
    template <class CardType>
    OutOfGame<CardType>::OutOfGame(std::vector<CardType> cards) : Deck<CardType>(cards)
    {
    }

    template<>
    OutOfGame<WagonCard>::OutOfGame(std::vector<std::shared_ptr<WagonCard>> cards) : Deck<WagonCard>(cards)
    {
    }

    template<>
    OutOfGame<DestinationCard>::OutOfGame(std::vector<std::shared_ptr<DestinationCard>> cards) : Deck<DestinationCard>(cards)
    {
    }

    template <class CardType>
    void OutOfGame<CardType>::display(int indent) {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "===== OUT OF GAME CARDS =====\n";
        Deck<CardType>::_display(indent);
        std::cout << indentation << "=============================\n";
    }

    template class OutOfGame<DestinationCard>;
    template class OutOfGame<WagonCard>;
}