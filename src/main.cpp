#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>

// TODO Get values from ENV values
#define NAME "Heating monitor"
#define VERSION "0.1.0 (27/12/2023)"

// TODO Change SSID to IoT subnet.
// TODO Get credentials from INI file
// TODO Obfuscate credentials
const char* SSID = "BraveNewWorld";
const char* PASS = "CucPrcFlus.088";

OneWire oneWire(D0);
DallasTemperature sensors(&oneWire);

const uint8_t SENSOR_RED[]   = { 0x28, 0xFF, 0x64, 0x1E, 0x9F, 0xF6, 0x89, 0x3F };
const uint8_t SENSOR_BLUE[]  = { 0x28, 0xFF, 0x64, 0x1E, 0x9F, 0xBE, 0x7C, 0x99 };
const uint8_t SENSOR_GREEN[] = { 0x28, 0xFF, 0x64, 0x1E, 0x9F, 0x89, 0x71, 0x27 };
const uint8_t SENSOR_WHITE[] = { 0x28, 0xFF, 0x64, 0x1E, 0x9F, 0xC3, 0x49, 0x27 };

// Flashing statuses for error
void flashLED(int count, int up, int down) {
    for (int i = 0; i < count; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(up);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(down);
    }
}

void setupWifiAsStation() {
    Serial.printf("Connecting to net SSID=[");
    Serial.print(SSID);
    Serial.print("] .");

    WiFi.begin(SSID, PASS);
    while (WiFi.status() != WL_CONNECTED) {
        flashLED(2, 125, 125);
        Serial.print(".");
    }
    Serial.println(" Ok");

    Serial.printf("IP=[");
    Serial.print(WiFi.localIP());
    Serial.println("]");

    Serial.print("MAC=[");
    Serial.print(WiFi.macAddress());
    Serial.println("]");
}

void printDeviceAddress(const uint8_t * deviceAddress)
{
    for (uint8_t i = 0; i < 8; i++) {
        if (deviceAddress[i] < 16) {
            Serial.print("0");
        }
        Serial.print(deviceAddress[i], HEX);
    }
}

void detectSensors()
{
    sensors.begin();

    uint8_t deviceCount = sensors.getDeviceCount();
    Serial.printf("Detected [%d] DS18B20 sensors\n", deviceCount);

    DeviceAddress address;
    for (uint8_t i = 0; i < deviceCount; i++) {
        sensors.getAddress(address, i);
        printDeviceAddress(address);
        Serial.println();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.print(NAME);
    Serial.print(" ");
    Serial.println(VERSION);

    pinMode(LED_BUILTIN, OUTPUT);
    flashLED(2, 200, 100);
    delay(1000);

    setupWifiAsStation();

    #ifdef DETECT_SENSORS
    detectSensors();
    #endif
}

void printTemperature(const uint8_t * deviceAddress) {

    float temperature = sensors.getTempC(deviceAddress);
    printDeviceAddress(deviceAddress);
    Serial.print(" = ");
    if (temperature == DEVICE_DISCONNECTED_C) {
        Serial.print("device disconnected");
    } else {
        Serial.print(temperature);
    }
    Serial.println();
}

void loop() {
    flashLED(3, 20, 50);
    delay(2000);

    sensors.requestTemperaturesByAddress(SENSOR_RED);
    printTemperature(SENSOR_RED);

    sensors.requestTemperaturesByAddress(SENSOR_GREEN);
    printTemperature(SENSOR_GREEN);

    sensors.requestTemperaturesByAddress(SENSOR_BLUE);
    printTemperature(SENSOR_BLUE);

    sensors.requestTemperaturesByAddress(SENSOR_WHITE);
    printTemperature(SENSOR_WHITE);

    Serial.println();
}