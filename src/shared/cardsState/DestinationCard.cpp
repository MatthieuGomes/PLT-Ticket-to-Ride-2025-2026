#include <iostream>

#include "DestinationCard.h"
#include "mapState/Station.h"



namespace cardsState {

    DestinationCard::DestinationCard(std::shared_ptr<mapState::Station> stationA, std::shared_ptr<mapState::Station> stationB, int points)
      : stationA(stationA), stationB(stationB), points(points) {}



    std::shared_ptr<mapState::Station> DestinationCard::getstationA() {
        return stationA;
    }
    std::shared_ptr<mapState::Station> DestinationCard::getstationB() {
        return stationB;
    }

    int DestinationCard::getPoints() {
        return points;
    }

    void DestinationCard::display() {
        std::cout << "Destination Card:" << std::endl;

        std::cout << "From: ";
        stationA->display();

        std::cout << "To: ";
        stationB->display();

        std::cout << "Points: " << points << std::endl;
    }
}
