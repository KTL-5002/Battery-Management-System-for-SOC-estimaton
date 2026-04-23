\# 🔋 Battery Management System (BMS) for SoC Estimation



\## 📌 Overview



This project implements a \*\*low-cost Battery Management System (BMS)\*\* using an ESP32 to estimate the \*\*State of Charge (SoC)\*\* of a Lithium-ion battery.



The system combines:



\* \*\*Coulomb Counting\*\* (charge tracking)

\* \*\*Open Circuit Voltage (OCV)\*\* (drift correction)



to achieve a balance between \*\*accuracy and computational simplicity\*\*.



\---



\## ⚙️ Features



\* 📊 Real-time monitoring of:



&#x20; \* Voltage

&#x20; \* Current

&#x20; \* Temperature

\* 🔋 SoC estimation using hybrid method (CC + OCV)

\* ⚡ Dual relay protection:



&#x20; \* Load cutoff

&#x20; \* Charger cutoff

\* 📺 LCD display for live data

\* 🔔 Buzzer alerts for safety events

\* 📡 Serial/Wireless logging via ESP32



\---



\## 🧠 Core Concept



Instead of using computationally heavy techniques like Kalman Filters, this system relies on:



\* \*\*Coulomb Counting\*\*

&#x20; → Integrates current over time to estimate charge



\* \*\*OCV Correction\*\*

&#x20; → Adjusts SoC when battery is idle



👉 This hybrid approach reduces drift while keeping the system lightweight.



\---



\## 🛠️ Hardware Components



\* ESP32 Microcontroller

\* INA219 Current Sensor

\* DS18B20 Temperature Sensor

\* 2-Channel Relay Module

\* 16x2 LCD Display

\* Li-ion Battery



\---



\## 🔌 System Workflow



1\. Sensors measure voltage, current, and temperature

2\. ESP32 processes sensor data

3\. SoC is calculated using Coulomb Counting

4\. OCV correction applied when current ≈ 0

5\. Safety thresholds are checked

6\. Relays disconnect load/charger if unsafe

7\. Data displayed on LCD and logged



\---



\## 🚨 Safety Features



\* 🔥 Over-temperature protection

\* ⚡ Over-voltage protection

\* 🔋 Under-voltage protection

\* 🔔 Audible alert system



\---



\## 📊 Results Summary



\* Voltage error: \*\*< 40 mV\*\*

\* Stable SoC estimation with minimal drift

\* Accurate current measurement after calibration

\* Reliable relay response under fault conditions



\---



\## 📚 Documentation



Detailed explanations are available here:



\* 📖 \[Introduction](docs/introduction.md)

\* ⚙️ \[Methodology](docs/methodology.md)

\* 📊 \[Results](docs/results.md)

\* 🔌 \[Hardware Connections](hardware/connections.md)



\---



\## 📂 Project Structure



```

battery-management-system/

│── README.md

│── docs/

│   ├── introduction.md

│   ├── methodology.md

│   ├── results.md

│── code/

│   └── bms\_esp32.ino

│── hardware/

│   └── connections.md

│── images/

```



\---



\## 🚀 How to Run



\### 1️⃣ Upload Code



\* Open Arduino IDE

\* Select ESP32 board

\* Upload `code/bms\_esp32.ino`



\### 2️⃣ Connect Hardware



Follow:

👉 \[Hardware Connections](hardware/connections.md)



\### 3️⃣ Power the System



\* Connect battery

\* Power ESP32



\### 4️⃣ Monitor Output



\* LCD Display

\* Serial Monitor (115200 baud)



\---



\## ⚠️ Limitations



\* No multi-cell balancing

\* No battery health (SOH) estimation

\* Fixed threshold-based protection



\---



\## 🚀 Future Improvements



\* Multi-cell battery support

\* Mobile app monitoring (Bluetooth/Wi-Fi)

\* MOSFET-based switching

\* Advanced SoC estimation (Kalman Filter / ML)



\---



\## 📸 System Preview



\*Add images in the `images/` folder and link here\*



\---



\## 📜 License



This project is open-source and available for educational and research purposes.



