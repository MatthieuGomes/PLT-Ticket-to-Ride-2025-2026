#include "DestinationCard.h"
#include <iostream>

cardState::DestinationCard::DestinationCard(int id_, const std::string& a, const std::string& b, int p)
    : Card(id_), cityA(a), cityB(b), points(p) {}

std::string cardState::DestinationCard::getCityA() const { return cityA; }
std::string cardState::DestinationCard::getCityB() const { return cityB; }
int cardState::DestinationCard::getPoints() const { return points; }

void cardState::DestinationCard::display() const {
    std::cout << "DestinationCard #" << id << " " << cityA << " - " << cityB
              << " (" << points << " points)" << std::endl;
}
