# OpenConstruct ESP32 — Ultralight Edge Node for Agent Networks

Arduino/ESP32 library that lets microcontrollers participate as **Plato shells** in OpenConstruct agent networks. ESP32s can't run full agents — but they can report sensor data, accept commands, and join fleet discovery.

**Part of [SuperInstance OpenConstruct](https://github.com/SuperInstance/OpenConstruct).**

## What This Gives You

- **WiFi + mDNS discovery** — ESP32 registers itself on the network automatically
- **MQTT command/control** — receive text commands, send sensor readings
- **Sensor registration** — digital, analog, temperature, humidity, motion
- **GPIO operations** — read/write pins through text commands
- **Human-readable output** — sensor values translated to natural language
- **Minimal footprint** — runs on any ESP32 variant with <100KB RAM

## Quick Start

```cpp
#include <openconstruct-esp32.h>

OpenConstructESP32 node;

void setup() {
    node.begin("kitchen-sensor", "WiFiSSID", "WiFiPass");
    node.registerSensor(4, "door", "digital");          // GPIO 4
    node.registerSensor(34, "light", "analog");         // ADC on GPIO 34
    node.registerSensor(23, "temp", "temperature");     // DHT22 on GPIO 23
}

void loop() {
    node.update();  // handles MQTT, mDNS, command parsing, sensor reporting
}
```

## Installation

### Arduino IDE

1. Download this repository as a ZIP
2. Sketch → Include Library → Add .ZIP Library

### PlatformIO

```ini
[env:esp32dev]
platform = espressif32
framework = arduino
lib_deps =
    https://github.com/SuperInstance/openconstruct-esp32.git
    PubSubClient
```

## How It Fits

In the OpenConstruct fleet topology, ESP32s are the sensor spokes to a Jetson hub. The [plato-fleet](https://github.com/SuperInstance/plato-fleet) module discovers them and represents their capabilities as rooms. See [openconstruct-jetson](https://github.com/SuperInstance/openconstruct-jetson) for the hub side and [openconstruct-examples](https://github.com/SuperInstance/openconstruct-examples) for a working sensor-node sketch.

## License

MIT
