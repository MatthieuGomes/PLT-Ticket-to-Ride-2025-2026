#include "AIController.h"

#include <cctype>
#include <cstdlib>
#include <ctime>
#include <sstream>

#include "Engine.h"
#include "EngineCommandType.h"
#include "StateMachine.h"
#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "mapState/Station.h"
#include "parser/JSONParser.h"

namespace engine
{
  namespace
  {
    const int kDefaultSeed = 12345;
    const int kDefaultFaceupIndex = 1;

    int clampIndex(int value, int minValue, int maxValue)
    {
      if (value < minValue)
      {
        return minValue;
      }
      if (value > maxValue)
      {
        return maxValue;
      }
      return value;
    }

    bool isCommandSupported(EngineCommandType type)
    {
      if (type == EngineCommandType::CMD_EXIT || type == EngineCommandType::CMD_HELP)
      {
        return false;
      }
      return true;
    }

    parser::CommandMessage makeCommand(const std::string& name)
    {
      parser::CommandMessage message;
      message.kind = "command";
      message.origin = "tui";
      message.version = 1;
      message.name = name;
      return message;
    }

    parser::CommandMessage buildSimpleCommand(EngineCommandType type)
    {
      if (type == EngineCommandType::CMD_DRAW_DESTINATION)
      {
        return makeCommand("draw_destination");
      }
      if (type == EngineCommandType::CMD_DRAW_WAGON_FACEUP)
      {
        parser::CommandMessage message = makeCommand("draw_faceup");
        message.payload = Json::Value(Json::objectValue);
        message.payload["index"] = kDefaultFaceupIndex;
        return message;
      }
      if (type == EngineCommandType::CMD_DRAW_WAGON_FACEDOWN)
      {
        return makeCommand("draw_facedown");
      }
      if (type == EngineCommandType::CMD_CONFIRM_ENDTURN)
      {
        return makeCommand("confirm");
      }
      return makeCommand("confirm");
    }

    std::string findAnyStation(const std::shared_ptr<state::State>& state)
    {
      if (!state)
      {
        return "";
      }
      std::vector<std::shared_ptr<mapState::Station>> stations = state->map.getStations();
      if (stations.empty() || !stations[0])
      {
        return "";
      }
      return stations[0]->getName();
    }

    int findAnyRoadId(const std::shared_ptr<state::State>& state)
    {
      if (!state)
      {
        return -1;
      }
      std::vector<std::shared_ptr<mapState::Road>> roads = state->map.getRoads();
      if (roads.empty() || !roads[0])
      {
        return -1;
      }
      return roads[0]->getId();
    }
  }

  parser::CommandMessage AIController::nextCommand(std::shared_ptr<state::State> state,
                                                   std::shared_ptr<Engine> engine)
  {
    if (!engine || !engine->stateMachine)
    {
      return makeCommand("confirm");
    }

    if (seed == 0)
    {
      seed = kDefaultSeed;
    }
    std::srand(static_cast<unsigned int>(seed));

    std::shared_ptr<GameState> current = engine->stateMachine->getState();
    if (!current)
    {
      return makeCommand("confirm");
    }

    std::vector<EngineCommandType> allowed = current->getAllowedCommands();
    std::vector<EngineCommandType> usable;
    for (std::size_t i = 0; i < allowed.size(); ++i)
    {
      if (isCommandSupported(allowed[i]))
      {
        usable.push_back(allowed[i]);
      }
    }

    if (usable.empty())
    {
      parser::CommandMessage message;
      message.kind = "command";
      message.origin = "tui";
      message.version = 1;
      message.name = "";
      return message;
    }

    int index = std::rand() % static_cast<int>(usable.size());
    EngineCommandType choice = usable[static_cast<std::size_t>(index)];

    if (choice == EngineCommandType::CMD_TAKE_ROAD || choice == EngineCommandType::CMD_BORROW_ROAD)
    {
      int roadId = findAnyRoadId(state);
      if (roadId < 0)
      {
        return buildSimpleCommand(EngineCommandType::CMD_CONFIRM_ENDTURN);
      }
      parser::CommandMessage message = makeCommand(choice == EngineCommandType::CMD_TAKE_ROAD ? "take" : "borrow");
      message.payload = Json::Value(Json::objectValue);
      message.payload["roadId"] = roadId;
      return message;
    }

    if (choice == EngineCommandType::CMD_CLAIM_STATION)
    {
      std::string station = findAnyStation(state);
      if (station.empty())
      {
        return buildSimpleCommand(EngineCommandType::CMD_CONFIRM_ENDTURN);
      }
      parser::CommandMessage message = makeCommand("claim_station");
      message.payload = Json::Value(Json::objectValue);
      message.payload["station"] = station;
      return message;
    }

    return buildSimpleCommand(choice);
  }

  void AIController::notify(const EngineResult& result)
  {
    (void)result;
  }
}
