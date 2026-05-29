#include <OpenConstructESP32.h>

// WiFi credentials - replace with your own
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASS = "your_wifi_password";

// MQTT broker (optional)
const char* MQTT_BROKER = "192.168.1.100";  // Replace with your broker IP
const int MQTT_PORT = 1883;

// Agent configuration
const char* AGENT_NAME = "esp32-shell-01";

OpenConstructESP32 shell;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n========================================");
    Serial.println("OpenConstruct ESP32 Shell - Basic Example");
    Serial.println("========================================\n");

    // Initialize the shell
    shell.begin(AGENT_NAME, WIFI_SSID, WIFI_PASS);

    // Optional: Configure MQTT
    shell.setMQTT(MQTT_BROKER, MQTT_PORT);

    // Register sensors
    // Modify pins and sensor types based on your hardware

    // Digital input sensor (e.g., door sensor, button)
    shell.registerSensor(4, "door_sensor", "digital");

    // Analog sensor (e.g., potentiometer, light sensor)
    shell.registerSensor(34, "light_level", "analog");

    // Digital output (e.g., LED)
    shell.registerSensor(2, "led", "digital");

    // Motion sensor (PIR)
    shell.registerSensor(5, "motion_detector", "motion");

    // Temperature sensor (simulated - connect actual DHT or DS18B20)
    shell.registerSensor(13, "temperature", "temperature");

    // Humidity sensor (simulated - connect actual DHT)
    shell.registerSensor(12, "humidity", "humidity");

    Serial.println("\nShell initialized!");
    Serial.println("Send commands via Serial Monitor (115200 baud)");
    Serial.println("Type 'help' for available commands\n");
}

void loop() {
    // Main loop: read sensors, check MQTT
    shell.update();

    // Process commands from Serial
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (!cmd.isEmpty()) {
            Serial.println("\n> " + cmd);
            String response = shell.processCommand(cmd);
            Serial.println(response);
            Serial.println();
        }
    }

    // Small delay to prevent watchdog issues
    delay(10);
}