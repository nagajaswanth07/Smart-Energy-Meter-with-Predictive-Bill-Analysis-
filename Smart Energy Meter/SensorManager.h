#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <ACS712.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

class SensorManager {
private:
    PZEM004Tv30 pzem;
    ACS712 acs1, acs2, acs3, acs4;
    DHT dht;

    float voltage, current, power, energy, frequency, pf;
    float currentL1, currentL2, currentL3, currentL4;
    float temp, humidity;

    const int calibration_factor = 126;

    float readCalibratedCurrent(ACS712 &sensor) {
        float average = 0;
        for (int i = 0; i < 20; i++) {
            average += sensor.mA_AC();
        }
        float mA = (abs(average / 20.0) - calibration_factor);
        if (mA <= 10) mA = 0; // Filter noise
        return mA / 1000.0; // Convert to Amps
    }

public:
    // Initialize sensors: Serial2 (16, 17) for PZEM, Analog pins for ACS
    SensorManager() : 
        pzem(Serial2, 16, 17),
        acs1(34, 3.3, 4095, 100), // 100mV/A for ACS712-20A or similar
        acs2(35, 3.3, 4095, 100),
        acs3(32, 3.3, 4095, 100),
        acs4(33, 3.3, 4095, 100),
        dht(DHTPIN, DHTTYPE)
    {}

    void begin() {
        dht.begin();
        Serial.println("Sensors initialized.");
    }

    void update() {
        // Read PZEM data
        voltage = pzem.voltage();
        current = pzem.current();
        power = pzem.power();
        energy = pzem.energy();
        frequency = pzem.frequency();
        pf = pzem.pf();

        // Read ACS712 data (individual loads)
        currentL1 = readCalibratedCurrent(acs1);
        currentL2 = readCalibratedCurrent(acs2);
        currentL3 = readCalibratedCurrent(acs3);
        currentL4 = readCalibratedCurrent(acs4);

        // Read DHT data
        temp = dht.readTemperature();
        humidity = dht.readHumidity();

        // Check for NaN
        if (isnan(voltage)) voltage = 0.0;
        if (isnan(current)) current = 0.0;
        if (isnan(power)) power = 0.0;
        if (isnan(temp)) temp = 25.0; // Default fallback
    }

    // Getters
    float getVoltage() { return voltage; }
    float getCurrent() { return current; }
    float getPower() { return power; }
    float getEnergy() { return energy; }
    float getFrequency() { return frequency; }
    float getPF() { return pf; }
    float getTemp() { return temp; }
    float getHumidity() { return humidity; }
    
    float getCurrentL1() { return currentL1; }
    float getCurrentL2() { return currentL2; }
    float getCurrentL3() { return currentL3; }
    float getCurrentL4() { return currentL4; }
};

#endif
