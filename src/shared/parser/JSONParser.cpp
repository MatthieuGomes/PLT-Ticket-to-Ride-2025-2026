#include "JSONParser.h"

#include <json/json.h>

#include <sstream>

namespace parser
{

  namespace {

    const int kDefaultVersion = 1;
    const int kUnsetId = -1;

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

    std::string name = readOptionalString(root, "name");
    if (name.empty()) {
      error = "Command missing name";
      return false;
    }

    out.name = name;
    out.payload = root.isMember("payload") ? root["payload"] : Json::Value(Json::objectValue);
    out.playerID = readOptionalInt(root, "playerId", kUnsetId);
    if (out.playerID == kUnsetId) {
      out.playerID = readOptionalInt(root, "playerID", kUnsetId);
    }
    out.turn = readOptionalInt(root, "turn", kUnsetId);
    out.requestID = readOptionalString(root, "requestId");
    if (out.requestID.empty()) {
      out.requestID = readOptionalString(root, "requestID");
    }
    out.version = readOptionalInt(root, "version", kDefaultVersion);
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

    out.ok = root.isMember("ok") ? root["ok"].asBool() : false;
    out.error = readOptionalString(root, "error");
    out.requestID = readOptionalString(root, "requestId");
    if (out.requestID.empty()) {
      out.requestID = readOptionalString(root, "requestID");
    }
    out.payload = root.isMember("payload") ? root["payload"] : Json::Value(Json::objectValue);
    out.version = readOptionalInt(root, "version", kDefaultVersion);
    out.events.clear();

    if (root.isMember("events") && root["events"].isArray()) {
      const Json::Value& events = root["events"];
      for (Json::ArrayIndex i = 0; i < events.size(); ++i) {
        if (!events[i].isObject()) {
          continue;
        }
        EventMessage event;
        std::string type = readOptionalString(events[i], "type");
        if (type.empty()) {
          type = readOptionalString(events[i], "eventType");
        }
        event.type = type;
        event.message = readOptionalString(events[i], "message");
        event.payload = events[i].isMember("payload")
            ? events[i]["payload"]
            : Json::Value(Json::objectValue);
        event.version = readOptionalInt(events[i], "version", out.version);
        out.events.push_back(event);
      }
    }

    return true;
  }

  std::string JSONParser::serializeCommand(const CommandMessage& in)
  {
    Json::Value root(Json::objectValue);
    root["type"] = "command";
    root["version"] = in.version;
    root["name"] = in.name;
    root["payload"] = in.payload;
    if (in.playerID != kUnsetId) {
      root["playerId"] = in.playerID;
    }
    if (in.turn != kUnsetId) {
      root["turn"] = in.turn;
    }
    if (!in.requestID.empty()) {
      root["requestId"] = in.requestID;
    }
    return writeJson(root);
  }

  std::string JSONParser::serializeResult(const ResultMessage& in)
  {
    Json::Value root(Json::objectValue);
    root["type"] = "result";
    root["version"] = in.version;
    root["ok"] = in.ok;
    root["error"] = in.error;
    root["requestId"] = in.requestID;
    root["payload"] = in.payload;

    Json::Value events(Json::arrayValue);
    for (std::size_t i = 0; i < in.events.size(); ++i) {
      Json::Value event(Json::objectValue);
      event["type"] = in.events[i].type;
      event["message"] = in.events[i].message;
      event["payload"] = in.events[i].payload;
      event["version"] = in.events[i].version;
      events.append(event);
    }
    root["events"] = events;
    return writeJson(root);
  }

  std::string JSONParser::serializeEvent(const EventMessage& in)
  {
    Json::Value root(Json::objectValue);
    root["type"] = in.type;
    root["message"] = in.message;
    root["payload"] = in.payload;
    root["version"] = in.version;
    return writeJson(root);
  }

}
