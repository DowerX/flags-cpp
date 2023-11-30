#include "flags.h"
#include <sstream>

namespace Flags {

std::vector<std::string> split(const std::string &input, const char separator) {
  std::vector<std::string> result;
  std::istringstream stream(input);
  std::string part;
  while (std::getline(stream, part, separator))
    result.push_back(part);

  return result;
}

bool starts_with(const std::string &input, const std::string &start) {
  return (input.length() >= start.length() && input.substr(0, 2) == start);
}

bool Parser::parse(int argc, char **argv) {
  // put each argument into a std::string
  std::vector<std::string> args;
  for (int i = 0; i < argc; i++) {
    std::string arg(argv[i]);
    if (starts_with(arg, prefix) && arg.length() > prefix.length()) {
      std::vector<std::string> parts = split(arg, '=');
      args.insert(args.end(), parts.begin(), parts.end());
    } else {
      args.push_back(arg);
    }
  }

  // search for flags
  for (size_t i = 1; i < args.size() - 1; i++) {
    const std::string &arg = args[i];
    if (starts_with(arg, prefix) && arg.length() > prefix.length()) {
      flags[arg.substr(prefix.length())]->parse(args[i + 1]);
    }
  }

  // check for the last argument
  if (starts_with(args[args.size() - 1], prefix) &&
      args[args.size() - 1].length() > prefix.length()) {
    flags[args[args.size() - 1].substr(prefix.length())]->parse("");
  }

  return get_found() == flags.size();
}

const unsigned Parser::get_parsed() const {
  return std::count_if(
      flags.begin(), flags.end(),
      [](std::pair<std::string, Flag *> f) { return f.second->get_parsed(); });
}

const unsigned Parser::get_found() const {
  return std::count_if(
      flags.begin(), flags.end(),
      [](std::pair<std::string, Flag *> f) { return f.second->get_found(); });
}

void Parser::help() const {
  std::cout << help_text << std::endl;

  for (auto f : flags) {
    std::cout << '\t' << prefix << f.first << ": "
              << f.second->get_description() << std::endl;
  }
}
} // namespace Flags