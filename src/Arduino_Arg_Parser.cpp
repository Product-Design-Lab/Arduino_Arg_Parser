#include "Arduino_Arg_Parser.h"

// Initialize the static const emptyArg
const Arduino_Arg_Parser::Arg Arduino_Arg_Parser::emptyArg = {"", ""};

Arduino_Arg_Parser::Arduino_Arg_Parser() : currentArgIndex(0) {
    reset();
}

int Arduino_Arg_Parser::parse(const char *inputString, size_t length) {
    reset();

    String input;
    if (length == 0) {
        input = String(inputString);  // Null-terminated, no length provided
    } else {
        input = String(inputString).substring(0, length);  // Handle non-null-terminated input
    }

    return parse(input);  // Use the overloaded String version
}

int Arduino_Arg_Parser::parse(const String &inputString) {
    reset();

    int spaceIndex = inputString.indexOf(' ');
    if (spaceIndex == -1) {
        parsedCommand.command = inputString;  // If no arguments, set the whole input as the command
        return 0;
    }

    parsedCommand.command = inputString.substring(0, spaceIndex);  // Extract command
    String remaining = inputString.substring(spaceIndex + 1);

    while (remaining.length() > 0) {
        int spacePos = remaining.indexOf(' ');
        String token;
        if (spacePos == -1) {
            token = remaining;  // Last argument
            remaining = "";
        } else {
            token = remaining.substring(0, spacePos);
            remaining = remaining.substring(spacePos + 1);
        }

        if (token.startsWith("--")) {
            int equalPos = token.indexOf('=');
            if (equalPos != -1) {
                // Long option with value: --key=value
                String key = token.substring(2, equalPos);
                String value = token.substring(equalPos + 1);
                parseOption(key, value);
            } else {
                // Long option without value: --key, value comes later
                String key = token.substring(2);
                if (remaining.length() > 0) {
                    spacePos = remaining.indexOf(' ');
                    if (spacePos == -1) {
                        parseOption(key, remaining);  // Use remaining as the value
                        remaining = "";
                    } else {
                        String value = remaining.substring(0, spacePos);
                        parseOption(key, value);
                        remaining = remaining.substring(spacePos + 1);
                    }
                }
            }
        } else if (token.startsWith("-")) {
            // Short option: -key value
            String key = token.substring(1);
            if (remaining.length() > 0) {
                spacePos = remaining.indexOf(' ');
                if (spacePos == -1) {
                    parseOption(key, remaining);  // Use remaining as the value
                    remaining = "";
                } else {
                    String value = remaining.substring(0, spacePos);
                    parseOption(key, value);
                    remaining = remaining.substring(spacePos + 1);
                }
            }
        } else {
            // Non-option argument
            if (parsedCommand.numArgs < MAX_ARGUMENTS) {
                parsedCommand.arguments[parsedCommand.numArgs].key = "";  // No key for non-option
                parsedCommand.arguments[parsedCommand.numArgs].value = token;
                parsedCommand.numArgs++;
            }
        }
    }

    return parsedCommand.numArgs;
}

void Arduino_Arg_Parser::reset() {
    parsedCommand.command = "";
    parsedCommand.numArgs = 0;
    currentArgIndex = 0;
}

String Arduino_Arg_Parser::cmdName() const {
    return parsedCommand.command;
}

const Arduino_Arg_Parser::Arg& Arduino_Arg_Parser::nextArg() {
    if (currentArgIndex < parsedCommand.numArgs) {
        return parsedCommand.arguments[currentArgIndex++];
    }
    return emptyArg;
}

const Arduino_Arg_Parser::ParsedCommand& Arduino_Arg_Parser::getParsedCommand() const {
    return parsedCommand;
}

const Arduino_Arg_Parser::Arg& Arduino_Arg_Parser::findArgByKey(const String &key) const {
    for (size_t i = 0; i < parsedCommand.numArgs; i++) {
        if (parsedCommand.arguments[i].key == key) {
            return parsedCommand.arguments[i];
        }
    }
    return emptyArg;
}

void Arduino_Arg_Parser::parseOption(const String &key, const String &value) {
    if (parsedCommand.numArgs < MAX_ARGUMENTS) {
        parsedCommand.arguments[parsedCommand.numArgs].key = key;
        parsedCommand.arguments[parsedCommand.numArgs].value = value;
        parsedCommand.numArgs++;
    }
}
