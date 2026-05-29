#include <Arduino.h>
#include <unity.h>
#include "openconstruct-esp32.h"

// Test fixture
OpenConstructESP32* shell;

void setUp(void) {
    shell = new OpenConstructESP32();
}

void tearDown(void) {
    delete shell;
}

// Test command parsing
void test_parse_command_status(void) {
    String cmd = "status";
    String response = shell->processCommand(cmd);
    TEST_ASSERT_TRUE(response.indexOf("Status") >= 0);
}

void test_parse_command_ping(void) {
    String cmd = "ping";
    String response = shell->processCommand(cmd);
    TEST_ASSERT_TRUE(response.indexOf("PONG") >= 0);
}

void test_parse_command_unknown(void) {
    String cmd = "unknown_command";
    String response = shell->processCommand(cmd);
    TEST_ASSERT_TRUE(response.indexOf("ERROR") >= 0);
}

// Test sensor registration
void test_sensor_registration(void) {
    int initialCount = 0;
    shell->registerSensor(4, "test_sensor", "digital");

    // Verify sensor was registered (check via status)
    String status = shell->getStatus();
    TEST_ASSERT_TRUE(status.indexOf("test_sensor") >= 0);
}

// Test sensor types
void test_sensor_digital(void) {
    shell->registerSensor(4, "door", "digital");
    String status = shell->getStatus();
    TEST_ASSERT_TRUE(status.indexOf("door") >= 0);
    TEST_ASSERT_TRUE(status.indexOf("digital") >= 0);
}

void test_sensor_analog(void) {
    shell->registerSensor(34, "light", "analog");
    String status = shell->getStatus();
    TEST_ASSERT_TRUE(status.indexOf("light") >= 0);
    TEST_ASSERT_TRUE(status.indexOf("analog") >= 0);
}

void test_sensor_motion(void) {
    shell->registerSensor(5, "pir", "motion");
    String status = shell->getStatus();
    TEST_ASSERT_TRUE(status.indexOf("pir") >= 0);
    TEST_ASSERT_TRUE(status.indexOf("motion") >= 0);
}

// Test invalid commands
void test_write_invalid_pin(void) {
    String cmd = "write 999 on";
    String response = shell->processCommand(cmd);
    TEST_ASSERT_TRUE(response.indexOf("ERROR") >= 0 || response.indexOf("Invalid") >= 0);
}

void test_write_missing_value(void) {
    String cmd = "write 13";
    String response = shell->processCommand(cmd);
    TEST_ASSERT_TRUE(response.indexOf("ERROR") >= 0);
}

void test_read_missing_sensor(void) {
    String cmd = "read nonexistent_sensor";
    String response = shell->processCommand(cmd);
    TEST_ASSERT_TRUE(response.indexOf("not found") >= 0);
}

// Test GPIO simulation (without hardware)
void test_gpio_command_format(void) {
    String cmd = "write 13 on";
    String response = shell->processCommand(cmd);
    // Should accept the command format
    TEST_ASSERT_FALSE(response.indexOf("Invalid write command") >= 0);
}

// Test agent name
void test_agent_name(void) {
    shell->begin("test-agent", "ssid", "pass");
    String name = shell->getAgentName();
    TEST_ASSERT_EQUAL_STRING("test-agent", name.c_str());
}

void test_connection_status_initial(void) {
    // Initially not connected (no WiFi)
    TEST_ASSERT_FALSE(shell->isConnected());
}

// Test status output format
void test_status_format(void) {
    shell->registerSensor(4, "test", "digital");
    String status = shell->getStatus();

    // Should contain key sections
    TEST_ASSERT_TRUE(status.indexOf("Status") >= 0);
    TEST_ASSERT_TRUE(status.indexOf("WiFi") >= 0);
    TEST_ASSERT_TRUE(status.indexOf("Sensors") >= 0);
}

// Main test runner
void setup() {
    delay(2000);  // Give serial time to initialize

    UNITY_BEGIN();

    // Run tests
    RUN_TEST(test_parse_command_status);
    RUN_TEST(test_parse_command_ping);
    RUN_TEST(test_parse_command_unknown);
    RUN_TEST(test_sensor_registration);
    RUN_TEST(test_sensor_digital);
    RUN_TEST(test_sensor_analog);
    RUN_TEST(test_sensor_motion);
    RUN_TEST(test_write_invalid_pin);
    RUN_TEST(test_write_missing_value);
    RUN_TEST(test_read_missing_sensor);
    RUN_TEST(test_gpio_command_format);
    RUN_TEST(test_agent_name);
    RUN_TEST(test_connection_status_initial);
    RUN_TEST(test_status_format);

    UNITY_END();
}

void loop() {
    // Nothing to do here
}