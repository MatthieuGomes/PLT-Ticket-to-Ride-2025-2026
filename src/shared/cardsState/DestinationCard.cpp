#include <iostream>
#include <ostream>

#include "DestinationCard.h"
namespace cardsState {
    /*
DestinationCard Constructor
@param cityA string Name of the first city.
@param cityB string Name of the second city.
@param points int  The number of points for this destination.
Construtcts a new DestinationCard with the first city, second city and the points    
    */
    DestinationCard:: DestinationCard (std::string cityA, std::string cityB, int points){
        this->cityA=cityA;
        this->cityB=cityB;
        this->points=points;
    }
    /*
getCityA
@return string cityA
gets the name of the city A
    */
    std::string DestinationCard::getCityA () {return this->cityA;}
    /*
getCityB
@return string cityB
gets the name of the city A
    */
    std::string DestinationCard::getCityB () {return this->cityB;}
    /*
getPoints
@return int points
gets the point value of the destination card
    */
    int DestinationCard::getPoints () {return this->points;}
    /*
setPoints
@param points int
set the cards points
    */
    void DestinationCard::setPoints (int points) {this->points=points;}
    /*
setCityB
@param cityA string
sets the city A
    */
    void DestinationCard::setCityA (std::string cityA) {this->cityA=cityA;}
    /*
setCityB
@param cityB string
sets the city B
    */
    void DestinationCard::setCityB (std::string cityB) {this->cityB=cityB;}
    /*
display
Displays detailed information about the destination card
    */
    void DestinationCard::display() {
        std::cout << "Destination from " << cityA << " to " << cityB
                << " worth " << points << " points." << std::endl;
    }
}