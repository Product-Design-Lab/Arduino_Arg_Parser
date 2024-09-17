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
    reset();  // Reset parsed command and arguments
    
    // The first token is the command
    int start = 0;
    int end = inputString.indexOf(' ', start);
    if (end == -1) {
        parsedCommand.command = inputString;
        return 0;  // Only command, no arguments
    }
    parsedCommand.command = inputString.substring(start, end);
    start = end + 1;

    // Process the rest of the tokens
    while (start < inputString.length()) {
        end = inputString.indexOf(' ', start);
        String token = (end == -1) ? inputString.substring(start) : inputString.substring(start, end);
        
        if (token.startsWith("-")) {
            // Option handling (both short and long)
            String key = token.startsWith("--") ? token.substring(2) : token.substring(1);
            if (end != -1 && start < inputString.length() - 1) {
                int nextStart = end + 1;
                int nextEnd = inputString.indexOf(' ', nextStart);
                if (nextEnd == -1) nextEnd = inputString.length();
                String nextToken = inputString.substring(nextStart, nextEnd);
                if (!nextToken.startsWith("-")) {
                    // Next token is a value for this key
                    parseOption(key, nextToken);
                    start = nextEnd + 1;
                    continue;
                }
            }
            // No value, treat as a flag
            parseOption(key, "true");
        } else {
            // Non-option argument
            if (parsedCommand.numArgs < MAX_ARGUMENTS) {
                parsedCommand.arguments[parsedCommand.numArgs].key = "";  // No key for non-option
                parsedCommand.arguments[parsedCommand.numArgs].value = token;
                parsedCommand.numArgs++;
            }
        }
        
        if (end == -1) break;
        start = end + 1;
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
