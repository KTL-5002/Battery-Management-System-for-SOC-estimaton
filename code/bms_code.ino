// =============================================================
// FINAL BMS CODE (INA219 + DS18B20 + LCD + SOC + 2 RELAYS + BUZZER)
// Load Relay  -> GPIO 26
// Charge Relay -> GPIO 27
// Buzzer -> GPIO 14 (active buzzer)
// =============================================================

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>

// ---------------- Object Instantiation ----------------
Adafruit_INA219 ina219;
OneWire oneWire(4);               // DS18B20 data pin
DallasTemperature tempSensor(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences prefs;

// ---------------- Pin Definitions ----------------
#define RELAY_LOAD   26   // Load relay
#define RELAY_CHARGE 27   // Charger relay
#define BUZZER_PIN   14   // Active buzzer

// ---------------- Battery & BMS Settings ----------------
const float BAT_CAPACITY_mAh = 2000.0;
const float R_SHUNT = 0.05f;
const int OFFSET_SAMPLES = 200;
const unsigned long SAMPLE_MS = 1000;

// Safety limits
const float TEMP_CUTOFF  = 60.0;
const float TEMP_RESUME  = 50.0;

const float UV_CUTOFF = 3.00;
const float UV_RESUME = 3.20;

const float OV_CUTOFF = 4.25;
const float OV_RESUME = 4.15;

// ---------------- State Variables ----------------
float offset_mV = 0;
float ema_mV = 0;
float stored_mAh = -1;

unsigned long lastSample = 0;

bool lastLoadOFF   = false;
bool lastChargeOFF = false;

// ---------------- Helper Functions ----------------
float mV_to_mA(float mv) { return mv / R_SHUNT; }

float OCV_to_SOC(float V) {
  if (V >= 4.20) return 1.0;
  if (V <= 3.00) return 0.0;
  return (V - 3.00) / (4.20 - 3.00);
}

// Buzzer alert helper
void beep(int count, int duration = 150, int gap = 120) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
    delay(gap);
  }
}

// Offset calibration (run with no load or charger)
void calibrateOffset() {
  Serial.println("Calibrating offset…");

  float sum = 0;
  for (int i = 0; i < OFFSET_SAMPLES; i++) {
    sum += ina219.getShuntVoltage_mV();
    delay(5);
  }
  offset_mV = sum / OFFSET_SAMPLES;
  ema_mV = 0;

  Serial.print("Offset set to: ");
  Serial.println(offset_mV, 6);
}

// =============================================================
//                          SETUP
// =============================================================
void setup() {
  Serial.begin(115200);
  delay(300);

  // Relay + Buzzer Setup
  pinMode(RELAY_LOAD, OUTPUT);
  pinMode(RELAY_CHARGE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Active LOW relays: HIGH = OFF
  digitalWrite(RELAY_LOAD, HIGH);
  digitalWrite(RELAY_CHARGE, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  // I2C + Sensors
  Wire.begin(21, 22);
  tempSensor.begin();
  tempSensor.setResolution(10);

  lcd.init();
  lcd.backlight();

  // INA219 startup
  if (!ina219.begin()) {
    Serial.println("INA219 NOT FOUND!");
    while (1);
  }

  ina219.setCalibration_32V_2A();

  prefs.begin("bms", false);
  stored_mAh = prefs.getFloat("stored_mAh", -1);
  prefs.end();

  // Calibrate offset (NO load, NO charger)
  calibrateOffset();

  // If first boot, estimate SOC from OCV
  if (stored_mAh < 0) {
    float V_init = ina219.getBusVoltage_V();
    stored_mAh = BAT_CAPACITY_mAh * OCV_to_SOC(V_init);
  }

  lastSample = millis();

  // Startup beep
  beep(1, 300);
}

// =============================================================
//                          LOOP
// =============================================================
void loop() {

  // ---------------- Read Sensors ----------------
  float V = ina219.getBusVoltage_V();
  float raw_mV = ina219.getShuntVoltage_mV();
  float corrected_mV = raw_mV - offset_mV;

  // EMA smoothing
  ema_mV = 0.2 * corrected_mV + 0.8 * ema_mV;

  // Current (positive = charging, negative = discharging)
  float I_mA = mV_to_mA(ema_mV);

  // Temperature
  tempSensor.requestTemperatures();
  delay(200);
  float tempC = tempSensor.getTempCByIndex(0);
  if (tempC < -40 || tempC > 125) tempC = -99;

  // ---------------- Coulomb Counting ----------------
  unsigned long now = millis();
  if (now - lastSample >= SAMPLE_MS) {
    float dt_h = (now - lastSample) / 3600000.0;
    stored_mAh += I_mA * dt_h;
    stored_mAh = constrain(stored_mAh, 0.0f, BAT_CAPACITY_mAh);
    lastSample = now;
  }

  float SOC = (stored_mAh / BAT_CAPACITY_mAh) * 100.0;

  // =============================================================
  //                          RELAY LOGIC
  // =============================================================

  bool loadOFF = false;
  bool chargeOFF = false;

  // -------- LOAD RELAY TRIGGERS --------
  if (tempC > TEMP_CUTOFF) loadOFF = true;
  if (V < UV_CUTOFF)       loadOFF = true;

  // Safe restore
  if (V > UV_RESUME && tempC < TEMP_RESUME) loadOFF = false;

  digitalWrite(RELAY_LOAD, loadOFF ? HIGH : LOW);

  // -------- CHARGER RELAY TRIGGERS --------
  if (tempC > TEMP_CUTOFF) chargeOFF = true;
  if (V > OV_CUTOFF)       chargeOFF = true;

  // Safe restore
  if (V < OV_RESUME && tempC < TEMP_RESUME) chargeOFF = false;

  digitalWrite(RELAY_CHARGE, chargeOFF ? HIGH : LOW);

  // =============================================================
  //                  RELAY STATE CHANGE ALERTS
  // =============================================================

  if (loadOFF != lastLoadOFF) {
    if (loadOFF)
      Serial.println(">>> LOAD RELAY ACTIVATED (Load Disconnected)");
    else
      Serial.println(">>> LOAD RELAY RESTORED (Load Connected)");

    beep(loadOFF ? 2 : 1);  
    lastLoadOFF = loadOFF;
  }

  if (chargeOFF != lastChargeOFF) {
    if (chargeOFF)
      Serial.println(">>> CHARGER RELAY ACTIVATED (Charging Disabled)");
    else
      Serial.println(">>> CHARGER RELAY RESTORED (Charging Enabled)");

    beep(chargeOFF ? 3 : 1);
    lastChargeOFF = chargeOFF;
  }

  // =============================================================
  //                     SERIAL PRINT
  // =============================================================
  Serial.print("V:"); Serial.print(V,3);
  Serial.print(" I:"); Serial.print(I_mA,2);
  Serial.print(" SOC:"); Serial.print(SOC,1);
  Serial.print(" Temp:"); Serial.print(tempC);

  Serial.print(" LoadRelay:");
  Serial.print(loadOFF ? "OFF" : "ON");

  Serial.print(" ChargeRelay:");
  Serial.println(chargeOFF ? "OFF" : "ON");

  // =============================================================
  //                     LCD DISPLAY
  // =============================================================
  lcd.setCursor(0,0);
  lcd.print("V:"); lcd.print(V,2);
  lcd.print(" I:");
  lcd.print(I_mA/1000.0,3);
  
  
  }