/**
 * Smart Energy Meter with Real-Time Monitoring & Predictive Bill Analysis
 * Main Application Firmware
 */

#define BLYNK_TEMPLATE_ID "TMPL6mm0IsdC8"
#define BLYNK_TEMPLATE_NAME "Energy Meter With Home Automation"
#define BLYNK_AUTH_TOKEN "RFOIahTNiBIxsmlfPhWRKRPWK-K7bWmH"

#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>

#include "SensorManager.h"
#include "BillingEngine.h"
#include "AIEngine.h"
#include "Dashboard.h"
#include "IoTManager.h"

// Credentials
const char* ssid = "Jaswaanth";
const char* pass = "7995533479";
const char* tsApiKey = "YQD5OMU0GJOGT8LS";

// Objects
WebServer server(80);
SensorManager sensorManager;
BillingEngine billingEngine(7.5); // $7.5 per kWh
SimpleNeuralNet aiModel;
IoTManager iotManager(tsApiKey);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Relay Pins
const int RELAY_PINS[] = {25, 26, 27, 14};
bool relayStates[] = {false, false, false, false};

// Timers
unsigned long lastUpdate = 0;

void handleRoot() {
    server.send(200, "text/html", DASHBOARD_HTML);
}

void handleData() {
    float aiScore = aiModel.predict(sensorManager.getVoltage(), sensorManager.getCurrent(), sensorManager.getPower());
    
    String json = "{";
    json += "\"voltage\":" + String(sensorManager.getVoltage()) + ",";
    json += "\"current\":" + String(sensorManager.getCurrent()) + ",";
    json += "\"power\":" + String(sensorManager.getPower()) + ",";
    json += "\"energy\":" + String(sensorManager.getEnergy()) + ",";
    json += "\"bill\":" + String(billingEngine.getCurrentBill()) + ",";
    json += "\"predict\":" + String(billingEngine.getPredictedBill()) + ",";
    json += "\"ai\":" + String(aiScore) + ",";
    json += "\"temp\":" + String(sensorManager.getTemp()) + ",";
    json += "\"hum\":" + String(sensorManager.getHumidity()) + ",";
    json += "\"relays\":[" + String(relayStates[0]) + "," + String(relayStates[1]) + "," + String(relayStates[2]) + "," + String(relayStates[3]) + "]";
    json += "}";
    server.send(200, "application/json", json);
}

void handleRelay() {
    if (server.hasArg("id")) {
        int id = server.arg("id").toInt() - 1;
        if (id >= 0 && id < 4) {
            relayStates[id] = !relayStates[id];
            digitalWrite(RELAY_PINS[id], relayStates[id] ? HIGH : LOW);
        }
    }
    server.send(200, "text/plain", "OK");
}

// Blynk Write handlers
BLYNK_WRITE(V10) { 
    relayStates[0] = param.asInt();
    digitalWrite(RELAY_PINS[0], relayStates[0] ? HIGH : LOW);
}
BLYNK_WRITE(V11) { 
    relayStates[1] = param.asInt();
    digitalWrite(RELAY_PINS[1], relayStates[1] ? HIGH : LOW);
}
BLYNK_WRITE(V12) { 
    relayStates[2] = param.asInt();
    digitalWrite(RELAY_PINS[2], relayStates[2] ? HIGH : LOW);
}
BLYNK_WRITE(V13) { 
    relayStates[3] = param.asInt();
    digitalWrite(RELAY_PINS[3], relayStates[3] ? HIGH : LOW);
}

void setup() {
    Serial.begin(115200);
    
    // LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Smart Meter Pro");

    // Relays
    for(int i=0; i<4; i++) {
        pinMode(RELAY_PINS[i], OUTPUT);
        digitalWrite(RELAY_PINS[i], LOW);
    }

    // WiFi
    WiFi.begin(ssid, pass);
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected");
        Serial.println(WiFi.localIP());
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP());
    } else {
        Serial.println("\nWiFi Failed. Starting AP Mode.");
        WiFi.softAP("SmartMeter_AP", "password123");
        lcd.setCursor(0, 1);
        lcd.print("AP: 192.168.4.1");
    }

    // Sensors & IoT
    sensorManager.begin();
    iotManager.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    // Routes
    server.on("/", handleRoot);
    server.on("/api/data", handleData);
    server.on("/api/relay", handleRelay);
    server.onNotFound(handleRoot);
    server.begin();
}

void loop() {
    server.handleClient();
    iotManager.run();

    if (millis() - lastUpdate > 2000) {
        sensorManager.update();
        billingEngine.update(sensorManager.getEnergy());
        
        // IoT Updates
        iotManager.updateBlynk(
            sensorManager.getVoltage(), sensorManager.getCurrent(), 
            sensorManager.getPower(), sensorManager.getEnergy(),
            billingEngine.getCurrentBill(), billingEngine.getPredictedBill(),
            sensorManager.getTemp(), sensorManager.getHumidity()
        );
        
        iotManager.updateThingSpeak(
            sensorManager.getVoltage(), sensorManager.getCurrent(),
            sensorManager.getPower(), sensorManager.getEnergy(),
            billingEngine.getCurrentBill()
        );

        lastUpdate = millis();
    }
}
