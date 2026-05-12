#ifndef IOT_MANAGER_H
#define IOT_MANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>

class IoTManager {
private:
    String tsApiKey;
    unsigned long lastTsUpdate;

public:
    IoTManager(String apiKey) {
        tsApiKey = apiKey;
        lastTsUpdate = 0;
    }

    void begin(const char* blynkAuth, const char* ssid, const char* pass) {
        Blynk.begin(blynkAuth, ssid, pass);
    }

    void run() {
        Blynk.run();
    }

    void updateBlynk(float voltage, float current, float power, float energy, float bill, float predict, float temp, float humidity) {
        Blynk.virtualWrite(V0, voltage);
        Blynk.virtualWrite(V1, current);
        Blynk.virtualWrite(V2, power);
        Blynk.virtualWrite(V3, energy);
        Blynk.virtualWrite(V4, bill);
        Blynk.virtualWrite(V5, predict);
        Blynk.virtualWrite(V6, temp);
        Blynk.virtualWrite(V7, humidity);
    }

    void updateThingSpeak(float voltage, float current, float power, float energy, float bill) {
        // ThingSpeak has a 15-second rate limit
        if (millis() - lastTsUpdate > 15000) {
            if (WiFi.status() == WL_CONNECTED) {
                HTTPClient http;
                String url = "http://api.thingspeak.com/update?api_key=" + tsApiKey +
                             "&field1=" + String(voltage) +
                             "&field2=" + String(current) +
                             "&field3=" + String(power) +
                             "&field4=" + String(energy) +
                             "&field5=" + String(bill);
                
                http.begin(url);
                int httpCode = http.GET();
                if (httpCode > 0) {
                    Serial.println("ThingSpeak Update Successful");
                }
                http.end();
                lastTsUpdate = millis();
            }
        }
    }
};

#endif
