#include <iostream>

#include "WagonCard.h"
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

    using WagonCardInfos = cardsState::ColorCard;
    using ColorCardCount = std::pair<ColorCard, int>;

    int WagonCard::locomotiveInitCardsCount = 14;
    int WagonCard::otherInitCardsCount = 12;
    int WagonCard::faceUpCardsCount = 5;

    ColorCard WagonCard::possibleColors[10] = {
        ColorCard::UNKNOWN,
        ColorCard::RED,
        ColorCard::BLUE,
        ColorCard::GREEN,
        ColorCard::WHITE,
        ColorCard::BLACK,
        ColorCard::YELLOW,
        ColorCard::PINK,
        ColorCard::ORANGE,
        ColorCard::LOCOMOTIVE};

    std::unordered_map<ColorCard, std::string> WagonCard::ColorsNames = {
        {ColorCard::UNKNOWN, "UNKNOWN"},
        {ColorCard::RED, "RED"},
        {ColorCard::BLUE, "BLUE"},
        {ColorCard::GREEN, "GREEN"},
        {ColorCard::BLACK, "BLACK"},
        {ColorCard::YELLOW, "YELLOW"},
        {ColorCard::ORANGE, "ORANGE"},
        {ColorCard::PINK, "PINK"},
        {ColorCard::WHITE, "WHITE"},
        {ColorCard::LOCOMOTIVE, "LOCOMOTIVE"}};

    ColorCardCount wagonCardsCompo[9] = {
        {ColorCard::RED, WagonCard::otherInitCardsCount},
        {ColorCard::BLUE, WagonCard::otherInitCardsCount},
        {ColorCard::GREEN, WagonCard::otherInitCardsCount},
        {ColorCard::BLACK, WagonCard::otherInitCardsCount},
        {ColorCard::YELLOW, WagonCard::otherInitCardsCount},
        {ColorCard::ORANGE, WagonCard::otherInitCardsCount},
        {ColorCard::PINK, WagonCard::otherInitCardsCount},
        {ColorCard::WHITE, WagonCard::otherInitCardsCount},
        {ColorCard::LOCOMOTIVE, WagonCard::locomotiveInitCardsCount},
    };

    WagonCard::WagonCard()
    {
        this->color = ColorCard::UNKNOWN;
    }
    WagonCard::WagonCard(ColorCard color)
    {
        this->color = color;
    }

    std::vector<std::shared_ptr<WagonCard>> WagonCard::BatchConstructor(std::vector<WagonCardInfos> infos)
    {
        std::vector<std::shared_ptr<WagonCard>> wagonCards;
        for (WagonCardInfos info : infos)
        {
            wagonCards.push_back(std::make_shared<WagonCard>(info));
        }
        return wagonCards;}
    std::vector<std::shared_ptr<WagonCard>> WagonCard::OrderedBatch(ColorCardCount (&infos)[9])
    {

        std::vector<std::shared_ptr<WagonCard>> wagonCards;
        for (ColorCardCount info : infos)
        {
            ColorCard color = info.first;
            int count = info.second;
            for (int i = 0; i < count; i++)
            {
                wagonCards.push_back(std::make_shared<WagonCard>(color));
            }
        }
        return wagonCards;}
    
    std::vector<std::shared_ptr<WagonCard>> WagonCard::StartCards()
    {
        std::vector<std::shared_ptr<WagonCard>> wagonCards = WagonCard::OrderedBatch(wagonCardsCompo);
        std::vector<std::shared_ptr<WagonCard>> shuffledWagonCards = Deck<WagonCard>::shuffle(wagonCards);
        return shuffledWagonCards;
    }

    ColorCard WagonCard::getColor()
    {
        return this->color;
    }
    void WagonCard::setColor(ColorCard color)
    {
        this->color = color;
    }

    void WagonCard::display(int indent)
    {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "----- WAGON CARD -----" << std::endl;
        this->_display(indent);
        std::cout << indentation << "---------------------" << std::endl;
    }
    void WagonCard::_display(int indent)
    {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "Color: " << WagonCard::ColorsNames[this->color] << std::endl;
    }
}