#include "CardState.h"
#include "WagonCard.h"

int main() {
    CardState state;

    for (int i = 0; i < 30; ++i)
        state.wagonDeck.addCard(std::make_shared<WagonCard>(i, ColorCard::RED));

    state.wagonDeck.shuffle(7, 5, 3, 11, 9, 8);

    DeckPlayer player;
    state.playerHands.push_back(player);

    for (int i = 0; i < 4; ++i) {
        auto card = state.wagonDeck.removeCard();
        state.onPick(card, state.playerHands[0]);
    }

    state.displayAll();
    state.playerHands[0].displayDeck();
    return 0;
}
