#include <iostream>
#include <ostream>

#include "DestinationCard.h"
namespace cardsState {
    DestinationCard:: DestinationCard (std::string cityA, std::string cityB, int points){
        this->cityA=cityA;
        this->cityB=cityB;
        this->points=points;
    }
   std::string DestinationCard::getCityA () {return this->cityA;}
    std::string DestinationCard::getCityB () {return this->cityB;}
    int DestinationCard::getPoints () {return this->points;}
   void DestinationCard:: display() {
        std::cout<<" this a destinationn card"<<std::endl;
        std::cout<<cityA<<" "<<cityB<<" "<<points<<std::endl;
    }


}