#ifndef FLAGS_H
#define FLAGS_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <typeindex>

namespace Flags {

// utility functions
std::vector<std::string> split(const std::string &input, const char separator);
bool starts_with(const std::string &input, const std::string &start);

// flag base class
// should be abstract, don't use
class Flag {
  std::string description;
  bool found;
  bool parsed;

protected:
  void set_found(const bool found) { this->found = found; }
  void set_parsed(const bool parsed) { this->parsed = parsed; }

public:
  virtual const void *get_value_ptr() const = 0;

  Flag(const std::string &description, const bool required)
      : description(description), found(!required), parsed(false) {}

  virtual ~Flag() {};

  const std::string &get_description() const { return description; }
  const bool get_found() const { return found; }
  const bool get_parsed() const { return parsed; }

  virtual void parse(const std::string &arg) {}
};

// type of flag based parser classes' factory functions
typedef std::function<Flag *(const void *, const std::string &, const bool)>
    flag_constructor_t;

class StringFlag : public Flag {
  std::string value;

public:
  StringFlag(const void *default_value, const std::string &description,
             const bool required)
      : Flag(description, required), value(*(std::string *)default_value) {}

  static Flag *make(const void *default_value, const std::string &description,
                    const bool required) {
    return new StringFlag(default_value, description, required);
  }

  const void *get_value_ptr() const override { return &value; };

  void parse(const std::string &arg) override {
    set_found(true);
    set_parsed(true);
    value = arg;
  }
};

// flag holder, parsing orchestrator
class Parser {
private:
  const std::string prefix;
  const std::string help_text;

  std::map<std::string, Flag *> flags;
  std::map<std::type_index, flag_constructor_t> constructors;

public:
  Parser(const std::string &prefix, const std::string &help_text)
      : prefix(prefix), help_text(help_text) {
    set_parser<std::string>(flag_constructor_t(StringFlag::make));
  }

  Parser() : Parser("--", "Help:") {}

  ~Parser() {
    for (auto f : flags) {
      delete f.second;
    }
  }

  template <typename T> void set_parser(flag_constructor_t constructor_function) {
    constructors[typeid(T)] = constructor_function;
  }

  template <typename T>
  T *add(const std::string &name, const std::string &description,
         const bool required, T default_value) {
    flags[name] =
        constructors[typeid(T)]((void *)&default_value, description, required);
    return (T *)flags[name]->get_value_ptr();
  }

  const unsigned get_found() const;

  const unsigned get_parsed() const;

  bool parse(int argc, char **argv);

  void help() const;
};
} // namespace Flags
#endif