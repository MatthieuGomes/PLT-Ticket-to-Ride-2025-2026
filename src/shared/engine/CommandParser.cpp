#include "CommandParser.h"

#include <cctype>
#include <string>

namespace engine {

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

Command CommandParser::makeEmptyCommand() {
  Command cmd;
  cmd.type = CommandType::INVALID;
  cmd.arg1 = "";
  cmd.arg2 = "";
  cmd.argInt = 0;
  return cmd;
}

ParseResult CommandParser::parse(const std::string& input) {
  ParseResult result;
  result.ok = false;
  result.command = makeEmptyCommand();
  result.error = "";

  const std::string trimmed = trim(input);
  if (trimmed.empty()) {
    return result;
  }

  const std::string lowered = toLower(trimmed);
  if (lowered == "exit" || lowered == "quit") {
    result.ok = true;
    result.command.type = CommandType::EXIT;
    return result;
  }
  if (lowered == "help") {
    result.ok = true;
    result.command.type = CommandType::HELP;
    return result;
  }

  result.error = "Unknown command: " + trimmed;
  return result;
}

}  // namespace engine
