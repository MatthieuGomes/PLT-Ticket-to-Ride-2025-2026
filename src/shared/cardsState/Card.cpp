#include "Card.h"

#include <iostream>


namespace cardsState {

    Card::~Card() {
        std::cout<<"Destroying Card"<< std::endl;
    };
}
