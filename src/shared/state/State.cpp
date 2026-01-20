#include "State.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <json/json.h>

namespace state
{
    namespace {
        const int kMaxPlayers = 5;
    }
    using playersInitInfos = std::tuple<std::string, playersState::PlayerColor, std::shared_ptr<cardsState::PlayerCards>>;
    State::State()
    {
        map = mapState::MapState();
        cards = cardsState::CardsState();
        players = playersState::PlayersState();
        
        // Initialize state variables here
    }
    State::State(std::string mapName, std::vector<playersInitInfos> playersInfos){
        if (playersInfos.size() > static_cast<std::size_t>(kMaxPlayers))
        {
            std::cerr << "Error: too many players (" << playersInfos.size()
                      << "). Maximum is " << kMaxPlayers << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        this->map = mapState::MapState::NamedMapState(mapName);
        this->cards = cardsState::CardsState::Europe(map.getStations(), static_cast<int>(playersInfos.size()));
        this->players = playersState::PlayersState(playersInfos, cards.playersCards);
    }

    State::State(std::string pathToFile)
    {
        this->map = mapState::MapState();
        this->cards = cardsState::CardsState();
        this->players = playersState::PlayersState();

        std::ifstream file(pathToFile.c_str());
        if (!file)
        {
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonContent = buffer.str();
        if (jsonContent.empty())
        {
            return;
        }

        Json::Value root;
        Json::CharReaderBuilder builder;
        builder["collectComments"] = false;
        std::string error;
        std::istringstream input(jsonContent);
        if (!Json::parseFromStream(builder, input, &root, &error))
        {
            return;
        }

        Json::StreamWriterBuilder writer;
        writer["commentStyle"] = "None";
        writer["indentation"] = "";

        std::string jsonPlayers;
        std::string jsonMap;
        std::string jsonCards;

        if (root.isObject())
        {
            if (root.isMember("players") && root["players"].isArray() &&
                root["players"].size() > static_cast<Json::ArrayIndex>(kMaxPlayers))
            {
                std::cerr << "Error: too many players (" << root["players"].size()
                          << "). Maximum is " << kMaxPlayers << "." << std::endl;
                std::exit(EXIT_FAILURE);
            }
            if (root.isMember("players"))
            {
                jsonPlayers = Json::writeString(writer, root["players"]);
            }
            if (root.isMember("map"))
            {
                jsonMap = Json::writeString(writer, root["map"]);
            }
            if (root.isMember("cards"))
            {
                jsonCards = Json::writeString(writer, root["cards"]);
            }
            if (jsonMap.empty() && (root.isMember("mapName") || root.isMember("stations") || root.isMember("roads")))
            {
                jsonMap = Json::writeString(writer, root);
            }
            if (jsonPlayers.empty() && root.isMember("players"))
            {
                jsonPlayers = Json::writeString(writer, root["players"]);
            }
        }
        else if (root.isArray())
        {
            if (root.size() > static_cast<Json::ArrayIndex>(kMaxPlayers))
            {
                std::cerr << "Error: too many players (" << root.size()
                          << "). Maximum is " << kMaxPlayers << "." << std::endl;
                std::exit(EXIT_FAILURE);
            }
            jsonPlayers = jsonContent;
        }

        if (!jsonPlayers.empty())
        {
            this->players = playersState::PlayersState::InitFromJSON(jsonPlayers);
        }
        if (!jsonMap.empty())
        {
            this->map = mapState::MapState::ParseFromJSON(jsonMap, std::make_shared<playersState::PlayersState>(players));
        }
        if (!jsonCards.empty())
        {
            this->cards = cardsState::CardsState::ParseFromJSON(jsonCards, std::make_shared<mapState::MapState>(map));
        }
        if (!jsonPlayers.empty())
        {
            std::string setupJson = jsonPlayers;
            if (root.isObject())
            {
                setupJson = jsonContent;
            }
            this->players.setupFromJSON(setupJson,
                std::make_shared<mapState::MapState>(map),
                std::make_shared<cardsState::CardsState>(cards));
        }
    }

    State State::Europe()
    {
        return State("/app/static/europe_state.json");
    }

    void State::display(int indent)
    {
        std::string indentation = std::string(indent, '\t');
        std::cout << indentation << "¤¤¤¤¤ STATE ¤¤¤¤¤" << std::endl;
        map.display(indent + 1);
        cards.display(indent + 1);
        players.display(indent + 1);
        std::cout << indentation << "¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤" << std::endl;
        // Here @theo + @chahrazed + @yosra will implement the details to print their part of the state
    }
}
