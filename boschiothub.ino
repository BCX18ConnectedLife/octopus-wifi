WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

/* Device Configuration */
char deviceId[100] ;
char mqttUsername[100];
char mqttTelemetryTopic[200];

void setupHub() {
    byte macAddress[6];
    
    WiFi.macAddress(macAddress);
    sprintf(deviceId, "octopus.%02x%02x", macAddress[4], macAddress[5]);
    sprintf(mqttUsername, "octopus.%02x%02x@%s", macAddress[4], macAddress[5], HUB_TENANT);    
    sprintf(mqttTelemetryTopic, "telemetry/%s/%s", HUB_TENANT, deviceId);

    printMsg("Bosch IoT Hub", "Establishing connection. Device ID: ");
    Serial.print(deviceId);
    Serial.print(", username: ");
    Serial.println(mqttUsername);


    mqttClient.setServer(mqttBroker, mqttPort);
    if (!wifiClient.connect(mqttBroker, mqttPort)) {
        printlnMsg("Bosch IoT Hub", "Connect failed, will restart");
        /* Secure connection failed, start over */
        ESP.restart();
    } else {
        printlnMsg("Bosch IoT Hub", "Secure connection established"); 
    }       
    if (!wifiClient.verify(mqttServerFingerprint, mqttBroker)) {
       printlnMsg("Bosch IoT Hub", "Failed to verify certificate, restarting");
       /* Verify failed, start over */
       ESP.restart();
    } else {
        printlnMsg("Bosch IoT Hub", "Server certificate verified"); 
    }       
}


void loopHub() {
    if (!mqttClient.connected()) {
        printMsg("Bosch IoT Hub", "Broker login");        
        showColor(1, 0x80, 0, 0, 0);
    }
    while (!mqttClient.connected())
    {
        Serial.print(".");
        /* If connected to the MQTT broker... */
        if (mqttClient.connect(deviceId, mqttUsername, mqttPassword))
        {
            Serial.println("OK");
        } else {
            /* otherwise wait for 1 second before retrying */
            delay(1000);
        }
    }
    showColor(1, 0, 0x80, 0, 0);
    mqttClient.loop();
}

void publishToHub(String payload) {
    printlnMsg("Bosch IoT Hub", payload);
    /* Publish all available data to the MQTT broker */
    if (!mqttClient.publish(mqttTelemetryTopic, payload.c_str())) {
            printlnMsg("Bosch IoT Hub", "Publish failed, if this happens repeatedly increase MQTT_MAX_PACKET_SIZE in PubSubClient.h");
    }   
}

String sensorUpdateString(String featureName, float sensorValue, float minValue, float maxValue, String units) {
    String output = "{\"topic\": \"";
    output += THINGS_NAMESPACE;
    output += "/";
    output += deviceId;
    output += "/things/twin/commands/modify\",\"headers\": {\"response-required\": false},";
    output += "\"path\": \"/features/" + featureName + "/properties/status\", \"value\": {\"sensorValue\": " + sensorValue;
    output += ", \"minMeasuredValue\": ";
    output += minValue;
    output += ", \"maxMeasuredValue\": ";
    output += maxValue;
    output += ", \"sensorUnits\": \"";
    output += units;
    output += "\"}}";    
    return output;
}

String sensor3dUpdateString(String featureName, float xValue, float yValue, float zValue, String units) {
    String output = "{\"topic\": \"";
    output += THINGS_NAMESPACE;
    output += "/";
    output += deviceId;
    output += "/things/twin/commands/modify\",\"headers\": {\"response-required\": true},";
    output += "\"path\": \"/features/" + featureName + "/properties/status\", \"value\": {\"xValue\": " + xValue;
    output += ", \"yValue\": ";
    output += yValue;
    output += ", \"zValue\": ";
    output += zValue;
    output += ", \"sensorUnits\": \"";
    output += units;
    output += "\"}}";    
    return output;
}
