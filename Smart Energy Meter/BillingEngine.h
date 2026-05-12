#ifndef BILLING_ENGINE_H
#define BILLING_ENGINE_H

#include <Arduino.h>

class BillingEngine {
private:
    float tariffRate;
    float currentBill;
    float predictedBill;
    unsigned long sessionStartTime;

public:
    BillingEngine(float rate = 7.5) {
        tariffRate = rate;
        currentBill = 0.0;
        predictedBill = 0.0;
        sessionStartTime = millis();
    }

    void update(float energy_kWh) {
        currentBill = energy_kWh * tariffRate;

        // Predictive Logic: 
        // Estimate the bill for a 30-day period based on usage rate since boot.
        unsigned long elapsedSeconds = (millis() - sessionStartTime) / 1000;
        
        if (elapsedSeconds > 60) { // Start predicting after 1 minute of data
            float energyPerSecond = energy_kWh / elapsedSeconds;
            float projectedMonthlyEnergy = energyPerSecond * (30.0f * 24.0f * 3600.0f);
            predictedBill = projectedMonthlyEnergy * tariffRate;
        } else {
            predictedBill = currentBill; // Not enough data yet
        }
    }

    float getCurrentBill() { return currentBill; }
    float getPredictedBill() { return predictedBill; }
    float getTariff() { return tariffRate; }
    void setTariff(float rate) { tariffRate = rate; }
};

#endif
