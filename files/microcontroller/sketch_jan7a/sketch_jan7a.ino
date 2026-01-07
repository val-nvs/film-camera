#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int stepPin = 2;
const int dirPin = 3;
const int enPin = 4;
const int btn1 = 8;  // Move
const int btn2 = 9;  // Toggle Lock
const int btn3 = 10; // Visual Test

bool motorLocked = false; 
String currentStatus = "IDLE";

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH); // Start unlocked (COLD)

  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  // --- 1. HANDLE BUTTON 2 (LOCK TOGGLE) ---
  if (digitalRead(btn2) == LOW) {
    motorLocked = !motorLocked;
    digitalWrite(enPin, motorLocked ? LOW : HIGH); // LOW = Enabled
    delay(200); // Debounce to prevent flickering
  }

  // --- 2. HANDLE BUTTON 1 (ROTATE) ---
  if (digitalRead(btn1) == LOW) {
    currentStatus = "MOVING";
    updateDisplay(); // Show "MOVING" immediately

    digitalWrite(enPin, LOW); // Ensure driver is on
    digitalWrite(dirPin, HIGH);
    
    for(int x = 0; x < 400; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(1000); // Slightly faster speed
      digitalWrite(stepPin, LOW);
      delayMicroseconds(1000);
    }
    
    // Return to locked state if toggle was on, otherwise turn off
    digitalWrite(enPin, motorLocked ? LOW : HIGH);
    currentStatus = "IDLE";
  }

  // --- 3. REFRESH DASHBOARD ---
  updateDisplay();
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("SYSTEM STATUS");
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  // Motor Info
  display.setCursor(0, 18);
  display.print("MOTOR: ");
  display.println(currentStatus);
  
  display.setCursor(0, 28);
  display.print("POWER: ");
  display.println(motorLocked ? "LOCKED (ON)" : "UNLOCKED (OFF)");

  // Button Feedback Line (at the bottom)
  display.setCursor(0, 48);
  display.print("INPUTS: [8] [9] [10]");
  
  // Draw indicator boxes under the numbers
  // Button 8
  if (digitalRead(btn1) == LOW) display.fillRect(45, 58, 15, 4, SSD1306_WHITE);
  else display.drawRect(45, 58, 15, 4, SSD1306_WHITE);
  
  // Button 9
  if (digitalRead(btn2) == LOW) display.fillRect(68, 58, 15, 4, SSD1306_WHITE);
  else display.drawRect(68, 58, 15, 4, SSD1306_WHITE);
  
  // Button 10
  if (digitalRead(btn3) == LOW) display.fillRect(95, 58, 15, 4, SSD1306_WHITE);
  else display.drawRect(95, 58, 15, 4, SSD1306_WHITE);

  display.display();
}