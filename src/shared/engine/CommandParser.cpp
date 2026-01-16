#include "CommandParser.h"

#include "EngineCommand.h"
#include "EngineEvent.h"
#include "EngineEventType.h"
#include "Phase.h"

#include "parser/CommandMessage.h"
#include "parser/EventMessage.h"
#include "parser/ResultMessage.h"

#include <json/json.h>

namespace engine
{
  namespace
  {
    const int kDefaultVersion = 1;

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
