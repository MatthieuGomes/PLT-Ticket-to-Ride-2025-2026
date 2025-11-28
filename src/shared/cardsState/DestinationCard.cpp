#include <iostream>
#include <ostream>

#include "DestinationCard.h"
#include "mapState/Station.h"



namespace cardsState {

    DestinationCard::DestinationCard(mapState::Station* stationA, mapState::Station* stationB, int points)
      : stationA(stationA), stationB(stationB), points(points) {}

    mapState::Station* DestinationCard::getstationA() {
        return stationA;
    }

    mapState::Station* DestinationCard::getstationB() {
        return stationB;
    }

    int DestinationCard::getPoints() {
        return points;
    }

}
