#include "Card.h"

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

    Card::~Card()
    {
        DEBUG_PRINT("Destroying Card");
    }
}
