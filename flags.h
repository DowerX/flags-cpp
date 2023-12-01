#ifndef FLAGS_H
#define FLAGS_H

#include <functional>
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

  virtual ~Flag(){};

  const std::string &get_description() const { return description; }
  bool get_found() const { return found; }
  bool get_parsed() const { return parsed; }

  virtual void parse(const std::string &arg) = 0;
};

// type of flag based parser classes' factory functions
typedef std::function<Flag *(const void *, const std::string &, const bool)>
    flag_constructor_t;

// use this class to create your own parser
template <typename T> class FlagTemplate : public Flag {
protected:
  T value;

public:
  explicit FlagTemplate<T>(const void *default_value,
                           const std::string &description, const bool required)
      : Flag(description, required), value(*(T *)default_value) {}

  const void *get_value_ptr() const override { return &value; };
};

#define PARSER(name, type, func)                                               \
  class name : public FlagTemplate<type> {                                     \
    using FlagTemplate<type>::FlagTemplate;                                    \
                                                                               \
  public:                                                                      \
    static Flag *make(const void *default_value,                               \
                      const std::string &description, const bool required) {   \
      return new name(default_value, description, required);                   \
    }                                                                          \
    void parse(const std::string &arg) override func                           \
  }; // namespace Flags

// flag holder, parsing orchestrator
class Parser {
private:
  const std::string prefix;
  const std::string help_text;

  std::map<std::string, Flag *> flags;
  std::map<std::type_index, flag_constructor_t> constructors;

public:
  Parser(const std::string &prefix, const std::string &help_text);

  Parser() : Parser("--", "Help:") {}

  ~Parser() {
    for (auto f : flags) {
      delete f.second;
    }
  }

  template <typename T>
  void set_parser(flag_constructor_t constructor_function) {
    constructors[typeid(T)] = constructor_function;
  }

  template <typename T>
  T *add(const std::string &name, const std::string &description,
         const bool required, T default_value) {
    flags[name] =
        constructors[typeid(T)]((void *)&default_value, description, required);
    return (T *)flags[name]->get_value_ptr();
  }

  unsigned get_found() const;

  unsigned get_parsed() const;

  bool parse(int argc, char **argv);

  void help() const;
};
} // namespace Flags
#endif