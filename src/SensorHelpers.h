#ifndef SENSOR_HELPERS_H
#define SENSOR_HELPERS_H

#include <Arduino.h>

// Helper functions for common sensors
// Extend this file to add more sensor types

// Simple analog read with smoothing
float readAnalogSmoothed(int pin, int samples = 10) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(pin);
        delay(1);
    }
    return (float)sum / samples;
}

// Convert analog to voltage (ESP32: 0-3.3V, 12-bit ADC)
float analogToVoltage(int rawValue) {
    return (rawValue / 4095.0) * 3.3;
}

// Simple temperature simulation (for testing)
float simulateTemperature(int analogPin) {
    float voltage = analogToVoltage(analogRead(analogPin));
    // LM35 simulation: 10mV per degree C
    return voltage * 100.0;
}

// Simple humidity simulation (for testing)
float simulateHumidity(int analogPin) {
    float voltage = analogToVoltage(analogRead(analogPin));
    // Linear simulation: 0V = 0%, 3.3V = 100%
    return (voltage / 3.3) * 100.0;
}

// Photoresistor to lux (approximate)
float photoresistorToLux(int analogPin, float resistorValue = 10000.0) {
    int raw = analogRead(analogPin);
    float vOut = analogToVoltage(raw);
    float rPhoto = resistorValue * ((3.3 - vOut) / vOut);

    // Approximate lux calculation
    float lux = 500.0 / rPhoto;
    return lux;
}

// MQ-2 gas sensor (smoke, LPG, CO)
float readGasSensor(int analogPin) {
    int raw = analogRead(analogPin);
    float voltage = analogToVoltage(raw);
    // Sensor output: 0.1V (clean) to ~1V (gas detected)
    // Returns 0-100% scale
    float percentage = (voltage / 1.0) * 100.0;
    return constrain(percentage, 0.0, 100.0);
}

// Sound sensor (analog microphone)
float readSoundLevel(int analogPin) {
    int raw = readAnalogSmoothed(analogPin, 20);
    float voltage = analogToVoltage(raw);
    // 0-3.3V scale, convert to dB approximation
    float db = 20.0 * log10(voltage / 0.00631);  // Reference: 0.00631V
    return constrain(db, 30.0, 120.0);
}

// DHT sensor placeholder - implement with DHT library
#ifdef DHT_H
float readDHTTemperature(DHT& dht) {
    float t = dht.readTemperature();
    if (isnan(t)) return -999.0;
    return t;
}

float readDHTHumidity(DHT& dht) {
    float h = dht.readHumidity();
    if (isnan(h)) return -999.0;
    return h;
}
#endif

#endif // SENSOR_HELPERS_H