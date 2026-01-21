#include "CommandParser.h"

#include "EngineCommand.h"
#include "EngineEvent.h"
#include "EngineEventType.h"
#include "Phase.h"

#include "parser/CommandMessage.h"
#include "parser/EventMessage.h"
#include "parser/ResultMessage.h"

#include <cctype>
#include <json/json.h>

namespace engine
{
  namespace
  {
    const int kDefaultVersion = 1;

    std::string toLower(const std::string& value)
    {
      std::string lowered = value;
      for (std::size_t i = 0; i < lowered.size(); ++i)
      {
        lowered[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowered[i])));
      }
      return lowered;
    }

    EngineCommandType mapCommandType(const std::string& name)
    {
      const std::string lowered = toLower(name);
      if (lowered == "exit" || lowered == "quit")
      {
        return EngineCommandType::CMD_EXIT;
      }
      if (lowered == "help")
      {
        return EngineCommandType::CMD_HELP;
      }
      if (lowered == "take" || lowered == "take_road")
      {
        return EngineCommandType::CMD_TAKE_ROAD;
      }
      if (lowered == "draw_destination")
      {
        return EngineCommandType::CMD_DRAW_DESTINATION;
      }
      if (lowered == "draw_faceup")
      {
        return EngineCommandType::CMD_DRAW_WAGON_FACEUP;
      }
      if (lowered == "draw_facedown")
      {
        return EngineCommandType::CMD_DRAW_WAGON_FACEDOWN;
      }
      if (lowered == "select_destination")
      {
        return EngineCommandType::CMD_SELECT_DESTINATION;
      }
      if (lowered == "claim_station")
      {
        return EngineCommandType::CMD_CLAIM_STATION;
      }
      if (lowered == "borrow")
      {
        return EngineCommandType::CMD_BORROW_ROAD;
      }
      if (lowered == "confirm")
      {
        return EngineCommandType::CMD_CONFIRM_ENDTURN;
      }
      return EngineCommandType::CMD_UNKNOWN;
    }

    std::string serializePayload(const Json::Value& payload)
    {
      Json::StreamWriterBuilder builder;
      builder["commentStyle"] = "None";
      builder["indentation"] = "";
      return Json::writeString(builder, payload);
    }

    EngineCommand toEngineCommand(const parser::CommandMessage& message)
    {
      EngineCommand command;
      command.name = message.name;
      command.type = mapCommandType(message.name);
      command.payload = serializePayload(message.payload);
      return command;
    }

    std::string eventTypeToString(EngineEventType type)
    {
      switch (type)
      {
        case EngineEventType::INFO:
          return "INFO";
        case EngineEventType::STATE_CHANGE:
          return "STATE_CHANGE";
        case EngineEventType::PROMPT:
          return "PROMPT";
        case EngineEventType::ERROR:
          return "ERROR";
      }
      return "UNKNOWN";
    }

    parser::ResultMessage toResultMessage(const EngineResult& result)
    {
      parser::ResultMessage message;
      message.kind = "result";
      message.origin = "engine";
      message.version = kDefaultVersion;
      message.ok = result.ok;
      message.error = result.error;
      message.payload = Json::Value(Json::objectValue);
      message.payload["nextPhase"] = static_cast<int>(result.nextPhase);
      message.payload["exit"] = (result.nextPhase == Phase::EXIT);
      message.events.clear();

      for (std::size_t i = 0; i < result.events.size(); ++i)
      {
        const EngineEvent& engineEvent = result.events[i];
        parser::EventMessage eventMessage;
        eventMessage.kind = "event";
        eventMessage.origin = "engine";
        eventMessage.version = kDefaultVersion;
        eventMessage.type = eventTypeToString(engineEvent.type);
        eventMessage.message = engineEvent.message;
        if (!engineEvent.payload.empty())
        {
          eventMessage.payload = Json::Value(engineEvent.payload);
        }
        else
        {
          eventMessage.payload = Json::Value(Json::objectValue);
        }
        message.events.push_back(eventMessage);
      }

      return message;
    }

    EngineResult buildErrorResult(const std::string& error, Phase phase)
    {
      EngineResult result;
      result.ok = false;
      result.error = error;
      result.nextPhase = phase;

      EngineEvent event;
      event.type = EngineEventType::ERROR;
      event.message = error;
      event.payload = "";
      result.events.push_back(event);

      return result;
    }
  }

  CommandParser::CommandParser()
  {
  }

  parser::CommandMessage CommandParser::decode(const std::string& json, std::string& error)
  {
    parser::CommandMessage message;
    bool ok = jsonParser.parseCommand(json, message, error);
    if (!ok)
    {
      return parser::CommandMessage();
    }
    if (message.version <= 0)
    {
      message.version = kDefaultVersion;
    }
    return message;
  }

  std::string CommandParser::encode(const parser::ResultMessage& result)
  {
    return jsonParser.serializeResult(result);
  }

  std::string CommandParser::encodeResult(const EngineResult& result)
  {
    parser::ResultMessage message = toResultMessage(result);
    return encode(message);
  }

  EngineCommand CommandParser::parse(const std::string& json)
  {
    std::string error;
    parser::CommandMessage message = decode(json, error);
    if (!error.empty())
    {
      EngineCommand invalid;
      invalid.name = "";
      invalid.payload = error;
      return invalid;
    }
    if (message.version <= 0)
    {
      message.version = kDefaultVersion;
    }
    return toEngineCommand(message);
  }

  EngineResult CommandParser::apply(std::shared_ptr<Engine> engine, const EngineCommand& command)
  {
    if (command.type == EngineCommandType::CMD_EXIT || command.name == "exit")
    {
      EngineResult result;
      result.ok = true;
      result.error = "";
      result.nextPhase = Phase::EXIT;

      EngineEvent event;
      event.type = EngineEventType::INFO;
      event.message = "Exit requested";
      event.payload = "";
      result.events.push_back(event);

      if (engine)
      {
        engine->phase = Phase::EXIT;
        engine->stateMachine.reset();
        engine->state.reset();
      }
      return result;
    }

    if (!engine)
    {
      return buildErrorResult("No engine instance", Phase::SETUP);
    }
    if (!engine->stateMachine)
    {
      return buildErrorResult("No state machine", engine->phase);
    }
    return engine->stateMachine->handleCommand(engine, command);
  }

  EngineResult CommandParser::parseAndApply(std::shared_ptr<Engine> engine, const std::string& json)
  {
    std::string error;
    parser::CommandMessage message = decode(json, error);
    if (!error.empty())
    {
      Phase phase = Phase::SETUP;
      if (engine)
      {
        phase = engine->phase;
      }
      return buildErrorResult(error, phase);
    }
    if (message.version <= 0)
    {
      message.version = kDefaultVersion;
    }
    EngineCommand command = toEngineCommand(message);
    return apply(engine, command);
  }
}
