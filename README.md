# OpenConstruct ESP32 — ESP32 Edge Node Library for Agent Networks

Arduino/ESP32 library that lets microcontrollers participate as **Plato shells** in OpenConstruct agent networks. ESP32s can't run full agents — but they can report sensor data, accept text commands, and register for fleet discovery.

**Part of [SuperInstance OpenConstruct](https://github.com/SuperInstance/OpenConstruct).**

## Status

This is an early scaffold. The command parser, sensor registration, GPIO read/write, and WiFi/mDNS connection are implemented and build cleanly under CI. MQTT publish/subscribe and the temperature/humidity sensor drivers are placeholder stubs (see below for detail).

## Capabilities

Implemented vs. placeholder, so you know what actually runs on hardware:

- ✅ **WiFi + mDNS discovery** — connects to WiFi and registers itself over mDNS as `<agentName>.local`
- ⚠️ **MQTT command/control** — `setMQTT()` accepts a broker/port, but the PubSubClient integration is a stub: `connectMQTT()` only flips an internal flag, and `publishStatus()` / `subscribeToCommands()` are commented out. No messages are sent or received yet. Commands are currently handled over Serial.
- ✅ **Sensor registration** — register digital, analog, temperature, humidity, and motion sensors by pin / name / type
- ✅ **Digital / analog / motion reads** — real hardware reads (`digitalRead`, 12-bit `analogRead`, PIR on a digital pin)
- ⚠️ **Temperature / humidity reads** — stubbed; `readTemperatureSensor` / `readHumiditySensor` return `analogRead(pin) * 0.1` as a stand-in until a real DHT or DS18B20 driver is wired in
- ✅ **GPIO operations** — read sensors and write pins (`write <pin> on/off`) through text commands
- ✅ **Human-readable output** — sensor values translated to natural language via `TextSensor`

The library keeps a small footprint (no large buffers; the only dynamic allocation is the sensor list), so it runs on any ESP32 variant.

## Quick Start

```cpp
#include <openconstruct-esp32.h>

OpenConstructESP32 node;

void setup() {
    node.begin("kitchen-sensor", "WiFiSSID", "WiFiPass");
    node.registerSensor(4, "door", "digital");          // GPIO 4
    node.registerSensor(34, "light", "analog");         // ADC on GPIO 34
    node.registerSensor(23, "temp", "temperature");     // ⚠️ simulated read (see Status)
}

void loop() {
    node.update();  // reads sensors; MQTT handling is a no-op stub for now
}
```

Text commands (over Serial, or wherever you feed `processCommand`): `status`, `ping`, `read <sensor>`, `write <pin> <value>` (`on`/`off`, `0`/`1`, `high`/`low`), and `help`.

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

The included `platformio.ini` defines an `esp32dev` build environment and a `test` environment (Unity, 15 tests in `test/test_main.cpp`). The [CI workflow](.github/workflows/ci.yml) builds the firmware and compiles the tests on every push.

## How It Fits

In the OpenConstruct fleet topology, ESP32s are the sensor spokes to a Jetson-class hub. [openconstruct-jetson](https://github.com/SuperInstance/openconstruct-jetson) is the hub side (GPU-accelerated inference, camera/sonar processing); [plato-fleet](https://github.com/SuperInstance/plato-fleet) discovers ESP32 nodes and represents their capabilities as rooms; see [openconstruct-examples](https://github.com/SuperInstance/openconstruct-examples) for a working sensor-node sketch (`examples/esp32/sensor_node.ino`).

## Related Repos

- **[openconstruct-jetson](https://github.com/SuperInstance/openconstruct-jetson)** — the GPU-accelerated hub this ESP32 acts as a sensor spoke to.
- **[plato-fleet](https://github.com/SuperInstance/plato-fleet)** — fleet discovery and topology detection; registers this device as a room-like node.
- **[plato-edge](https://github.com/SuperInstance/plato-edge)** — the host-side Plato agent runtime; this library exists so an ESP32 can act as a lightweight "Plato shell" reported into that fleet.
- **[nexus-edge-runtime](https://github.com/SuperInstance/nexus-edge-runtime)** — a heavier edge runtime with fleet coordination, sensor fusion, and a wire protocol, for devices that can run a full agent loop instead of a thin sensor node.
- **[vessel-bridge](https://github.com/SuperInstance/vessel-bridge)** — another edge bridge between hardware sensors/commands and the fleet; shares the "translate raw GPIO into fleet-meaningful messages" concern.

## License

MIT
