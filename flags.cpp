#include "flags.h"
#include <sstream>
#include <iostream>

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
  return (input.size() >= start.size() && input.substr(0, 2) == start);
}

PARSER(StringFlag, std::string, {
  set_found(true);
  set_parsed(true);
  value = arg;
})

PARSER(IntFlag, int, {
  set_found(true);
  try {
    value = std::stoi(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(LongIntFlag, long int, {
  set_found(true);
  try {
    value = std::stol(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(LongLongIntFlag, long long int, {
  set_found(true);
  try {
    value = std::stoll(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(UnsignedLongIntFlag, unsigned long int, {
  set_found(true);
  try {
    value = std::stoul(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(UnsignedLongLongIntFlag, unsigned long long int, {
  set_found(true);
  try {
    value = std::stoull(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(FloatFlag, float, {
  set_found(true);
  try {
    value = std::stof(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(DoubleFlag, double, {
  set_found(true);
  try {
    value = stod(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(LongDoubleFlag, long double, {
  set_found(true);
  try {
    value = stold(arg);
    set_parsed(true);
  } catch (std::exception &) {
  }
})

PARSER(BoolFlag, bool, {
  std::string copy(arg);
  std::transform(copy.begin(), copy.end(), copy.begin(),
                 [](const char c) { return std::tolower(c); });
  if (copy == "false")
    value = false;
  else
    value = true;
})

Parser::Parser(const std::string &prefix, const std::string &help_text)
    : prefix(prefix), help_text(help_text) {
  set_parser<std::string>(flag_constructor_t(StringFlag::make));
  set_parser<int>(flag_constructor_t(IntFlag::make));
  set_parser<long int>(flag_constructor_t(LongIntFlag::make));
  set_parser<long long int>(flag_constructor_t(LongLongIntFlag::make));
  set_parser<unsigned long int>(flag_constructor_t(UnsignedLongIntFlag::make));
  set_parser<unsigned long long int>(
      flag_constructor_t(UnsignedLongLongIntFlag::make));
  set_parser<float>(flag_constructor_t(FloatFlag::make));
  set_parser<double>(flag_constructor_t(DoubleFlag::make));
  set_parser<long double>(flag_constructor_t(LongDoubleFlag::make));
  set_parser<bool>(flag_constructor_t(BoolFlag::make));
}

bool Parser::parse(int argc, char **argv) {
  // put each argument into a std::string
  std::vector<std::string> args;
  for (int i = 0; i < argc; i++) {
    std::string arg(argv[i]);
    if (starts_with(arg, prefix) && arg.size() > prefix.size()) {
      std::vector<std::string> parts = split(arg, '=');
      args.insert(args.end(), parts.begin(), parts.end());
    } else {
      args.push_back(arg);
    }
  }

  // search for flags
  for (size_t i = 1; i < args.size() - 1; i++) {
    const std::string &arg = args[i];
    if (starts_with(arg, prefix) && arg.size() > prefix.size()) {
      flags[arg.substr(prefix.size())]->parse(args[i + 1]);
    }
  }

  // check for the last argument
  if (starts_with(args[args.size() - 1], prefix) &&
      args[args.size() - 1].size() > prefix.size()) {
    flags[args[args.size() - 1].substr(prefix.size())]->parse("");
  }

  return get_found() == flags.size();
}

unsigned Parser::get_parsed() const {
  return std::count_if(
      flags.begin(), flags.end(),
      [](std::pair<std::string, Flag *> f) { return f.second->get_parsed(); });
}

unsigned Parser::get_found() const {
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