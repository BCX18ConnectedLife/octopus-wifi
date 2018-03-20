#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_Sensor.h>  // Make sure you have the Adafruit Sensor library installed
#ifdef ENABLE_BME280
#include <Adafruit_BME280.h>  // Make sure you have the Adafruit BME280 library installed
#endif
#ifdef ENABLE_BME680
#include <Adafruit_BME680.h>  // Make sure you have the Adafruit BME680 library installed
#endif
#ifdef ENABLE_BNO055
#include <Adafruit_BNO055.h>  // Make sure you have the Adafruit BNO055 library installed
#include <utility/imumaths.h>
#endif
#ifdef ENABLE_NEOPIXEL
#include <Adafruit_NeoPixel.h> // Make sure you have the Adafruit NeoPixel library installed
#endif

// ---- Hardware Configuration ----

#ifdef ENABLE_VCC
ADC_MODE(ADC_VCC); // enable reading in VCC of ESP8266
#endif

const int sensorUpdateRateMS = 5000; // Print updated sensor value every 5 seconds
const int loopDelay = 100;

#define PIN_NEOPIXEL      13
#define SEALEVELPRESSURE_HPA (1013.25)

// ---- Network configuration ----

/* Bosch IoT Hub MQTT configuration */
#define HUB_TENANT "..."
#define THINGS_NAMESPACE "..."

const char* mqttBroker = "mqtt.bosch-iot-hub.com";
const int   mqttPort = 8883;
const char* mqttPassword = "...";
/* SHA-1 fingerprint of the server certificate of the MQTT broker, UPPERCASE and spacing */
const char* mqttServerFingerprint = "EE 6A DB 0F B7 C3 E1 7F B4 FB BB A2 95 C5 DC E1 4F FE B1 7F";

/* WiFi Configuration */
const char* ssid = "...";
const char* password = "...";

    
// ---- Data structures ----

struct Bno055Values {
    float orientationX;
    float orientationY;
    float orientationZ;
    uint8_t calibrationSys;
    uint8_t calibrationGyro;
    uint8_t calibrationAccel;
    uint8_t calibrationMag;
};


