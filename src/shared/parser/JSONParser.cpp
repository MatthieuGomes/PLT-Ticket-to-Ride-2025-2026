#include "JSONParser.h"

#include <fstream>
#include <sstream>

namespace parser
{

  bool JSONParser::parseString(const std::string& json, Json::Value& output, std::string& error)
  {
    error.clear();

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::string errors;
    std::istringstream input(json);
    bool ok = Json::parseFromStream(builder, input, &output, &errors);
    if (!ok) {
      error = errors;
    }
    return ok;
  }

  bool JSONParser::parseFile(std::string& path, Json::Value& output, std::string& error)
  {
    error.clear();

    std::ifstream input(path.c_str());
    if (!input.is_open()) {
      error = "Unable to open JSON file";
      return false;
    }

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::string errors;
    bool ok = Json::parseFromStream(builder, input, &output, &errors);
    if (!ok) {
      error = errors;
    }
    return ok;
  }

}
