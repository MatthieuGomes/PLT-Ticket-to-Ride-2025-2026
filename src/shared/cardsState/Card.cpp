#include "Card.h"

#include <iostream>
#include <ostream>

namespace cardsState {
    Card::~Card() {
        std::cout<<"Destroying Card"<< std::endl;
    };
}
