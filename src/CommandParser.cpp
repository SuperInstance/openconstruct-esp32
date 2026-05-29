#include "openconstruct-esp32.h"
#include <Arduino.h>

// CommandParser.cpp - Parses text commands into GPIO actions

enum CommandType {
    CMD_UNKNOWN,
    CMD_STATUS,
    CMD_PING,
    CMD_READ,
    CMD_WRITE,
    CMD_HELP
};

struct ParsedCommand {
    CommandType type;
    String target;  // sensor name or pin
    String value;   // value for write operations
    String raw;     // original command
    bool valid;
};

ParsedCommand parseCommand(const String& cmd) {
    ParsedCommand result;
    result.raw = cmd;
    result.valid = false;
    result.type = CMD_UNKNOWN;

    String trimmed = cmd;
    trimmed.trim();

    if (trimmed.length() == 0) {
        return result;
    }

    String firstWord;
    int spaceIdx = trimmed.indexOf(' ');

    if (spaceIdx == -1) {
        firstWord = trimmed;
    } else {
        firstWord = trimmed.substring(0, spaceIdx);
    }

    firstWord.toLowerCase();

    if (firstWord == "status") {
        result.type = CMD_STATUS;
        result.valid = true;
    } else if (firstWord == "ping") {
        result.type = CMD_PING;
        result.valid = true;
    } else if (firstWord == "read") {
        result.type = CMD_READ;
        if (spaceIdx == -1) {
            result.valid = false;  // Missing sensor name
        } else {
            result.target = trimmed.substring(spaceIdx + 1);
            result.target.trim();
            result.valid = !result.target.isEmpty();
        }
    } else if (firstWord == "write") {
        result.type = CMD_WRITE;
        if (spaceIdx == -1) {
            result.valid = false;  // Missing pin
        } else {
            String rest = trimmed.substring(spaceIdx + 1);
            rest.trim();

            int secondSpace = rest.indexOf(' ');
            if (secondSpace == -1) {
                result.valid = false;  // Missing value
            } else {
                result.target = rest.substring(0, secondSpace);
                result.target.trim();
                result.value = rest.substring(secondSpace + 1);
                result.value.trim();
                result.valid = !result.target.isEmpty() && !result.value.isEmpty();
            }
        }
    } else if (firstWord == "help") {
        result.type = CMD_HELP;
        result.valid = true;
    }

    return result;
}

bool isValidPin(const String& pinStr) {
    // ESP32 pins: 0-39 (some reserved)
    int pin = pinStr.toInt();

    // Check if conversion was successful
    if (String(pin) != pinStr) {
        return false;
    }

    // Valid ESP32 GPIO pins
    if (pin < 0 || pin > 39) {
        return false;
    }

    // Reserved pins (6-11 are used for flash)
    if (pin >= 6 && pin <= 11) {
        return false;
    }

    return true;
}

bool isValidValue(const String& valueStr) {
    // Accept: 0, 1, on, off, high, low, true, false
    String lower = valueStr;
    lower.toLowerCase();

    if (lower == "0" || lower == "1" || lower == "on" || lower == "off" ||
        lower == "high" || lower == "low" || lower == "true" || lower == "false") {
        return true;
    }

    // Check if it's a number
    int val = valueStr.toInt();
    if (String(val) != valueStr) {
        return false;
    }

    return val >= 0 && val <= 1;
}

int parseValue(const String& valueStr) {
    String lower = valueStr;
    lower.toLowerCase();

    if (lower == "1" || lower == "on" || lower == "high" || lower == "true") {
        return HIGH;
    }

    return LOW;
}

String getHelpText() {
    return R"(
OpenConstruct ESP32 Shell Commands:

  status          - Display device and sensor status
  ping            - Check if shell is responsive
  read <sensor>   - Read a specific sensor value
  write <pin> <value>  - Set GPIO pin value (0/1, on/off, high/low)
  help            - Show this help message

Sensor Types:
  digital   - Digital input/output
  analog    - Analog input (ADC)
  temperature - Temperature sensor
  humidity  - Humidity sensor
  motion    - Motion detector (PIR)

Example Commands:
  read temperature
  read door_sensor
  write 13 on
  write led_pin 1
  status
  ping
)";
}

String formatCommandResponse(const ParsedCommand& cmd, const String& result) {
    String response = "CMD: " + cmd.raw + "\n";
    response += "→ " + result;
    return response;
}

String formatError(const String& message) {
    return "ERROR: " + message;
}