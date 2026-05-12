import matplotlib.pyplot as plt
import numpy as np
import os
from meter_logic import SimpleNeuralNet, BillingEngine

# Create assets directory if it doesn't exist
os.makedirs('assets', exist_ok=True)

# Set style
plt.style.use('dark_background')
ACCENT_COLOR = '#00f2ff'
DANGER_COLOR = '#ff4757'

def generate_normal_profile():
    print("Generating Normal Power Profile...")
    time = np.linspace(0, 24, 100) # 24 hours
    # Simulated power usage: higher in evening and morning
    base_power = 500 + 1000 * np.sin(np.pi * (time - 6) / 12)**2 
    noise = np.random.normal(0, 50, 100)
    power = base_power + noise
    
    plt.figure(figsize=(10, 5))
    plt.plot(time, power, color=ACCENT_COLOR, linewidth=2, label='Power (W)')
    plt.fill_between(time, power, alpha=0.1, color=ACCENT_COLOR)
    plt.title('24-Hour Normal Energy Consumption Profile', fontsize=14, pad=20, color='white')
    plt.xlabel('Time (Hours)', color='#888')
    plt.ylabel('Power (Watts)', color='#888')
    plt.grid(alpha=0.1)
    plt.savefig('assets/normal_profile.png', dpi=150, bbox_inches='tight')
    plt.close()

def generate_anomaly_detection():
    print("Generating AI Anomaly Detection Demo...")
    nn = SimpleNeuralNet()
    time = np.linspace(0, 10, 100)
    
    # Normal data for first 7 seconds, then a spike
    voltage = 230 + np.random.normal(0, 2, 100)
    current = np.concatenate([
        10 + np.random.normal(0, 0.5, 70), # Normal 10A
        [45] * 10,                        # Anomaly 45A (Overlimit)
        10 + np.random.normal(0, 0.5, 20)  # Back to normal (if system didn't trip)
    ])
    power = voltage * current
    
    scores = [nn.predict(v, i, p) for v, i, p in zip(voltage, current, power)]
    
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)
    
    # Plot Power
    ax1.plot(time, power, color='#888', label='Power (W)')
    ax1.set_title('AI Edge Anomaly Detection', fontsize=14, pad=20)
    ax1.set_ylabel('Power (Watts)')
    ax1.grid(alpha=0.1)
    
    # Plot AI Score
    ax2.plot(time, scores, color=ACCENT_COLOR, linewidth=2, label='AI Confidence Score')
    ax2.fill_between(time, scores, 0.7, where=(np.array(scores) >= 0.7), color=DANGER_COLOR, alpha=0.5, label='Anomaly Flag')
    ax2.axhline(y=0.7, color=DANGER_COLOR, linestyle='--', alpha=0.5)
    ax2.set_ylabel('Anomaly Confidence')
    ax2.set_xlabel('Time (Seconds)')
    ax2.set_ylim(0, 1.1)
    ax2.legend()
    ax2.grid(alpha=0.1)
    
    plt.tight_layout()
    plt.savefig('assets/anomaly_detection.png', dpi=150, bbox_inches='tight')
    plt.close()

def generate_billing_prediction():
    print("Generating Predictive Billing Chart...")
    billing = BillingEngine(tariff_rate=7.5)
    days = np.arange(1, 31)
    # Simulate slightly increasing daily usage
    daily_energy = 5 + 0.1 * days + np.random.normal(0, 0.5, 30)
    cumulative_energy = np.cumsum(daily_energy)
    actual_bill = billing.calculate_bill(cumulative_energy)
    
    # Current point: Day 15
    current_day = 15
    elapsed_hours = current_day * 24
    predicted_bill = billing.predict_monthly_bill(cumulative_energy[current_day-1], elapsed_hours)
    
    plt.figure(figsize=(10, 5))
    plt.plot(days[:current_day], actual_bill[:current_day], color=ACCENT_COLOR, linewidth=3, label='Actual Bill (to date)')
    
    # Projection line
    plt.plot([current_day, 30], [actual_bill[current_day-1], predicted_bill], color=ACCENT_COLOR, linestyle='--', alpha=0.5, label='Predicted Monthly Bill')
    plt.scatter([30], [predicted_bill], color=ACCENT_COLOR, s=100, edgecolors='white', zorder=5)
    plt.annotate(f'Predicted: ${predicted_bill:.2f}', (30, predicted_bill), xytext=(-120, 10), textcoords='offset points', color=ACCENT_COLOR, fontweight='bold')
    
    plt.title('Predictive Billing Analysis (30-Day Forecast)', fontsize=14, pad=20)
    plt.xlabel('Day of Month')
    plt.ylabel('Cost ($)')
    plt.xticks(np.arange(0, 31, 5))
    plt.grid(alpha=0.1)
    plt.legend()
    
    plt.savefig('assets/billing_prediction.png', dpi=150, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    generate_normal_profile()
    generate_anomaly_detection()
    generate_billing_prediction()
    print("All assets generated in Simulation/assets/")
