#include <iostream>

#include "DestinationCard.h"
#include "mapState/Station.h"



namespace cardsState {

    DestinationCard::DestinationCard(std::string stationA, std::string stationB, int points)
      : stationA(stationA), stationB(stationB), points(points) {}

    std::string DestinationCard::getstationA() {
        return stationA;
    }

    std::string DestinationCard::getstationB() {
        return stationB;
    }

    int DestinationCard::getPoints() {
        return points;
    }
/*
    void DestinationCard::display() {
        std::cout << "Destination Card: " << stationA->data->name << " to " << stationB->data->name
                  << " for " << points << " points." << std::endl;
    }*/
    void DestinationCard::display() {
        std::cout << "Destination Card: " << stationA << " to " << stationB
                  << " for " << points << " points." << std::endl;
    }
}
