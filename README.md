# OpenConstructESP32

An ultralight ESP32 edge node library for OpenConstruct networks.

## Overview

OpenConstructESP32 enables ESP32 microcontrollers to participate as **Plato shells** in OpenConstruct agent networks. While a full agent cannot run on resource-constrained microcontrollers, this library provides essential capabilities:

- **Register as a Plato shell** in the OpenConstruct network
- **Report device status** (sensors, GPIO state) as human-readable text
- **Accept text commands** and translate them to GPIO operations
- **Participate in A2A discovery** via mDNS and MQTT
- **Stream sensor data** as natural-language descriptions

## Features

- ✅ WiFi connectivity and mDNS discovery
- ✅ MQTT integration for command/control
- ✅ Sensor registration (digital, analog, temperature, humidity, motion)
- ✅ Text-based command parsing
- ✅ Human-readable sensor descriptions
- ✅ GPIO read/write operations
- ✅ Arduino IDE and PlatformIO support
- ✅ Minimal memory footprint

## Requirements

- ESP32 board (any variant)
- Arduino IDE 1.8+ or PlatformIO
- WiFi network access
- (Optional) MQTT broker

## Installation

### Arduino IDE

1. Download this repository as a ZIP
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file

### PlatformIO

Clone or add as a lib dependency in `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

lib_deps =
    https://github.com/SuperInstance/openconstruct-esp32.git
    PubSubClient
```

## Quick Start

### Basic Example

```cpp
#include <OpenConstructESP32.h>

const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASS = "your_wifi_password";
const char* AGENT_NAME = "esp32-shell-01";

OpenConstructESP32 shell;

void setup() {
    Serial.begin(115200);
    shell.begin(AGENT_NAME, WIFI_SSID, WIFI_PASS);

    // Register sensors
    shell.registerSensor(4, "door_sensor", "digital");
    shell.registerSensor(34, "light_level", "analog");
    shell.registerSensor(5, "motion_detector", "motion");
}

void loop() {
    shell.update();

    // Process Serial commands
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        String response = shell.processCommand(cmd);
        Serial.println(response);
    }

    delay(10);
}
```

### With MQTT

```cpp
#include <OpenConstructESP32.h>

const char* AGENT_NAME = "esp32-shell-01";
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASS = "your_wifi_password";
const char* MQTT_BROKER = "192.168.1.100";

OpenConstructESP32 shell;

void setup() {
    Serial.begin(115200);
    shell.begin(AGENT_NAME, WIFI_SSID, WIFI_PASS);
    shell.setMQTT(MQTT_BROKER, 1883);

    shell.registerSensor(4, "door_sensor", "digital");
}

void loop() {
    shell.update();
    delay(10);
}
```

## API Reference

### Core Methods

#### `begin(agentName, wifi_ssid, wifi_pass)`

Initialize the library and connect to WiFi.

```cpp
void begin(const char* agentName, const char* wifi_ssid, const char* wifi_pass);
```

**Parameters:**
- `agentName`: Unique name for this shell (used for mDNS and MQTT topics)
- `wifi_ssid`: WiFi network name
- `wifi_pass`: WiFi password

**Example:**
```cpp
shell.begin("esp32-kitchen", "MyWiFi", "password123");
```

---

#### `setMQTT(broker, port)`

Configure MQTT broker connection.

```cpp
void setMQTT(const char* broker, int port);
```

**Parameters:**
- `broker`: MQTT broker IP address or hostname
- `port`: MQTT port (default: 1883)

**Example:**
```cpp
shell.setMQTT("192.168.1.100", 1883);
```

---

#### `registerSensor(pin, name, type)`

Register a sensor with the shell.

```cpp
void registerSensor(int pin, const char* name, const char* type);
```

**Parameters:**
- `pin`: GPIO pin number
- `name`: Human-readable sensor name
- `type`: Sensor type (see supported types below)

**Supported Sensor Types:**
- `"digital"` - Digital input/output
- `"analog"` - Analog input (ADC, pins 32-39 on ESP32)
- `"temperature"` - Temperature sensor
- `"humidity"` - Humidity sensor
- `"motion"` - Motion detector (PIR)

**Example:**
```cpp
shell.registerSensor(4, "door", "digital");
shell.registerSensor(34, "light", "analog");
shell.registerSensor(5, "pir", "motion");
```

---

#### `getStatus()`

Get a text description of all sensors and network status.

```cpp
String getStatus();
```

**Returns:** Human-readable status string

**Example:**
```cpp
String status = shell.getStatus();
Serial.println(status);
// Output:
// [esp32-kitchen] Status:
//   WiFi: Connected
//   MQTT: Connected
//   Sensors:
//     - door (digital): OFF
//     - light (analog): 1.65V (raw: 2048)
//     - pir (motion): No motion
```

---

#### `processCommand(cmd)`

Parse and execute a text command.

```cpp
String processCommand(String cmd);
```

