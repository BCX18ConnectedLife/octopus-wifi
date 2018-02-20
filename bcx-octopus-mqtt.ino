/* 
 * This sketch tests the following sensors and actuators: 
 * - WiFi MAC address
 * - ESP8266 power supply
 * - BME280 (if available)
 * - BME680 (if available)
 * - BNE055 (if available)
 * 
 */

// Enable/disable sensor measurements if you want to
// Sensors enabled, but not found in the hardware will be ignored
#define ENABLE_VCC
#define ENABLE_A0 // Analog input - will be ignored if ENABLE_VCC is set
#define ENABLE_BME280
#define ENABLE_BME680
#define ENABLE_BNO055
#define ENABLE_NEOPIXEL

#include "settings.h"

WiFiClient wifi;
byte macAddress[6];

const bool enableBME280 = true;
const bool enableBME680 = true;
const bool enableBNO055 = true;
const bool enableNeoPixel = true;


void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    setupLights();

    while (!Serial);

    Serial.println("                             "); // print some spaces to let the Serial Monitor catch up
    Serial.println();
    Serial.println("--- Initializing Octopus --- ");
    printlnMsg("Reset reason", ESP.getResetReason());
    WiFi.macAddress(macAddress);
    printMsg("MAC address", "");
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
        macAddress[0],
        macAddress[1],
        macAddress[2],
        macAddress[3],
        macAddress[4],
        macAddress[5]);

    showColor(0, 0, 0, 0, 0x80); // white
    WiFi.begin(ssid, password);
    printMsg("WiFi", "Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    showColor(0, 0x80, 0x80, 0, 0); // yellow
    randomSeed(micros());
    printMsg("WiFi", "Connected. IP address: ");
    Serial.println(WiFi.localIP());
    showColor(0, 0, 0, 0x80, 0); // blue
    setupHub();
    setupSensors();
    showColor(0, 0, 0x80, 0, 0); // green
    Serial.println();
    
    Serial.println("--- Sensor Loop --- ");        
}

void loop() {
    loopHub();
    loopSensors();
}
