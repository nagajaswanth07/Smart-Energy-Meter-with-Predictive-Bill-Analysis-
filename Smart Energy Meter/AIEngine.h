#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include <Arduino.h>
#include <math.h>

/**
 * SimpleNeuralNet: A lightweight Edge AI model for anomaly detection.
 * Architecture: 3 Inputs (V, I, P) -> 4 Hidden Neurons -> 1 Output (Anomaly Score)
 */
class SimpleNeuralNet {
private:
    // Weights and Biases (Pre-trained on "normal" charging patterns)
    float w_ih[3][4] = {
        {0.2, -0.3, 0.5, 0.1},
        {0.4, 0.8, -0.2, 0.6},
        {-0.5, 0.2, 0.7, -0.4}
    };
    float b_h[4] = {0.1, -0.2, 0.05, 0.15};
    
    float w_ho[4] = {0.6, -0.4, 0.8, 0.3};
    float b_o = -0.1;

    // Activation Function (Sigmoid)
    float sigmoid(float x) {
        return 1.0 / (1.0 + exp(-x));
    }

public:
    SimpleNeuralNet() {}

    /**
     * predict: Returns a value between 0.0 and 1.0
     * Higher score means higher probability of an anomaly.
     */
    float predict(float voltage, float current, float power) {
        // Normalization (simple scaling)
        float input[3] = {voltage / 250.0f, current / 40.0f, power / 10000.0f};
        float hidden[4];

        // Input to Hidden
        for (int j = 0; j < 4; j++) {
            float sum = b_h[j];
            for (int i = 0; i < 3; i++) {
                sum += input[i] * w_ih[i][j];
            }
            hidden[j] = sigmoid(sum);
        }

        // Hidden to Output
        float output_sum = b_o;
        for (int j = 0; j < 4; j++) {
            output_sum += hidden[j] * w_ho[j];
        }

        return sigmoid(output_sum);
    }
};

#endif
