#include "CommandParser.h"

#include "EngineCommand.h"
#include "EngineEvent.h"
#include "EngineEventType.h"
#include "Phase.h"

#include "parser/CommandMessage.h"
#include "parser/JSONParser.h"

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

  EngineCommand CommandParser::parse(const std::string& json)
  {
    parser::JSONParser parser;
    parser::CommandMessage message;
    std::string error;
    bool ok = parser.parseCommand(json, message, error);
    if (!ok)
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
    parser::JSONParser parser;
    parser::CommandMessage message;
    std::string error;
    bool ok = parser.parseCommand(json, message, error);
    if (!ok)
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
