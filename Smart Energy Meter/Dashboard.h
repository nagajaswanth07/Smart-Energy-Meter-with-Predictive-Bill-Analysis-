#ifndef DASHBOARD_H
#define DASHBOARD_H

const char* DASHBOARD_HTML = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>EV Smart Meter Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg: #0b0e14;
            --card-bg: rgba(255, 255, 255, 0.05);
            --accent: #00f2ff;
            --accent-glow: rgba(0, 242, 255, 0.3);
            --text: #ffffff;
            --danger: #ff4757;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            background: var(--bg);
            color: var(--text);
            font-family: 'Outfit', sans-serif;
            padding: 20px;
            overflow-x: hidden;
        }
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 30px;
            padding: 20px;
            background: var(--card-bg);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            border: 1px solid rgba(255,255,255,0.1);
        }
        .header h1 { font-weight: 600; font-size: 1.5rem; letter-spacing: 1px; }
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .card {
            background: var(--card-bg);
            backdrop-filter: blur(10px);
            padding: 25px;
            border-radius: 24px;
            border: 1px solid rgba(255,255,255,0.1);
            transition: transform 0.3s ease, border-color 0.3s ease;
        }
        .card:hover { transform: translateY(-5px); border-color: var(--accent); }
        .card h3 { font-size: 0.9rem; color: #888; margin-bottom: 15px; text-transform: uppercase; letter-spacing: 1px; }
        .card .value { font-size: 2.2rem; font-weight: 600; margin-bottom: 5px; }
        .card .unit { font-size: 1rem; color: #888; }
        .card.accent { border-left: 4px solid var(--accent); }
        .card.ai { background: linear-gradient(135deg, rgba(0, 242, 255, 0.1), transparent); }
        
        .chart-container {
            background: var(--card-bg);
            padding: 25px;
            border-radius: 24px;
            margin-bottom: 30px;
            border: 1px solid rgba(255,255,255,0.1);
        }
        .controls {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
        }
        .btn {
            background: var(--card-bg);
            border: 1px solid rgba(255,255,255,0.1);
            color: white;
            padding: 15px;
            border-radius: 12px;
            cursor: pointer;
            font-family: inherit;
            font-weight: 600;
            transition: all 0.3s ease;
        }
        .btn.active { background: var(--accent); color: var(--bg); box-shadow: 0 0 20px var(--accent-glow); }
        .btn:hover:not(.active) { background: rgba(255,255,255,0.15); }

        .ai-status {
            display: flex;
            align-items: center;
            gap: 10px;
            margin-top: 10px;
            font-size: 0.9rem;
        }
        .dot { width: 8px; height: 8px; border-radius: 50%; background: #444; }
        .dot.active { background: #00ff88; box-shadow: 0 0 10px #00ff88; }

        @media (max-width: 600px) {
            .grid { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>Smart Energy Meter <span style="color: var(--accent);">Pro</span></h1>
        <div id="connection-status" style="font-size: 0.8rem; color: #888;">Connected</div>
    </div>

    <div class="grid">
        <div class="card accent">
            <h3>Total Power</h3>
            <div class="value" id="val-power">0.00</div>
            <span class="unit">Watts</span>
        </div>
        <div class="card">
            <h3>Voltage</h3>
            <div class="value" id="val-voltage">0.0</div>
            <span class="unit">Volts AC</span>
        </div>
        <div class="card">
            <h3>Current Bill</h3>
            <div class="value" id="val-bill" style="color: #00ff88;">$0.00</div>
            <span class="unit">Tariff: $7.5/kWh</span>
        </div>
        <div class="card">
            <h3>Predicted Bill</h3>
            <div class="value" id="val-predict" style="color: var(--accent);">$0.00</div>
            <span class="unit">Monthly Estimate</span>
        </div>
        <div class="card ai">
            <h3>Edge AI Anomaly Score</h3>
            <div class="value" id="val-ai">0%</div>
            <div class="ai-status">
                <div class="dot active"></div>
                <span id="ai-text">Monitoring normal patterns</span>
            </div>
        </div>
        <div class="card">
            <h3>Environment</h3>
            <div class="value" id="val-env">0°C</div>
            <span class="unit" id="val-hum">Humidity: 0%</span>
        </div>
    </div>

    <div class="chart-container">
        <h3>Power Consumption History</h3>
        <canvas id="powerChart" height="100"></canvas>
    </div>

    <div class="header">
        <h1>Load Control</h1>
    </div>
    <div class="controls">
        <button class="btn" id="btn-1" onclick="toggleRelay(1)">Load 1</button>
        <button class="btn" id="btn-2" onclick="toggleRelay(2)">Load 2</button>
        <button class="btn" id="btn-3" onclick="toggleRelay(3)">Load 3</button>
        <button class="btn" id="btn-4" onclick="toggleRelay(4)">Load 4</button>
    </div>

    <script>
        const ctx = document.getElementById('powerChart').getContext('2d');
        const powerChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Power (W)',
                    data: [],
                    borderColor: '#00f2ff',
                    backgroundColor: 'rgba(0, 242, 255, 0.1)',
                    borderWidth: 2,
                    fill: true,
                    tension: 0.4
                }]
            },
            options: {
                responsive: true,
                scales: {
                    y: { beginAtZero: true, grid: { color: 'rgba(255,255,255,0.05)' }, ticks: { color: '#888' } },
                    x: { grid: { display: false }, ticks: { color: '#888' } }
                },
                plugins: { legend: { display: false } }
            }
        });

        let relayStates = [false, false, false, false];

        function updateData() {
            fetch('/api/data')
                .then(res => res.json())
                .then(data => {
                    document.getElementById('val-power').innerText = data.power.toFixed(2);
                    document.getElementById('val-voltage').innerText = data.voltage.toFixed(1);
                    document.getElementById('val-bill').innerText = '$' + data.bill.toFixed(2);
                    document.getElementById('val-predict').innerText = '$' + data.predict.toFixed(2);
                    
                    const aiScore = data.ai * 100;
                    document.getElementById('val-ai').innerText = aiScore.toFixed(1) + '%';
                    
                    if (aiScore > 70) {
                        document.getElementById('val-ai').style.color = 'var(--danger)';
                        document.getElementById('ai-text').innerText = 'Anomaly Detected!';
                    } else {
                        document.getElementById('val-ai').style.color = 'white';
                        document.getElementById('ai-text').innerText = 'Monitoring normal patterns';
                    }

                    document.getElementById('val-env').innerText = data.temp.toFixed(1) + '°C';
                    document.getElementById('val-hum').innerText = 'Humidity: ' + data.hum.toFixed(0) + '%';

                    // Update Chart
                    const now = new Date().toLocaleTimeString();
                    powerChart.data.labels.push(now);
                    powerChart.data.datasets[0].data.push(data.power);
                    if (powerChart.data.labels.length > 20) {
                        powerChart.data.labels.shift();
                        powerChart.data.datasets[0].data.shift();
                    }
                    powerChart.update();

                    // Update Relay Buttons
                    for(let i=1; i<=4; i++) {
                        const btn = document.getElementById('btn-' + i);
                        if (data.relays[i-1]) btn.classList.add('active');
                        else btn.classList.remove('active');
                    }
                })
                .catch(err => {
                    document.getElementById('connection-status').innerText = 'Disconnected';
                    document.getElementById('connection-status').style.color = 'var(--danger)';
                });
        }

        function toggleRelay(id) {
            fetch('/api/relay?id=' + id)
                .then(res => res.text())
                .then(() => updateData());
        }

        setInterval(updateData, 2000);
        updateData();
    </script>
</body>
</html>
)=====";

#endif