**Parameters:**
- `cmd`: Command string to execute

**Returns:** Command response

**Example:**
```cpp
String response = shell.processCommand("read door");
Serial.println(response);
// Output: door (digital): OFF
```

---

#### `update()`

Main loop function - reads sensors and handles MQTT.

```cpp
void update();
```

**Call this in your `loop()` function.**

**Example:**
```cpp
void loop() {
    shell.update();
    delay(10);
}
```

---

#### `isConnected()`

Check if WiFi is connected.

```cpp
bool isConnected();
```

**Returns:** `true` if connected to WiFi

---

#### `getAgentName()`

Get the shell's agent name.

```cpp
String getAgentName();
```

**Returns:** The agent name set in `begin()`

## Commands

The shell accepts the following text commands:

### `status`

Display full device and sensor status.

```
status
```

### `ping`

Check if the shell is responsive.

```
ping
```

**Response:** `PONG: <agent_name> is alive!`

### `read <sensor>`

Read a specific sensor value.

```
read <sensor_name>
```

**Example:**
```
read temperature
read door_sensor
```

### `write <pin> <value>`

Set a GPIO pin value.

```
write <pin_number> <value>
```

**Values accepted:**
- `0`, `1`, `off`, `on`, `low`, `high`, `false`, `true`

**Example:**
```
write 13 on
write 2 high
write led_pin 1
```

### `help`

Show available commands.

```
help
```

## Network Integration

### mDNS Discovery

When connected to WiFi, the shell advertises itself via mDNS:

```
<agent_name>.local
```

You can discover shells on your network:

```bash
# On Linux/macOS
ping esp32-kitchen.local

# On Windows
ping esp32-kitchen.local
```

### MQTT Topics

The library uses the following MQTT topic structure:

- **Status:** `openconstruct/<agent_name>/status`
- **Commands:** `openconstruct/<agent_name>/commands`
- **Sensors:** `openconstruct/<agent_name>/sensors`

## Hardware Examples

### Door Sensor (Digital)

```cpp
shell.registerSensor(4, "door", "digital");
```

### Light Sensor (Analog)

```cpp
shell.registerSensor(34, "light", "analog");
```

### PIR Motion Sensor

```cpp
shell.registerSensor(5, "pir", "motion");
```

### Temperature/Humidity (DHT11/DHT22)

```cpp
// Note: Requires DHT library integration
shell.registerSensor(13, "temp", "temperature");
shell.registerSensor(12, "humid", "humidity");
```

### LED Control

```cpp
// Output pin
shell.registerSensor(2, "led", "digital");

// Then control via command
shell.processCommand("write 2 on");
```

## PlatformIO Configuration

Example `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

; Serial monitor options
monitor_speed = 115200
monitor_filters = esp32_exception_decoder

; Library dependencies
lib_deps =
    https://github.com/SuperInstance/openconstruct-esp32.git
    PubSubClient

; Build options
build_flags =
    -DCORE_DEBUG_LEVEL=3
```

## Flashing with PlatformIO

```bash
# Upload to device
pio run -t upload

# Monitor serial output
pio device monitor

# Upload and monitor
pio run -t upload && pio device monitor
```

## Flashing with Arduino IDE

1. Select your board: Tools → Board → ESP32 Arduino → (your board)
2. Select port: Tools → Port → (your ESP32 port)
3. Click Upload button (→)
4. Open Serial Monitor (magnifying glass icon, 115200 baud)

## Troubleshooting

### WiFi Connection Failed

- Check SSID and password are correct
- Ensure 2.4GHz WiFi network (ESP32 doesn't support 5GHz)
- Check WiFi signal strength

### MQTT Connection Failed

- Verify broker IP and port
- Check if broker is running
- Ensure device is on the same network as broker

### Sensor Readings Are 0

- Verify pin numbers are correct
- Check sensor wiring
- For analog sensors, use pins 32-39 (ESP32 ADC1)
- Check if pin is reserved (pins 6-11 are used for flash)

### Upload Fails

- Press BOOT button when connecting to enter download mode
- Try a different USB cable (data cable required)
- Check driver: CH340 or CP2102 drivers may be needed

## Memory Usage

- **RAM:** ~15KB
- **Flash:** ~40KB
- **Stack:** Minimal (Arduino-compatible design)

## Future Enhancements

- [ ] Native DHT/DS18B20 sensor support
- [ ] OTA updates
- [ ] Web UI for configuration
- [ ] Bluetooth LE discovery
- [ ] Sensor calibration
- [ ] Event thresholds and notifications

## Contributing

Contributions welcome! Please read the contribution guidelines before submitting PRs.

## License

MIT License - see LICENSE file for details.

## Links

- **GitHub:** https://github.com/SuperInstance/openconstruct-esp32
- **Issues:** https://github.com/SuperInstance/openconstruct-esp32/issues
- **OpenConstruct:** [Main Project Link]

## Author

SuperInstance <dev@superinstance.com>