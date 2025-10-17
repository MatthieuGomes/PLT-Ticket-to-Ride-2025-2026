#include "WagonCard.h"
#include <iostream>

WagonCard::WagonCard(int id_, ColorCard c) : Card(id_), color(c) {}

ColorCard WagonCard::getColor() const { return color; }

void WagonCard::display() const {
    std::cout << "WagonCard #" << id << " [" << colorCardToString(color) << "]" << std::endl;
}
