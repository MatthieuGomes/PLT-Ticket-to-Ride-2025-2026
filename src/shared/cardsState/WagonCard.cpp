#include "WagonCard.h"

#include <iostream>

namespace cardsState {
    WagonCard::WagonCard(ColorCard color) {
        this->color = color;
    }
    ColorCard WagonCard::getColor() {
        return this->color;
    }
    void WagonCard::setColor(ColorCard color) {
            this->color = color;
    }
    void WagonCard::display() {
        std::cout<<" this a wagon card"<< std::endl;
        std::cout<<this->color<<std::endl;
    }

}