#include "JSONParser.h"

#include <json/json.h>

#include <sstream>

namespace parser
{

  namespace {

    const int kDefaultVersion = 1;
    const int kUnsetId = -1;
    const char kKindCommand[] = "command";
    const char kKindResult[] = "result";
    const char kKindEvent[] = "event";

    bool parseRoot(const std::string& jsonText, Json::Value& root, std::string& error) {
      error.clear();

      Json::CharReaderBuilder builder;
      builder["collectComments"] = false;
      std::string errors;
      std::istringstream input(jsonText);
      bool ok = Json::parseFromStream(builder, input, &root, &errors);
      if (!ok) {
        error = errors;
      }
      return ok;
    }

    std::string writeJson(const Json::Value& root) {
      Json::StreamWriterBuilder builder;
      builder["commentStyle"] = "None";
      builder["indentation"] = "";
      return Json::writeString(builder, root);
    }

    int readOptionalInt(const Json::Value& root, const char* key, int fallback) {
      if (root.isMember(key) && root[key].isInt()) {
        return root[key].asInt();
      }
      return fallback;
    }

    std::string readOptionalString(const Json::Value& root, const char* key) {
      if (root.isMember(key) && root[key].isString()) {
        return root[key].asString();
      }
      return std::string();
    }

    void readBaseFields(const Json::Value& root, MessageBase& out) {
      out.kind = readOptionalString(root, "kind");
      if (out.kind.empty()) {
        out.kind = readOptionalString(root, "type");
      }
      out.origin = readOptionalString(root, "origin");
      if (out.origin.empty()) {
        out.origin = readOptionalString(root, "source");
      }
      out.version = readOptionalInt(root, "version", kDefaultVersion);
      out.requestID = readOptionalString(root, "requestId");
      if (out.requestID.empty()) {
        out.requestID = readOptionalString(root, "requestID");
      }
      out.playerID = readOptionalInt(root, "playerId", kUnsetId);
      if (out.playerID == kUnsetId) {
        out.playerID = readOptionalInt(root, "playerID", kUnsetId);
      }
      out.turn = readOptionalInt(root, "turn", kUnsetId);
    }

    void writeBaseFields(Json::Value& root, const MessageBase& in, const char* defaultKind) {
      std::string kind = in.kind.empty() ? defaultKind : in.kind;
      root["kind"] = kind;
      if (!in.origin.empty()) {
        root["origin"] = in.origin;
      }
      int version = in.version <= 0 ? kDefaultVersion : in.version;
      root["version"] = version;
      if (!in.requestID.empty()) {
        root["requestId"] = in.requestID;
      }
      if (in.playerID != kUnsetId) {
        root["playerId"] = in.playerID;
      }
      if (in.turn != kUnsetId) {
        root["turn"] = in.turn;
      }
    }

  }  // namespace

  bool JSONParser::parseCommand(const std::string& json, CommandMessage& out, std::string& error)
  {
    Json::Value root;
    if (!parseRoot(json, root, error)) {
      return false;
    }
    if (!root.isObject()) {
      error = "Command JSON is not an object";
      return false;
    }

    readBaseFields(root, out);
    if (out.kind.empty()) {
      out.kind = kKindCommand;
    }

    std::string name = readOptionalString(root, "name");
    if (name.empty()) {
      name = readOptionalString(root, "action");
    }
    if (name.empty()) {
      error = "Command missing name";
      return false;
    }

    out.name = name;
    out.payload = root.isMember("payload") ? root["payload"] : Json::Value(Json::objectValue);
    return true;
  }

  bool JSONParser::parseResult(const std::string& json, ResultMessage& out, std::string& error)
  {
    Json::Value root;
    if (!parseRoot(json, root, error)) {
      return false;
    }
    if (!root.isObject()) {
      error = "Result JSON is not an object";
      return false;
    }

    readBaseFields(root, out);
    if (out.kind.empty()) {
      out.kind = kKindResult;
    }
    out.ok = root.isMember("ok") ? root["ok"].asBool() : false;
    out.error = readOptionalString(root, "error");
    out.payload = root.isMember("payload") ? root["payload"] : Json::Value(Json::objectValue);
    out.events.clear();

    if (root.isMember("events") && root["events"].isArray()) {
      const Json::Value& events = root["events"];
      for (Json::ArrayIndex i = 0; i < events.size(); ++i) {
        if (!events[i].isObject()) {
          continue;
        }
        EventMessage event;
        readBaseFields(events[i], event);
        if (event.kind.empty()) {
          event.kind = kKindEvent;
        }
        if (event.origin.empty()) {
          event.origin = out.origin;
        }
        if (event.version <= 0) {
          event.version = out.version;
        }
        std::string type = readOptionalString(events[i], "type");
        if (type.empty()) {
          type = readOptionalString(events[i], "eventType");
        }
        event.type = type;
        event.message = readOptionalString(events[i], "message");
        event.payload = events[i].isMember("payload")
            ? events[i]["payload"]
            : Json::Value(Json::objectValue);
        out.events.push_back(event);
      }
    }

    return true;
  }

  std::string JSONParser::serializeCommand(const CommandMessage& in)
  {
    Json::Value root(Json::objectValue);
    writeBaseFields(root, in, kKindCommand);
    root["name"] = in.name;
    root["payload"] = in.payload;
    return writeJson(root);
  }

  std::string JSONParser::serializeResult(const ResultMessage& in)
  {
    Json::Value root(Json::objectValue);
    writeBaseFields(root, in, kKindResult);
    root["ok"] = in.ok;
    root["error"] = in.error;
    root["payload"] = in.payload;

    Json::Value events(Json::arrayValue);
    for (std::size_t i = 0; i < in.events.size(); ++i) {
      Json::Value event(Json::objectValue);
      writeBaseFields(event, in.events[i], kKindEvent);
      event["type"] = in.events[i].type;
      event["message"] = in.events[i].message;
      event["payload"] = in.events[i].payload;
      events.append(event);
    }
    root["events"] = events;
    return writeJson(root);
  }

  std::string JSONParser::serializeEvent(const EventMessage& in)
  {
    Json::Value root(Json::objectValue);
    writeBaseFields(root, in, kKindEvent);
    root["type"] = in.type;
    root["message"] = in.message;
    root["payload"] = in.payload;
    return writeJson(root);
  }

}
