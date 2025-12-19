/*
 * NEMA14 Stepper Motor Control with DRV8825
 * * Hardware Connections:
 * - Arduino GND -> DRV8825 GND
 * - Arduino Pin 2 -> DRV8825 DIR
 * - Arduino Pin 3 -> DRV8825 STEP
 * - Arduino Pin 13 -> Built-in LED (Status Indicator)
 * * Power Connections:
 * - Motor Power Supply (12V-24V) -> DRV8825 VMOT & GND
 * - 100uF 100V Capacitor -> Connected across VMOT and GND (Vital for driver protection)
 * - NEMA14 Coils -> DRV8825 A1/A2 and B1/B2
 * * Note: Ensure current limit on DRV8825 is adjusted via the potentiometer 
 * before connecting the motor to prevent overheating.
 */

// Pin Definitions
const int dirPin = 2;
const int stepPin = 3;
const int ledPin = 13; // Uses the built-in LED on the Arduino Uno

// Motor Settings
const int stepsPerRevolution = 200; // Standard for NEMA14 (1.8 degree step angle)
const int stepDelay = 1000;         // Delay between steps in microseconds (lower = faster)

void setup() {
  // Declare pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // Initialize LED to LOW
  digitalWrite(ledPin, LOW);
}

void loop() {
  // --- Direction: Clockwise ---
  digitalWrite(dirPin, HIGH);
  
  // Turn LED ON to indicate motor is active
  digitalWrite(ledPin, HIGH);
  
  // Spin motor one full revolution
  for(int x = 0; x < stepsPerRevolution; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay); 
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  
  // Turn LED OFF indicating movement is done
  digitalWrite(ledPin, LOW);
  
  // Wait 2 seconds
  delay(2000);
  
  // --- Direction: Counter-Clockwise ---
  digitalWrite(dirPin, LOW);
  
  // Turn LED ON
  digitalWrite(ledPin, HIGH);
  
  // Spin motor one full revolution back
  for(int x = 0; x < stepsPerRevolution; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay); 
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  
  // Turn LED OFF
  digitalWrite(ledPin, LOW);
  
  // Wait 2 seconds
  delay(2000);
}