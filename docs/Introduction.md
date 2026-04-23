# Introduction

## 🔋 What is this project?
This project implements a Battery Management System (BMS) using ESP32 to estimate the State of Charge (SoC) of a Lithium-ion battery.

## ❗ Problem Statement
Lithium-ion batteries require continuous monitoring to prevent:
- Overcharging
- Deep discharge
- Overheating

Without proper management, battery life reduces and safety risks increase.

## 💡 Proposed Solution
This system uses:
- Coulomb Counting
- Open Circuit Voltage (OCV)

to estimate SoC in a simple and cost-effective way.

## 🎯 Objectives
- Estimate SoC accurately
- Monitor voltage, current, temperature
- Provide safety cutoffs using relays
- Display real-time data