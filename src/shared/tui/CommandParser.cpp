#include "CommandParser.h"

#include <cctype>
#include <string>
#include <vector>

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
  std::string lowered = toLower(trimmed);

  std::vector<std::string> tokens;
  std::string current;
  for (std::size_t i = 0; i < lowered.size(); ++i) {
    char ch = lowered[i];
    if (std::isspace(static_cast<unsigned char>(ch)) != 0) {
      if (!current.empty()) {
        tokens.push_back(current);
        current.clear();
      }
    } else {
      current.push_back(ch);
    }
  }
  if (!current.empty()) {
    tokens.push_back(current);
  }

  if (!tokens.empty() && tokens[0] == "draw") {
    if (tokens.size() >= 2 && (tokens[1] == "dest" || tokens[1] == "destination" || tokens[1] == "destcard")) {
      action = "draw_destination";
    } else if (tokens.size() >= 3 && tokens[1] == "card") {
      if (tokens[2] == "faceup") {
        action = "draw_faceup";
      } else if (tokens[2] == "facedown") {
        action = "draw_facedown";
      }
    }
  }
  if (!tokens.empty() && tokens[0] == "take") {
    action = "take";
  }
  if (!tokens.empty() && (tokens[0] == "select" || tokens[0] == "choose")) {
    action = "select_destination";
  }
  if (!tokens.empty() && tokens[0] == "build" && tokens.size() >= 2 && tokens[1] == "station") {
    action = "claim_station";
  }
  if (!tokens.empty() && (tokens[0] == "color" || tokens[0] == "colour")) {
    action = "color";
  }

  if (action.empty()) {
    if (tokens.empty()) {
      return result;
    }
    action = tokens[0];
  }

  if (action == "quit") {
    action = "exit";
  }

  if (tokens.size() > 1) {
    std::string rebuilt;
    for (std::size_t i = 1; i < tokens.size(); ++i) {
      if (i > 1) {
        rebuilt.push_back(' ');
      }
      rebuilt += tokens[i];
    }
    args = rebuilt;
  }

  parser::CommandMessage message;
  message.kind = "command";
  message.origin = "tui";
  message.version = 1;
  message.name = action;
  message.payload = Json::Value(Json::objectValue);
  if (!args.empty()) {
    message.payload["args"] = args;
  }
  if (action == "draw_faceup" && tokens.size() >= 4) {
    const std::string& indexToken = tokens[3];
    int index = 0;
    try {
      index = std::stoi(indexToken);
    } catch (...) {
      index = 0;
    }
    if (index > 0) {
      message.payload["index"] = index;
    }
  }
  if (action == "take" && tokens.size() >= 2) {
    int roadId = 0;
    try {
      roadId = std::stoi(tokens[1]);
    } catch (...) {
      roadId = 0;
    }
    if (roadId > 0) {
      message.payload["roadId"] = roadId;
    }
    if (tokens.size() >= 3) {
      message.payload["color"] = tokens[2];
    }
  }
  if (action == "color" && tokens.size() >= 2) {
    message.payload["color"] = tokens[1];
  }
  if (action == "select_destination" && tokens.size() >= 2) {
    Json::Value indices(Json::arrayValue);
    for (std::size_t i = 1; i < tokens.size(); ++i) {
      int index = 0;
      try {
        index = std::stoi(tokens[i]);
      } catch (...) {
        index = 0;
      }
      if (index > 0) {
        indices.append(index);
      }
    }
    if (!indices.empty()) {
      message.payload["indices"] = indices;
    }
  }
  if (action == "claim_station" && tokens.size() >= 3) {
    message.payload["station"] = tokens[2];
    if (tokens.size() >= 4) {
      message.payload["color"] = tokens[3];
    }
  }

  result.ok = true;
  result.json = jsonparser.serializeCommand(message);
  return result;
}

}  // namespace tui
