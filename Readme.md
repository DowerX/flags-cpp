# Flags for C++
This project aims to recreate the functionality of the `flag` package from Golang.

## Usage
### Create a parser
```c++
#include "flags.h"

Parser parser;
or
// set the prefix and the help text
Parser parser("--", "Help:");
```

### Declare a flag
```c++
// name, description, required, default value
std::string* x = parser.add<std::string>("myStr", "some description", true, "default value");
```

### Parse arguments and show help text
```c++
if (!parser.parse(argc, argv))
  parser.help();
```

### Declare a custom parser class
```c++
PARSER(MyTypeFlag, MyType, {
  // parser body
  // available variables:
  //   MyType value;      <- storage for your value
  //   std::string arg;   <- the next argument after the current flag
  set_found(true);
  try {
    value = my_parser_function(arg);
    set_found(true);
  } catch(std::exception&) {}
})

parser.set_parser<MyType>(flag_constructor_t(MyTypeFlag::make));
```

## Built-in parsers
- std::string
- int (long, long long, unsigned long, unsigned long long)
- float, double, long double
- bool
