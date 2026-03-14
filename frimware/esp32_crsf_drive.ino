#include <AlfredoCRSF.h>
#include <HardwareSerial.h>

// ================= PIN DEFINITIONS =================
#define RX_PIN 16
#define TX_PIN 17

// Motor Driver 1 (LEFT)
#define L_RPWM 32 
#define L_LPWM 33 

// Motor Driver 2 (RIGHT)
#define R_RPWM 26 
#define R_LPWM 27

// ================= TUNING SETTINGS =================
#define BAUD_RATE 420000 
#define DEADBAND 0       // 5% stick deadzone
#define EXPO_AMOUNT 0.6     // 0.6 = Soft center, precision aiming
#define SMOOTHING 0.75      // Lower = Smoother/Slower ramp up
#define MIN_PWR 20          // Minimum PWM to overcome friction
#define MAX_PWR 255         // Max speed

// --- CHANGED: REMOVED FIXED DEFINE, ADDED CHANNEL DEFINITION ---
// #define STEERING_WEIGHTAGE 0.6  <-- DELETED
#define TUNING_CHANNEL 10    // Channel mapped to your Scroll Wheel/Slider on Radio

AlfredoCRSF crsf;

// Variables for Smoothing
float targetSpeedL = 0;
float targetSpeedR = 0;
float currentSpeedL = 0;
float currentSpeedR = 0;

// Variable for Dynamic Weightage
float currentSteeringWeight = 0.6; // Default value

// Forward declaration of helper functions
float applyExpo(float input, float expo);
float lerpSmooth(float start, float end, float amount);
int speedToPWM(float speedVal);
void setLeftMotor(float speedVal);
void setRightMotor(float speedVal);
void stopMotors();

void setup() {

  pinMode(L_RPWM, OUTPUT); pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT); pinMode(R_LPWM, OUTPUT);

  Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  crsf.begin(Serial2);
}

void loop() {
  crsf.update();

  // 1. FAILSAFE
  int armSwitch = crsf.getChannel(5);
  if (!crsf.isLinkUp() || armSwitch < 1500) {
    stopMotors();
    currentSpeedL = 0;
    currentSpeedR = 0;
    return;
  }

  // 2. READ INPUTS (-1.0 to 1.0)
  float throttleRaw = (crsf.getChannel(2) - 1500) / 500.0;
  float steeringRaw = (crsf.getChannel(1) - 1500) / 500.0;

  // --- NEW: READ TUNING CHANNEL (0.0 to 1.0) ---
  // CRSF channels send 1000 to 2000. 
  // We subtract 1000 to get 0-1000, then divide by 1000.0 to get 0.0-1.0
  float rawTuning = crsf.getChannel(TUNING_CHANNEL);
  
  // Safety constrain to ensure we don't go out of bounds
  rawTuning = constrain(rawTuning, 1000.0, 2000.0);
  
  // Map to 0.0 -> 1.0 range
  currentSteeringWeight = (rawTuning - 1000.0) / 1000.0;

  // Optional: Set a minimum limit so steering never completely disappears (e.g., min 0.1)
  if (currentSteeringWeight < 0.1) currentSteeringWeight = 0.1;

  // 3. APPLY DEADBAND
  if (abs(throttleRaw) < DEADBAND) throttleRaw = 0;
  if (abs(steeringRaw) < DEADBAND) steeringRaw = 0;

  // 4. APPLY EXPO
  float throttle = applyExpo(throttleRaw, EXPO_AMOUNT);
  float steering = applyExpo(steeringRaw, EXPO_AMOUNT);

  // 5. MIXING (Uses currentSteeringWeight instead of fixed define)
  targetSpeedL = throttle + (steering * currentSteeringWeight);
  targetSpeedR = throttle - (steering * currentSteeringWeight);

  targetSpeedL = constrain(targetSpeedL, -1.0, 1.0);
  targetSpeedR = constrain(targetSpeedR, -1.0, 1.0);

  // 6. SMOOTHING 
  currentSpeedL = lerpSmooth(currentSpeedL, targetSpeedL, SMOOTHING);
  currentSpeedR = lerpSmooth(currentSpeedR, targetSpeedR, SMOOTHING);

  // 7. DRIVE
  setLeftMotor(currentSpeedL);
  setRightMotor(currentSpeedR);

  // DEBUGGING: Uncomment to see the value changing in Serial Monitor
  // Serial.print("Weight: "); Serial.println(currentSteeringWeight);
}

// ================= HELPER FUNCTIONS =================

float applyExpo(float input, float expo) {
  return (input * (1 - expo)) + (input * input * input * expo);
}

float lerpSmooth(float start, float end, float amount) {
  return start + ((end - start) * amount);
}

void setLeftMotor(float speedVal) {
  int pwm = speedToPWM(speedVal);
  if (pwm > 0) {
    analogWrite(L_RPWM, pwm);
    analogWrite(L_LPWM, 0);
  } else if (pwm < 0) {
    analogWrite(L_RPWM, 0);
    analogWrite(L_LPWM, abs(pwm));
  } else {
    analogWrite(L_RPWM, 0);
    analogWrite(L_LPWM, 0);
  }
}

void setRightMotor(float speedVal) {
  int pwm = speedToPWM(speedVal);
  if (pwm > 0) {
    analogWrite(R_RPWM, pwm);
    analogWrite(R_LPWM, 0);
  } else if (pwm < 0) {
    analogWrite(R_RPWM, 0);
    analogWrite(R_LPWM, abs(pwm));
  } else {
    analogWrite(R_RPWM, 0);
    analogWrite(R_LPWM, 0);
  }
}

void stopMotors() {
  analogWrite(L_RPWM, 0); analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0); analogWrite(R_LPWM, 0);
}

int speedToPWM(float speedVal) {
  if (abs(speedVal) < 0.01) return 0; 
  int pwm = map(abs(speedVal) * 1000, 0, 1000, MIN_PWR, MAX_PWR);
  if (speedVal < 0) pwm = -pwm;
  return pwm;
}
