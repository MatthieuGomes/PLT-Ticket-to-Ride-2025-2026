#include <iostream>

#include "DestinationCard.h"
#include "mapState/MapState.h"
#include "Deck.h"

#define DEBUG_MODE false
#if DEBUG_MODE == true
#define DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

namespace cardsState
{
    using DestinationCardInfos = std::tuple<std::shared_ptr<mapState::Station>, std::shared_ptr<mapState::Station>, int, bool>;
    using DestinationCardInfosNames = std::tuple<std::string, std::string, int, bool>;

    std::vector<DestinationCardInfosNames> europeCardsInfosNames = {
        {"athina", "angora", 5, false},
        {"budapest", "sofia", 5, false},
        {"frankfurt", "kobenhavn", 5, false},
        {"rostov", "erzurum", 5, false},
        {"sofia", "smyrna", 5, false},
        {"kyiv", "petrograd", 6, false},
        {"zurich", "brindisi", 6, false},
        {"zurich", "budapest", 6, false},
        {"warszawa", "smolensk", 6, false},
        {"zagrab", "brindisi", 6, false},
        {"paris", "zagreb", 7, false},
        {"brest", "marseille", 7, false},
        {"london", "berlin", 7, false},
        {"edinburgh", "paris", 7, false},
        {"amsterdam", "pamplona", 7, false},
        {"roma", "smyrna", 8, false},
        {"palermo", "constantinople", 8, false},
        {"sarajevo", "sevastopol", 8, false},
        {"madrid", "dieppe", 8, false},
        {"barcelona", "bruxelles", 8, false},
        {"paris", "wien", 8, false},
        {"barcelona", "munchen", 8, false},
        {"brest", "venezia", 8, false},
        {"smolensk", "rostov", 8, false},
        {"marseille", "essen", 8, false},
        {"kyiv", "sochi", 8, false},
        {"madrid", "zurich", 8, false},
        {"berlin", "bucuresti", 8, false},
        {"bruxelles", "danzig", 9, false},
        {"berlin", "roma", 9, false},
        {"angora", "kharkov", 10, false},
        {"riga", "bucuresti", 10, false},
        {"essen", "kyiv", 10, false},
        {"venizia", "constantinople", 10, false},
        {"london", "wien", 10, false},
        {"athina", "wilno", 11, false},
        {"stockholm", "wien", 11, false},
        {"berlin", "moskva", 12, false},
        {"amsterdam", "wilno", 12, false},
        {"frankfurt", "smolensk", 13, false},
        {"lisboa", "danzig", 20, true},
        {"brest", "petrograd", 20, true},
        {"palermo", "moskva", 20, true},
        {"kobenhavn", "erzurum", 21, true},
        {"edinburgh", "athina", 21, true},
        {"cadiz", "stockholm", 21, true}};
    
    // TODO ? add EuropeLongNb + other variables to avoid magic numbers
    DestinationCard::DestinationCard()
    {
        this->points = 0;
    }
    DestinationCard::DestinationCard(std::shared_ptr<mapState::Station> stationA, std::shared_ptr<mapState::Station> stationB, int points, bool isLong)
        : stationA(stationA), stationB(stationB), points(points), isLong(isLong)
    {
    }

    std::vector<std::shared_ptr<DestinationCard>> DestinationCard::BatchConstructor(std::vector<DestinationCardInfos> infos)
    {
        std::vector<std::shared_ptr<DestinationCard>> destinationCards;
        for (DestinationCardInfos info : infos)
        {
            destinationCards.push_back(std::make_shared<DestinationCard>(std::get<0>(info), std::get<1>(info), std::get<2>(info), std::get<3>(info)));
        }
        std::vector<std::shared_ptr<DestinationCard>> shuffledCards = Deck<DestinationCard>::shuffle(destinationCards);
        return shuffledCards;
    }
    std::vector<std::shared_ptr<DestinationCard>> DestinationCard::BatchConstructorByName(std::vector<DestinationCardInfosNames> infos, std::vector<std::shared_ptr<mapState::Station>> stations)
    {
        std::vector<std::shared_ptr<DestinationCard>> destinationCards;
        for (DestinationCardInfosNames info : infos)
        {
            std::shared_ptr<mapState::Station> stationA = mapState::Station::getStationByName(stations, std::get<0>(info));
            std::shared_ptr<mapState::Station> stationB = mapState::Station::getStationByName(stations, std::get<1>(info));
            destinationCards.push_back(std::make_shared<DestinationCard>(stationA, stationB, std::get<2>(info), std::get<3>(info)));
        }
        std::vector<std::shared_ptr<DestinationCard>> shuffledCards = Deck<DestinationCard>::shuffle(destinationCards);
        return shuffledCards;
    }

    std::vector<std::shared_ptr<DestinationCard>> DestinationCard::Europe(std::vector<std::shared_ptr<mapState::Station>> stations)
    {
        std::vector<std::shared_ptr<DestinationCard>> destinationCards;
        std::vector<DestinationCardInfosNames> infos = europeCardsInfosNames;
        destinationCards = DestinationCard::BatchConstructorByName(infos, stations);
        return destinationCards;
    }

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

    void DestinationCard::display(int indent)
    {

        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "----- DESTINATION CARD -----" << std::endl;
        this->_display(indent);
        std::cout << indentation << "-------------------------" << std::endl;
    }
    void DestinationCard::_display(int indent)
    {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "Station A: " << ((this->stationA != nullptr) ? stationA->getName() : "None") << std::endl;
        std::cout << indentation << "Station B: " << ((this->stationB != nullptr) ? stationB->getName() : "None") << std::endl;
        std::cout << indentation << "Points: " << points << std::endl;
        std::cout << indentation << "Is Long: " << (isLong ? "Yes" : "No") << std::endl;
    }

}
