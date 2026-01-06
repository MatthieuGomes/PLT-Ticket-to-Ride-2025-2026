#include <iostream>

#include "DestinationCard.h"
#include "mapState/MapState.h"

namespace cardsState
{

    DestinationCard::DestinationCard(std::shared_ptr<mapState::Station> stationA, std::shared_ptr<mapState::Station> stationB, int points)
        : stationA(stationA), stationB(stationB), points(points)
    {
    }
    // TODO : add a UNKNOWN color to 404 + change number of NONE to 0
    // TODO :: make ColorCard inheritance
    /*
    Enum Colors 
        UNKNOWN = 0,
    Enum PlayerColors : Colors
        RED     = 1,
        BLUE     = 2,
        GREEN     = 3,
        BLACK     = 4,
        YELLOW     = 5
    Enum CommonColors : PlayerColors
        WHITE     = 6,
        PINK     = 7,
        ORANGE     = 8,
    Enum RoadColors : CommonColors
        NONE = 0,
    Enum WagonColors : CommonColors
        LOCOMOTIVE     = 42,
    */
    /*
    ColorCard DestinationCard::possibleColors[9] = {
        ColorCard::RED,
        ColorCard::BLUE,
        ColorCard::GREEN,
        ColorCard::WHITE,
        ColorCard::BLACK,
        ColorCard::YELLOW,
        ColorCard::PINK,
        ColorCard::ORANGE,
        ColorCard::LOCOMOTIVE,
    };
    */

    std::shared_ptr<mapState::Station> DestinationCard::getstationA()
    {
        return stationA;
    }
    std::shared_ptr<mapState::Station> DestinationCard::getstationB()
    {
        return stationB;
    }

    int DestinationCard::getPoints()
    {
        return points;
    }

    void DestinationCard::display()
    {
        std::cout << "Destination Card:" << std::endl;

        std::cout << "From: ";
        stationA->display();

        std::cout << "To: ";
        stationB->display();

        std::cout << "Points: " << points << std::endl;
    }
}
