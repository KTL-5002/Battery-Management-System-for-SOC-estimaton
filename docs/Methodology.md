# Methodology

## 🧩 System Components
- ESP32 (Microcontroller)
- INA219 (Current Sensor)
- DS18B20 (Temperature Sensor)
- Relay Module
- LCD Display

## 🔄 System Workflow
1. Sensors measure voltage, current, and temperature
2. Data sent to ESP32
3. SOC calculated using Coulomb Counting
4. OCV used to correct SOC drift
5. Thresholds checked
6. Relays activated if unsafe

## ⚡ SOC Estimation

### Coulomb Counting
Tracks current over time to estimate charge.

### OCV Method
Uses battery voltage when idle to correct SOC.

## 🛡️ Safety Logic
- Over-temperature → Cutoff
- Under-voltage → Load disconnected
- Over-voltage → Charging stopped

## 🧠 Control Logic Insight
The system uses threshold-based decision making:
- If temperature > 60°C → shutdown
- If voltage < 3.0V → disconnect load

This ensures real-time protection without complex computation.

