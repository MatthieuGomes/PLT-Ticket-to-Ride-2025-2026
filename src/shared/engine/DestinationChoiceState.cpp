#include "DestinationChoiceState.h"

#include <json/json.h>

#include <sstream>

#include "ConfirmationState.h"
#include "Engine.h"
#include "EngineCommand.h"
#include "EngineEvent.h"
#include "StateMachine.h"
#include "cardsState/CardsState.h"
#include "cardsState/DestinationCard.h"
#include "cardsState/SharedDeck.h"

namespace engine
{
  namespace
  {
    EngineResult buildError(std::shared_ptr<Engine> engine, const std::string& message)
    {
      EngineResult result;
      result.ok = false;
      result.error = message;
      result.nextPhase = engine ? engine->phase : Phase::DESTINATION_CHOICE;

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

    std::vector<int> parseIndices(const Json::Value& root)
    {
      std::vector<int> indices;
      if (root.isMember("indices") && root["indices"].isArray())
      {
        for (Json::ArrayIndex i = 0; i < root["indices"].size(); ++i)
        {
          if (root["indices"][i].isInt())
          {
            indices.push_back(root["indices"][i].asInt());
          }
        }
      }
      else if (root.isMember("index") && root["index"].isInt())
      {
        indices.push_back(root["index"].asInt());
      }
      else if (root.isMember("choice") && root["choice"].isInt())
      {
        indices.push_back(root["choice"].asInt());
      }
      return indices;
    }
  }

  void DestinationChoiceState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::DESTINATION_CHOICE;
  }

  EngineResult DestinationChoiceState::handleCommand(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (!engine || !engine->stateMachine)
    {
      return buildError(engine, "Destination choice: engine not initialized");
    }
    if (command.type != EngineCommandType::CMD_SELECT_DESTINATION)
    {
      return buildError(engine, "Destination choice: command not allowed");
    }

    Json::Value root;
    std::string error;
    if (!parsePayload(command.payload, root, error))
    {
      return buildError(engine, error);
    }

    std::vector<int> picks = parseIndices(root);
    if (picks.empty())
    {
      return buildError(engine, "Destination choice: missing selection");
    }

    std::shared_ptr<state::State> state = engine->getState();
    if (!state)
    {
      return buildError(engine, "Destination choice: missing state");
    }
    std::shared_ptr<cardsState::CardsState> cardsState(state, &state->cards);
    if (!cardsState || !cardsState->gameDestinationCards)
    {
      return buildError(engine, "Destination choice: deck not ready");
    }

    std::vector<std::shared_ptr<cardsState::DestinationCard>>& offered = engine->context.pendingTickets.offered;
    if (offered.empty())
    {
      return buildError(engine, "Destination choice: no pending tickets");
    }

    int minKeep = engine->context.minKeepTickets;
    if (static_cast<int>(picks.size()) < minKeep)
    {
      return buildError(engine, "Destination choice: must keep at least " + std::to_string(minKeep) + " card(s)");
    }

    std::vector<std::shared_ptr<playersState::Player>> players = state->players.getPlayers();
    int playerIndex = engine->context.currentPlayer;
    if (players.empty() || playerIndex < 0 || playerIndex >= static_cast<int>(players.size()))
    {
      return buildError(engine, "Destination choice: invalid player");
    }
    std::shared_ptr<playersState::Player> player = players[playerIndex];
    if (!player)
    {
      return buildError(engine, "Destination choice: missing player");
    }
    std::shared_ptr<cardsState::PlayerCards> hand = player->getHand();
    if (!hand || !hand->destinationCards)
    {
      return buildError(engine, "Destination choice: missing player hand");
    }

    std::vector<bool> keepFlags(offered.size(), false);
    for (std::size_t i = 0; i < picks.size(); ++i)
    {
      int index = picks[i];
      if (index <= 0 || index > static_cast<int>(offered.size()))
      {
        return buildError(engine, "Destination choice: invalid selection index");
      }
      keepFlags[static_cast<std::size_t>(index - 1)] = true;
    }

    for (std::size_t i = 0; i < offered.size(); ++i)
    {
      std::shared_ptr<cardsState::DestinationCard> card = offered[i];
      if (keepFlags[i])
      {
        hand->destinationCards->cards.push_back(card);
      }
      else
      {
        if (cardsState->gameDestinationCards->faceDownCards)
        {
          cardsState->gameDestinationCards->faceDownCards->addCard(card);
        }
      }
    }

    offered.clear();

    EngineResult result;
    result.ok = true;
    result.error = "";
    result.nextPhase = Phase::CONFIRMATION;
    addInfo(result, "Destination cards selected.");

    std::shared_ptr<GameState> nextState(new ConfirmationState());
    engine->stateMachine->transitionTo(engine, nextState);
    return result;
  }

  std::vector<EngineCommandType> DestinationChoiceState::getAllowedCommands()
  {
    std::vector<EngineCommandType> allowed;
    allowed.push_back(EngineCommandType::CMD_SELECT_DESTINATION);
    allowed.push_back(EngineCommandType::CMD_EXIT);
    return allowed;
  }
}
