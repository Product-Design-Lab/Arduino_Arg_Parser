#include "Arduino_Arg_Parser.h"

// Create an instance of Arduino_Arg_Parser
Arduino_Arg_Parser parser;

void setup() {
    Serial.begin(9600);
    while (!Serial) {}  // Wait for the Serial connection

    Serial.println("Ready to parse commands.");

    // Example input string
    String input = "set_pwm --dutycycle 0.5 --frequency 100 file1 file2";

    // Parse the command
    int result = parser.parse(input);

    // Check if parsing was successful
    if (result < 0) {
        Serial.println("Error parsing command.");
        return;
    }

    // Access parsed command through getParsedCommand() and check validity
    const Arduino_Arg_Parser::ParsedCommand& parsedCmd = parser.getParsedCommand();
    if (parsedCmd.isValid()) {
        Serial.println("Command: " + parsedCmd.command);
        Serial.println("Number of arguments: " + String(parsedCmd.numArgs));
    }

    // Iterate over the arguments
    const Arduino_Arg_Parser::Arg* arg;
    while (!(arg = &parser.nextArg())->isEmpty()) {  // Call nextArg() in every iteration
        if (arg->key.length() != 0) {
            Serial.print("Option: ");
            Serial.print(arg->key);
            Serial.print(" Value: ");
            Serial.println(arg->value);
        } else {
            Serial.print("Non-option argument: ");
            Serial.println(arg->value);
        }
    }


    // Find a specific option by key (e.g., "dutycycle") and ensure it cannot be modified
    const Arduino_Arg_Parser::Arg& dutyCycleArg = parser.findArgByKey("dutycycle");
    if (!dutyCycleArg.isEmpty()) {
        float duty = dutyCycleArg.value.toFloat();
        Serial.print("Duty Cycle: ");
        Serial.println(duty);
    }
}

void loop() {
    // Do nothing here
}
