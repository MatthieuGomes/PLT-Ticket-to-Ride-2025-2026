#include "BorrowRoadState.h"

#include <json/json.h>

#include <sstream>

#include "ConfirmationState.h"
#include "Engine.h"
#include "EngineCommand.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "mapState/MapState.h"
#include "mapState/Road.h"
#include "playersState/Player.h"
#include "playersState/PlayersState.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::BORROW_ROAD;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }

    void addInfo(EngineResult& result, const std::string& message)
    {
      EngineEvent event;
      event.type = EngineEventType::INFO;
      event.message = message;
      event.payload = "";
      result.events.push_back(event);
    }

    bool parsePayload(const std::string& payload, Json::Value& root, std::string& error)
    {
      error.clear();
      if (payload.empty())
      {
        error = "Missing payload";
        return false;
      }
      Json::CharReaderBuilder builder;
      builder["collectComments"] = false;
      std::string errs;
      std::istringstream stream(payload);
      if (!Json::parseFromStream(builder, stream, &root, &errs))
      {
        error = "Invalid payload: " + errs;
        return false;
      }
      if (!root.isObject())
      {
        error = "Payload must be an object";
        return false;
      }
      return true;
    }

    int readRoadId(const Json::Value& root)
    {
      if (root.isMember("roadId"))
      {
        return root["roadId"].asInt();
      }
      if (root.isMember("id"))
      {
        return root["id"].asInt();
      }
      if (root.isMember("road_id"))
      {
        return root["road_id"].asInt();
      }
      return -1;
    }
  }

  void BorrowRoadState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (engine)
    {
      engine->phase = Phase::BORROW_ROAD;
    }
  }

  EngineResult BorrowRoadState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Borrow road: engine not initialized");
    }
    if (command.type != EngineCommandType::CMD_BORROW_ROAD)
    {
      return buildError(engine, "Borrow road: command not allowed");
    }

    Json::Value root;
    std::string error;
    if (!parsePayload(command.payload, root, error))
    {
      return buildError(engine, error);
    }

    int roadId = readRoadId(root);
    if (roadId < 0)
    {
      return buildError(engine, "Borrow road: missing road id");
    }

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      return buildError(engine, "Borrow road: missing state");
    }
    std::shared_ptr<mapState::MapState> mapState(state, &state->map);
    if (!mapState)
    {
      return buildError(engine, "Borrow road: missing map");
    }

    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    int playerIndex = engine->context.currentPlayer;
    if (players.empty() || playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
      return buildError(engine, "Borrow road: invalid player");
    }
    std::shared_ptr<playersState::Player> player = players[playerIndex];
    if (!player)
    {
      return buildError(engine, "Borrow road: missing player");
    }

    std::shared_ptr<mapState::Road> road = mapState->getRoadByID(roadId);
    if (!road)
    {
      return buildError(engine, "Borrow road: road not found");
    }
    if (!road->getOwner())
    {
      return buildError(engine, "Borrow road: road is unowned");
    }
    if (road->getOwner() == player)
    {
      return buildError(engine, "Borrow road: cannot borrow your own road");
    }

    std::vector<std::shared_ptr<mapState::Road>> borrowed = player->getBorrowedRoads();
    for (const std::shared_ptr<mapState::Road>& existing : borrowed)
    {
      if (existing && existing->getId() == road->getId())
      {
        return buildError(engine, "Borrow road: already borrowed");
      }
    }

    player->borrowedRoads.push_back(road);

    EngineResult result;
    result.ok = true;
    result.error = "";
    result.nextPhase = Phase::CONFIRMATION;
    addInfo(result, "Borrowed road " + std::to_string(road->getId()) + ".");

    std::shared_ptr<GameState> nextState(new ConfirmationState());
    engine->stateMachine->transitionTo(engine, nextState);
    return result;
  }

  std::vector<EngineCommandType> BorrowRoadState::getAllowedCommands()
  {
    std::vector<EngineCommandType> allowed;
    allowed.push_back(EngineCommandType::CMD_BORROW_ROAD);
    allowed.push_back(EngineCommandType::CMD_EXIT);
    return allowed;
  }
}
