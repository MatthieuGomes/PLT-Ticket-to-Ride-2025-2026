#include "DestinationCard.h"
#include <iostream>

DestinationCard::DestinationCard(int id_, const std::string& a, const std::string& b, int p)
    : Card(id_), cityA(a), cityB(b), points(p) {}

std::string DestinationCard::getCityA() const { return cityA; }
std::string DestinationCard::getCityB() const { return cityB; }
int DestinationCard::getPoints() const { return points; }

void DestinationCard::display() const {
    std::cout << "DestinationCard #" << id << " " << cityA << " - " << cityB
              << " (" << points << " points)" << std::endl;
}
