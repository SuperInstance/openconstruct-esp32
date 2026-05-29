#include "openconstruct-esp32.h"
#include <Arduino.h>

// TextSensor.cpp - Translates sensor readings into human-readable text descriptions

String describeDigitalValue(float value) {
    return value > 0.5 ? "ON" : "OFF";
}

String describeAnalogValue(float value, int pin) {
    // Convert raw ADC to voltage (ESP32: 0-3.3V, 12-bit ADC)
    float voltage = (value / 4095.0) * 3.3;
    return String(voltage, 2) + "V (raw: " + String((int)value) + ")";
}

String describeTemperature(float value) {
    return String(value, 1) + "°C";
}

String describeHumidity(float value) {
    return String(value, 1) + "%";
}

String describeMotion(float value) {
    return value > 0.5 ? "MOTION DETECTED" : "No motion";
}

String describeSensor(const Sensor& sensor) {
    String sensorType = sensor.type.toLowerCase();
    String description;

    if (sensorType == "digital") {
        description = describeDigitalValue(sensor.lastValue);
    } else if (sensorType == "analog") {
        description = describeAnalogValue(sensor.lastValue, sensor.pin);
    } else if (sensorType == "temperature") {
        description = describeTemperature(sensor.lastValue);
    } else if (sensorType == "humidity") {
        description = describeHumidity(sensor.lastValue);
    } else if (sensorType == "motion") {
        description = describeMotion(sensor.lastValue);
    } else {
        description = String(sensor.lastValue);
    }

    return sensor.name + " [" + sensor.type + "]: " + description;
}

String formatSensorList(const std::vector<Sensor>& sensors) {
    if (sensors.empty()) {
        return "No sensors registered.";
    }

    String result = "Sensors:\n";
    for (const auto& sensor : sensors) {
        result += "  • " + describeSensor(sensor) + "\n";
    }

    return result;
}

String formatSensorStatus(const std::vector<Sensor>& sensors) {
    if (sensors.empty()) {
        return "No sensors available.";
    }

    String result = "";
    for (const auto& sensor : sensors) {
        String sensorType = sensor.type.toLowerCase();
        String valueText;

        if (sensorType == "digital") {
            valueText = describeDigitalValue(sensor.lastValue);
        } else if (sensorType == "temperature") {
            valueText = describeTemperature(sensor.lastValue);
        } else if (sensorType == "humidity") {
            valueText = describeHumidity(sensor.lastValue);
        } else if (sensorType == "motion") {
            valueText = describeMotion(sensor.lastValue);
        } else {
            valueText = String(sensor.lastValue, 2);
        }

        result += sensor.name + ":" + valueText + ";";
    }

    // Remove trailing semicolon
    if (result.length() > 0 && result.charAt(result.length() - 1) == ';') {
        result.remove(result.length() - 1);
    }

    return result;
}