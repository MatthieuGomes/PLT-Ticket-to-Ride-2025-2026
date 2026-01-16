#include "CommandParser.h"

#include <cctype>
#include <string>

#include <json/json.h>

namespace tui {

std::string CommandParser::trim(const std::string& input) {
  std::size_t start = 0;
  while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start])) != 0) {
    ++start;
  }
  if (start == input.size()) {
    return std::string();
  }
  std::size_t end = input.size() - 1;
  while (end > start && std::isspace(static_cast<unsigned char>(input[end])) != 0) {
    --end;
  }
  return input.substr(start, end - start + 1);
}

std::string CommandParser::toLower(const std::string& input) {
  std::string lowered = input;
  for (std::size_t i = 0; i < lowered.size(); ++i) {
    lowered[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowered[i])));
  }
  return lowered;
}

std::string CommandParser::escapeJson(const std::string& input) {
  std::string escaped;
  escaped.reserve(input.size());
  for (std::size_t i = 0; i < input.size(); ++i) {
    const char ch = input[i];
    if (ch == '\\') {
      escaped += "\\\\";
    } else if (ch == '"') {
      escaped += "\\\"";
    } else if (ch == '\n') {
      escaped += "\\n";
    } else if (ch == '\r') {
      escaped += "\\r";
    } else if (ch == '\t') {
      escaped += "\\t";
    } else {
      escaped += ch;
    }
  }
  return escaped;
}

ParseResult CommandParser::parse(const std::string& input) {
  ParseResult result;
  result.ok = false;
  result.json = "";
  result.error = "";

  const std::string trimmed = trim(input);
  if (trimmed.empty()) {
    return result;
  }

  std::string action;
  std::string args;

  const std::size_t space = trimmed.find(' ');
  if (space == std::string::npos) {
    action = toLower(trimmed);
  } else {
    action = toLower(trimmed.substr(0, space));
    args = trim(trimmed.substr(space + 1));
  }

  if (action == "quit") {
    action = "exit";
  }

  parser::CommandMessage command;
  command.kind = "command";
  command.origin = "tui";
  command.version = 1;
  command.requestID = requestID;
  command.playerID = playerID;
  command.turn = turn;
  command.name = action;
  command.payload = Json::Value(Json::objectValue);
  if (!args.empty()) {
    command.payload["args"] = args;
  }

  result.ok = true;
  result.json = jsonParser.serializeCommand(command);
  return result;
}

void CommandParser::setContext(int newPlayerID, int newTurn, const std::string& newRequestID) {
  playerID = newPlayerID;
  turn = newTurn;
  requestID = newRequestID;
}

}  // namespace tui
