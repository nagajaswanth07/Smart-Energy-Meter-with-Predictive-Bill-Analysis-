import math
import numpy as np

class SimpleNeuralNet:
    """
    Python implementation of the Edge AI model used on the ESP32.
    Architecture: 3 Inputs (V, I, P) -> 4 Hidden Neurons -> 1 Output (Anomaly Score)
    """
    def __init__(self):
        # Weights and Biases (Mirroring the C++ implementation)
        self.w_ih = np.array([
            [0.2, -0.3, 0.5, 0.1],
            [0.4, 0.8, -0.2, 0.6],
            [-0.5, 0.2, 0.7, -0.4]
        ])
        self.b_h = np.array([0.1, -0.2, 0.05, 0.15])
        
        self.w_ho = np.array([0.6, -0.4, 0.8, 0.3])
        self.b_o = -0.1

    def sigmoid(self, x):
        return 1.0 / (1.0 + np.exp(-x))

    def predict(self, voltage, current, power):
        # Normalization (mirroring C++ scaling)
        inputs = np.array([voltage / 250.0, current / 40.0, power / 10000.0])
        
        # Input to Hidden
        hidden = self.sigmoid(np.dot(inputs, self.w_ih) + self.b_h)
        
        # Hidden to Output
        output_sum = np.dot(hidden, self.w_ho) + self.b_o
        return self.sigmoid(output_sum)

class BillingEngine:
    """
    Python implementation of the Predictive Billing logic.
    """
    def __init__(self, tariff_rate=7.5):
        self.tariff_rate = tariff_rate

    def calculate_bill(self, energy_kWh):
        return energy_kWh * self.tariff_rate

    def predict_monthly_bill(self, energy_kWh, elapsed_hours):
        if elapsed_hours < 0.01:
            return 0.0
        
        energy_per_hour = energy_kWh / elapsed_hours
        projected_monthly_energy = energy_per_hour * (24 * 30)
        return projected_monthly_energy * self.tariff_rate
