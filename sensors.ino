#ifdef ENABLE_BME280
Adafruit_BME280 bme280; // I2C
bool bme280Initialized = false;
#endif
#ifdef ENABLE_BME680
Adafruit_BME680 bme680; // I2C
bool bme680Initialized = false;
#endif
#ifdef ENABLE_BNO055
Adafruit_BNO055 bno055 = Adafruit_BNO055(55);
bool bno055Initialized = false;
Bno055Values bnoValues; 
#endif

#ifdef ENABLE_NEOPIXEL
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN_NEOPIXEL, NEO_GRBW + NEO_KHZ800);
#endif

unsigned long lastSensorUpdateMillis = 0;

// ---- Functions ----

void setupLights() {
#ifdef ENABLE_NEOPIXEL
    printlnMsg("Neopixel", "Initializing Neopixels");
    strip.begin();
    strip.show(); 
    // Initialize all pixels to 'off'
    strip.setPixelColor(0,0,0,0); // alle aus
    strip.setPixelColor(1,0,0,0);
    strip.show(); 
#endif
}

void setupSensors() {
    delay(1000); // give sensors some time to start up
    
 #ifdef ENABLE_BME280
    printMsg("Sensors", "Initializing BME280: ");
    if (bme280.begin()) {
        bme280Initialized = true;
        Serial.println("OK");
    } else {
        Serial.println("Not found");
    }
#endif
#ifdef ENABLE_BME680
    printMsg("Sensors", "Initializing BME680: ");
    if (bme680.begin(118)) {
        bme680.setTemperatureOversampling(BME680_OS_8X);
        bme680.setHumidityOversampling(BME680_OS_2X);
        bme680.setPressureOversampling(BME680_OS_4X);
        bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme680.setGasHeater(320, 150); // 320*C for 150 ms
        bme680Initialized = true;
        Serial.println("OK");
    } else {
        Serial.println("Not found");
    }
#endif
#ifdef ENABLE_BNO055
    printMsg("Sensors", "Initializing BNO055: ");
    if (bno055.begin()) {
        bno055.setExtCrystalUse(true);
        bno055Initialized = true;
        Serial.println("OK");
    } else {
        Serial.println("Not found");
    }
#endif
    delay(500);
}

float getVcc () {
  return ESP.getVcc() / 1000.0;
}

// ---- Pretty-printing Functions ----

float humidityMin = 1E+20;
float humidityMax = 1E-20;
float humidity;
float tempMin = 1E+20;
float tempMax = 1E-20;
float temp;
float barometerMin = 1E+20;
float barometerMax = 1E-20;
float barometer;
float powerMin = 1E+20;
float powerMax = 1E-20;
float power;
float analogInMin = 1E+20;
float analogInMax = 1E-20;
float analogIn;

void loopSensors() {
    if (millis() - lastSensorUpdateMillis > sensorUpdateRateMS) {
        lastSensorUpdateMillis = millis();
        printSensors();
    }
    delay(loopDelay);
}

