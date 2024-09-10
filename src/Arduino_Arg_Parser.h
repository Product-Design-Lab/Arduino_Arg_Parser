#ifndef ARDUINO_ARG_PARSER_H
#define ARDUINO_ARG_PARSER_H

#include "Arduino.h"

#define MAX_ARGUMENTS 20  // Max 20 arguments including options and non-options

class Arduino_Arg_Parser {
  public:
    // Struct to store parsed argument data (key-value pair for options or non-option argument)
    struct Arg {
        String key;     // Key for options or index for non-options
        String value;   // Value for options or non-option argument

        bool isEmpty() const {
            return key.length() == 0 && value.length() == 0;  // Use length() instead of isEmpty()
        }
    };

    // Struct to store the parsed command and its arguments
    struct ParsedCommand {
        String command;
        Arg arguments[MAX_ARGUMENTS];
        size_t numArgs;

        // Check if the parsed command is valid
        bool isValid() const {
            return command.length() > 0;  // Use length() to check if command is valid
        }
    };

    Arduino_Arg_Parser();

    int parse(const char *inputString, size_t length = 0);  // Parse input (const char*)
    int parse(const String &inputString);                   // Parse input (String)

    String cmdName() const;  // Get command name

    const Arg& nextArg();    // Get next argument

    const ParsedCommand& getParsedCommand() const;  // Get non-modifiable reference to parsed command

    void reset();            // Reset parser state

    const Arg& findArgByKey(const String &key) const;  // Find argument by key (non-modifiable)

  private:
    ParsedCommand parsedCommand;
    size_t currentArgIndex;
    static const Arg emptyArg;  // Static const empty Arg

    void parseOption(const String &option, const String &value);
};

#endif
