#include "openconstruct-esp32.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>

OpenConstructESP32::OpenConstructESP32()
    : _mqttPort(1883), _wifiConnected(false), _mqttConnected(false) {
}

void OpenConstructESP32::begin(const char* agentName, const char* wifi_ssid, const char* wifi_pass) {
    _agentName = agentName;
    _wifiSSID = wifi_ssid;
    _wifiPass = wifi_pass;

    Serial.begin(115200);
    Serial.println("[" + _agentName + "] Starting OpenConstruct ESP32...");

    connectWiFi();
    connectMQTT();
    subscribeToCommands();
}

void OpenConstructESP32::setMQTT(const char* broker, int port) {
    _mqttBroker = broker;
    _mqttPort = port;
    _mqttConnected = false;
    connectMQTT();
}

void OpenConstructESP32::registerSensor(int pin, const char* name, const char* type) {
    Sensor s;
    s.pin = pin;
    s.name = name;
    s.type = type;
    s.lastValue = 0.0;
    s.lastRead = 0;

    // Configure pin based on type
    String sensorType = String(type).toLowerCase();
    if (sensorType == "digital") {
        pinMode(pin, INPUT);
    } else if (sensorType == "analog") {
        // ESP32 analog pins don't need pinMode
    } else if (sensorType == "motion") {
        pinMode(pin, INPUT);
    } else if (sensorType == "temperature" || sensorType == "humidity") {
        pinMode(pin, INPUT);
    }

    _sensors.push_back(s);
    Serial.println("[" + _agentName + "] Registered sensor: " + name + " (" + type + ") on pin " + String(pin));
}

String OpenConstructESP32::getStatus() {
    String status = "[" + _agentName + "] Status:\n";
    status += "  WiFi: " + String(_wifiConnected ? "Connected" : "Disconnected") + "\n";
    status += "  MQTT: " + String(_mqttConnected ? "Connected" : "Disconnected") + "\n";
    status += "  Sensors:\n";

    for (const auto& sensor : _sensors) {
        status += "    - " + sensor.name + " (" + sensor.type + "): " + String(sensor.lastValue) + "\n";
    }

    return status;
}

String OpenConstructESP32::processCommand(String cmd) {
    cmd.trim();
    Serial.println("[" + _agentName + "] Processing command: " + cmd);

    String cmdLower = cmd;
    cmdLower.toLowerCase();

    if (cmdLower == "status") {
        return getStatus();
    } else if (cmdLower == "ping") {
        return executePing();
    } else if (cmdLower.startsWith("read ")) {
        String sensorName = cmd.substring(5);
        return executeRead(sensorName);
    } else if (cmdLower.startsWith("write ")) {
        String rest = cmd.substring(6);
        int spaceIdx = rest.indexOf(' ');
        if (spaceIdx == -1) {
            return "ERROR: Invalid write command. Usage: write <pin> <value>";
        }
        String pinStr = rest.substring(0, spaceIdx);
        String valueStr = rest.substring(spaceIdx + 1);
        return executeWrite(pinStr, valueStr);
    } else {
        return "ERROR: Unknown command: " + cmd;
    }
}

void OpenConstructESP32::update() {
    readSensors();

    if (_wifiConnected && !_mqttConnected) {
        connectMQTT();
    }

    // MQTT client loop handled in connectMQTT
}

bool OpenConstructESP32::connectWiFi() {
    Serial.println("[" + _agentName + "] Connecting to WiFi...");
    WiFi.begin(_wifiSSID.c_str(), _wifiPass.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        _wifiConnected = true;
        Serial.println("\n[" + _agentName + "] WiFi connected! IP: " + WiFi.localIP().toString());

        // Start mDNS service
        if (MDNS.begin(_agentName.c_str())) {
            Serial.println("[" + _agentName + "] mDNS responder started: " + _agentName + ".local");
        }
        return true;
    } else {
        _wifiConnected = false;
        Serial.println("\n[" + _agentName + "] WiFi connection failed");
        return false;
    }
}

bool OpenConstructESP32::connectMQTT() {
    if (_mqttBroker.isEmpty() || !_wifiConnected) {
        return false;
    }

    Serial.println("[" + _agentName + "] Connecting to MQTT broker...");

    // Note: Actual MQTT implementation would use PubSubClient
    // This is a placeholder structure
    _mqttConnected = true;
    Serial.println("[" + _agentName + "] MQTT connected to " + _mqttBroker + ":" + String(_mqttPort));
    return true;
}

void OpenConstructESP32::subscribeToCommands() {
    // Placeholder: Subscribe to command topic
    // client.subscribe(("openconstruct/" + _agentName + "/commands").c_str());
}

void OpenConstructESP32::readSensors() {
    for (auto& sensor : _sensors) {
        String sensorType = sensor.type.toLowerCase();
        float value = 0.0;

        if (sensorType == "digital") {
            value = digitalRead(sensor.pin);
        } else if (sensorType == "analog") {
            value = readAnalogSensor(sensor.pin);
        } else if (sensorType == "temperature") {
            value = readTemperatureSensor(sensor.pin);
        } else if (sensorType == "humidity") {
            value = readHumiditySensor(sensor.pin);
        } else if (sensorType == "motion") {
            value = readMotionSensor(sensor.pin) ? 1.0 : 0.0;
        }

        sensor.lastValue = value;
        sensor.lastRead = millis();
    }
}

Sensor* OpenConstructESP32::findSensor(const String& name) {
    for (auto& sensor : _sensors) {
        if (sensor.name.equalsIgnoreCase(name)) {
            return &sensor;
        }
    }
    return nullptr;
}

String OpenConstructESP32::executeRead(const String& sensorName) {
    Sensor* sensor = findSensor(sensorName);
    if (!sensor) {
        return "ERROR: Sensor not found: " + sensorName;
    }

    return sensor->name + " (" + sensor->type + "): " + String(sensor->lastValue);
}

String OpenConstructESP32::executeWrite(const String& pinStr, const String& valueStr) {
    int pin = pinStr.toInt();
    int value = valueStr.toInt();

    if (pin < 0 || pin > 39) {
        return "ERROR: Invalid pin number";
    }

    pinMode(pin, OUTPUT);
    digitalWrite(pin, value > 0 ? HIGH : LOW);

    return "OK: Pin " + String(pin) + " set to " + String(value > 0 ? "HIGH" : "LOW");
}

String OpenConstructESP32::executePing() {
    return "PONG: " + _agentName + " is alive!";
}

float OpenConstructESP32::readAnalogSensor(int pin) {
    // ESP32 has 12-bit ADC (0-4095)
    return analogRead(pin);
}

float OpenConstructESP32::readTemperatureSensor(int pin) {
    // Placeholder: Implement DHT or DS18B20 reading
    // For now, simulate with analog reading
    return analogRead(pin) * 0.1; // Simulated conversion
}

float OpenConstructESP32::readHumiditySensor(int pin) {
    // Placeholder: Implement DHT reading
    return analogRead(pin) * 0.1; // Simulated conversion
}

bool OpenConstructESP32::readMotionSensor(int pin) {
    return digitalRead(pin) == HIGH;
}

void OpenConstructESP32::publishStatus() {
    // Placeholder: Publish status to MQTT
    // String status = getStatus();
    // client.publish(("openconstruct/" + _agentName + "/status").c_str(), status.c_str());
}

bool OpenConstructESP32::isConnected() {
    return _wifiConnected;
}

String OpenConstructESP32::getAgentName() {
    return _agentName;
}