void printSensors () {
#ifdef ENABLE_VCC
    printMsg("VCC", "ESP8266 supply voltage: ");
    Serial.println(getVcc());
    power = getVcc();
    if (powerMin > power) {
        powerMin = power;
    }
    if (powerMax < power) {
        powerMax = power;
    }
    publishToHub(sensorUpdateString("Power_0", power, powerMin, powerMax, "V"));            
#else
#ifdef ENABLE_A0
    printMsg("AnalogIn", "");
    Serial.println(analogRead(A0));
    analogIn = ((float) analogRead(A0)) * (5.0 / 1023.0);
    if (analogInMin < analogIn) {
        analogInMin = analogIn;
    }
    if (analogInMax > analogIn) {
        analogInMax = analogIn;
    }
    publishToHub(sensorUpdateString("AnalogInput_0", analogIn, analogInMin, analogInMax, "V"));
#endif
#endif


#ifdef ENABLE_BME280
    if (bme280Initialized) {
        printMsg("BME280", "");
        Serial.print("temp: ");
        Serial.print(bme280.readTemperature());
        Serial.print(" °C, pressure: ");
        Serial.print(bme280.readPressure() / 100.0F);
        Serial.print(" hPa, humidity: ");
        Serial.print(bme280.readHumidity());
        Serial.print(" %, altitude = ");
        Serial.print(bme280.readAltitude(SEALEVELPRESSURE_HPA));
        Serial.println(" m");
    }
#endif

#ifdef ENABLE_BME680
    if (bme680Initialized) {
        printMsg("BME680", "");
        if (!bme680.performReading()) { 
            Serial.println("Sensor reading failure");
        } else {
            Serial.print("temp: ");
            Serial.print(bme680.temperature);
            Serial.print(" °C, pressure: ");
            Serial.print(bme680.pressure / 100.0);
            Serial.print(" hPa, humidity: ");
            Serial.print(bme680.humidity);
            Serial.print(" %, Gas: ");
            Serial.print(bme680.gas_resistance / 1000.0);
            Serial.print(" KOhms, altitude = ");
            Serial.print(bme680.readAltitude(SEALEVELPRESSURE_HPA));
            Serial.println(" m");

            
            humidity = bme680.humidity;
            if (humidityMin > humidity) {
                humidityMin = humidity;
            }
            if (humidityMax < humidity) {
                humidityMax = humidity;
            }
            publishToHub(sensorUpdateString("HumiditySensor_0", humidity, humidityMin, humidityMax, "%"));
            temp = bme680.temperature;
            if (tempMin > temp) {
                tempMin = temp;
            }
            if (tempMax < temp) {
                tempMax = temp;
            }
            publishToHub(sensorUpdateString("TemperatureSensor_0", temp, tempMin, tempMax, "°C"));
            barometer = bme680.pressure / 100.0;
            if (barometerMin > barometer) {
                barometerMin = barometer;
            }
            if (barometerMax < barometer) {
                barometerMax = barometer;
            }
            publishToHub(sensorUpdateString("Barometer_0", barometer, barometerMin, barometerMax, "hPa"));        
        }
    }
#endif

#ifdef ENABLE_BNO055
    if (bno055Initialized) {
        uint8_t system, gyro, accel, mag = 0;
        // Only use values if calibration status > 0
        printMsg("BNO055", "Calibration status: Sys=");
        bno055.getCalibration(&system, &gyro, &accel, &mag);
        Serial.print(system, DEC);
        Serial.print(", Gyro=");
        Serial.print(gyro, DEC);
        Serial.print(", Accel=");
        Serial.print(accel, DEC);
        Serial.print(", Mag=");
        Serial.println(mag, DEC);

        printMsg("BNO055", "Ambient Temperature: ");
        int8_t bnoTemp = bno055.getTemp();
        Serial.print(bnoTemp);
        Serial.println("°C");

        imu::Vector<3> bnoAccel = bno055.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        printMsg("BNO055", "Acceleration (m/s^2) x=");
        Serial.print(bnoAccel.x());
        Serial.print(", y=");
        Serial.print(bnoAccel.y());
        Serial.print(", z=");
        Serial.println(bnoAccel.z());

        imu::Vector<3> bnoEuler = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);
        printMsg("BNO055", "Abs Orientation (°, Euler) x=");
        Serial.print(bnoEuler.x());
        Serial.print(", y=");
        Serial.print(bnoEuler.y());
        Serial.print(", z=");
        Serial.println(bnoEuler.z());
        
        imu::Vector<3> bnoGravity = bno055.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
        printMsg("BNO055", "Gravity (m/s^2) x=");
        Serial.print(bnoGravity.x());
        Serial.print(", y=");
        Serial.print(bnoGravity.y());
        Serial.print(", z=");
        Serial.println(bnoGravity.z());

        imu::Vector<3> bnoGyro = bno055.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        printMsg("BNO055", "Angular velocity (rad/s) x=");
        Serial.print(bnoGyro.x());
        Serial.print(", y=");
        Serial.print(bnoGyro.y());
        Serial.print(", z=");
        Serial.println(bnoGyro.z());
    
        imu::Vector<3> bnoLinearAccel = bno055.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        printMsg("BNO055", "Linear Acceleration (m/s^2) x=");
        Serial.print(bnoLinearAccel.x());
        Serial.print(", y=");
        Serial.print(bnoLinearAccel.y());
        Serial.print(", z=");
        Serial.println(bnoLinearAccel.z());
    
        imu::Vector<3> bnoMagnet = bno055.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        printMsg("BNO055", "Magnetic Field Strength (uT) x=");
        Serial.print(bnoMagnet.x());
        Serial.print(", y=");
        Serial.print(bnoMagnet.y());
        Serial.print(", z=");
        Serial.println(bnoMagnet.z());
        if (system > 0) {
            publishToHub(sensor3dUpdateString("Accelerometer_0", bnoAccel.x(), bnoAccel.y(), bnoAccel.z(), "m/s^2"));
            publishToHub(sensor3dUpdateString("AbsoluteOrientation_0", bnoEuler.x(), bnoEuler.y(), bnoEuler.z(), "°"));
            publishToHub(sensor3dUpdateString("Gravity_0", bnoGravity.x(), bnoGravity.y(), bnoGravity.z(), "m/s^2"));
            publishToHub(sensor3dUpdateString("AngularVelocity_0", bnoGyro.x(), bnoGyro.y(), bnoGyro.z(), "rad/s"));
            publishToHub(sensor3dUpdateString("LinearAcceleration_0", bnoLinearAccel.x(), bnoLinearAccel.y(), bnoLinearAccel.z(), "m/s^2"));
            publishToHub(sensor3dUpdateString("Magnetometer_0", bnoMagnet.x(), bnoMagnet.y(), bnoMagnet.z(), "uT"));
        } else {
            printlnMsg("BNO055", "Skipping update to hub, system calibration < 1");
        }

    }
#endif


    Serial.println();

}

void showColor(char led, char red, char green, char blue, char white) {
#ifdef ENABLE_NEOPIXEL
    strip.setPixelColor(led, red, green, blue, white);
    strip.show();
#endif
}

