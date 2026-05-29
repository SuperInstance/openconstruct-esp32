#ifndef OPENCONSTRUCT_ESP32_H
#define OPENCONSTRUCT_ESP32_H

#include <Arduino.h>
#include <vector>

struct Sensor {
    int pin;
    String name;
    String type;
    float lastValue;
    unsigned long lastRead;
};

class OpenConstructESP32 {
public:
    OpenConstructESP32();

    // Core initialization
    void begin(const char* agentName, const char* wifi_ssid, const char* wifi_pass);

    // Sensor management
    void registerSensor(int pin, const char* name, const char* type);

    // Status and commands
    String getStatus();  // text description of all sensors
    String processCommand(String cmd);  // parse and execute text command

    // Main loop
    void update();  // read sensors, check MQTT

    // MQTT configuration
    void setMQTT(const char* broker, int port);

    // Network status
    bool isConnected();
    String getAgentName();

private:
    String _agentName;
    String _wifiSSID;
    String _wifiPass;
    String _mqttBroker;
    int _mqttPort;
    bool _wifiConnected;
    bool _mqttConnected;

    std::vector<Sensor> _sensors;

    // Internal methods
    bool connectWiFi();
    bool connectMQTT();
    void readSensors();
    Sensor* findSensor(const String& name);
    String executeRead(const String& sensorName);
    String executeWrite(const String& pinStr, const String& valueStr);
    String executePing();
    float readAnalogSensor(int pin);
    float readTemperatureSensor(int pin);
    float readHumiditySensor(int pin);
    bool readMotionSensor(int pin);
    void publishStatus();
    void subscribeToCommands();
};

#endif // OPENCONSTRUCT_ESP32_H