#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 0.96" OLED: 128x64 pixels total
// Dual color: Top 16 pixels = YELLOW, Bottom 48 pixels = BLUE
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Stepper motor pins
#define STEP_PIN 2
#define DIR_PIN 3
#define EN_PIN 6
// RST and SLP tied to 3V3
// Wiring: Pin 2→STP, Pin 3→DIR, Pin 6→EN, 3V3→RST+SLP, GND→GND, VCC→UMOT
// Motor: Navigate to Motor panel, press center button to rotate motor

int fps = 24;           // Current FPS value
int shutterSpeed = 50;  // Current shutter speed (Hz)
int selectedItem = 0;   // 0 = FPS selected, 1 = Shutter selected, 2 = Motor selected
bool editMode = false;  // true when editing a value

// FPS values (max 48, 2 digits)
int fpsValues[] = {12, 15, 24, 25, 30, 48};
int fpsIndex = 2;

// Shutter speeds (Hz values, 24-60 only)  
int shutterValues[] = {24, 30, 50, 60};
int shutterIndex = 2; // Start at 50Hz

// Motor control variables
int motorPosition = 0;  // Current step position

// Button state tracking
bool lastButton7 = HIGH;
bool lastButton8 = HIGH;
bool lastButton9 = HIGH;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(7, INPUT_PULLUP);  // LEFT/DECREASE button
  pinMode(8, INPUT_PULLUP);  // ENTER/EXIT button (center)
  pinMode(9, INPUT_PULLUP);  // RIGHT/INCREASE button
  
  // Stepper motor setup
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  
  digitalWrite(DIR_PIN, LOW);      // Set initial direction
  digitalWrite(EN_PIN, LOW);       // Force motor always enabled for testing
  
  fps = fpsValues[fpsIndex];
  shutterSpeed = shutterValues[shutterIndex];
  
  Serial.println("Setup complete - motor should be stiff now");
}

void stepMotor(int steps, bool clockwise) {
  digitalWrite(DIR_PIN, clockwise ? HIGH : LOW);
  
  for(int i = 0; i < abs(steps); i++) {
    digitalWrite(STEP_PIN, HIGH);
    delay(2);  // Step pulse width
    digitalWrite(STEP_PIN, LOW);
    delay(2);  // Step interval
  }
}

void rotateMotor() {
  Serial.println("Motor function called!");
  
  // Enable motor (already enabled for testing, but keeping logic)
  digitalWrite(EN_PIN, LOW);
  delay(10);  // Give motor time to energize
  
  Serial.println("Starting 50 steps...");
  // Rotate motor (50 steps = about 1/4 turn for most steppers)
  stepMotor(50, true);  // 50 steps clockwise
  motorPosition += 50;
  
  Serial.println("Steps complete");
  delay(100);  // Brief pause
  
  // Keep motor enabled for testing (comment out disable)
  // digitalWrite(EN_PIN, HIGH);
}

void showCameraSettings() {
  display.clearDisplay();
  
  // Button indicators in yellow area (top 16 pixels)
  // Left button (pin 7) circle
  if(digitalRead(7) == LOW) {
    display.fillCircle(85, 8, 4, SSD1306_WHITE);  // Filled when pressed
  } else {
    display.drawCircle(85, 8, 4, SSD1306_WHITE);  // Hollow when not pressed
  }
  
  // Center button (pin 8) circle  
  if(digitalRead(8) == LOW) {
    display.fillCircle(100, 8, 4, SSD1306_WHITE);
  } else {
    display.drawCircle(100, 8, 4, SSD1306_WHITE);
  }
  
  // Right button (pin 9) circle
  if(digitalRead(9) == LOW) {
    display.fillCircle(115, 8, 4, SSD1306_WHITE);
  } else {
    display.drawCircle(115, 8, 4, SSD1306_WHITE);
  }
  
  // FPS Display - Left box
  display.setTextSize(2);
  if(selectedItem == 0 && editMode) {
    display.fillRect(5, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 0 && !editMode) {
    display.drawRect(5, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(8, 22);
  display.print(fps);
  
  // FPS label
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(8, 45);
  display.print("fps");
  
  // Shutter Speed Display - Center box
  display.setTextSize(2);
  if(selectedItem == 1 && editMode) {
    display.fillRect(46, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 1 && !editMode) {
    display.drawRect(46, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(49, 22);
  display.print(shutterSpeed);
  
  // Hz label
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(58, 45);
  display.print("Hz");
  
  // Motor Display - Right box
  display.setTextSize(2);
  if(selectedItem == 2 && editMode) {
    display.fillRect(87, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 2 && !editMode) {
    display.drawRect(87, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(90, 22);
  display.print("GO");
  
  // Motor label
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(95, 45);
  display.print("motor");
  
  display.display();
}

void loop() {
  // Continuous display updates for real-time button feedback
  showCameraSettings();
  
  // Read current button states
  bool button7 = digitalRead(7);
  bool button8 = digitalRead(8);
  bool button9 = digitalRead(9);
  
  // Debug button states every second
  static unsigned long lastDebug = 0;
  if(millis() - lastDebug > 1000) {
    Serial.print("Button states: 7=");
    Serial.print(button7 ? "HIGH" : "LOW");
    Serial.print(" 8=");
    Serial.print(button8 ? "HIGH" : "LOW");
    Serial.print(" 9=");
    Serial.print(button9 ? "HIGH" : "LOW");
    Serial.print(" | Last: 7=");
    Serial.print(lastButton7 ? "HIGH" : "LOW");
    Serial.print(" 8=");
    Serial.print(lastButton8 ? "HIGH" : "LOW");
    Serial.print(" 9=");
    Serial.println(lastButton9 ? "HIGH" : "LOW");
    lastDebug = millis();
  }
  
  // Pin 7 - LEFT/DECREASE button (detect falling edge)
  if(button7 == LOW && lastButton7 == HIGH) {
    if(!editMode) {
      // Navigate left: cycle between FPS, Shutter, Motor
      selectedItem--;
      if(selectedItem < 0) selectedItem = 2;  // Wrap to Motor
    } else {
      // Decrease selected value (only for FPS and Shutter in edit mode)
      if(selectedItem == 0) {
        fpsIndex--;
        if(fpsIndex < 0) fpsIndex = 5;  // Wrap to end
        fps = fpsValues[fpsIndex];
      } else if(selectedItem == 1) {
        shutterIndex--;
        if(shutterIndex < 0) shutterIndex = 3;  // Wrap to end
        shutterSpeed = shutterValues[shutterIndex];
      }
    }
  }
  
  // Pin 8 - CENTER/ENTER/EXIT button (detect falling edge)
  if(button8 == LOW && lastButton8 == HIGH) {
    Serial.print("Center button pressed! selectedItem=");
    Serial.print(selectedItem);
    Serial.print(" editMode=");
    Serial.println(editMode);
    
    if(!editMode) {
      // Enter edit mode for FPS and Shutter, or rotate motor
      if(selectedItem == 0 || selectedItem == 1) {
        Serial.println("Entering edit mode");
        editMode = true;
      } else if(selectedItem == 2) {
        Serial.println("Motor panel - calling rotateMotor()");
        // Motor: rotate when pressed
        rotateMotor();
      }
    } else {
      Serial.println("Exiting edit mode");
      // Exit edit mode
      editMode = false;
    }
  }
  
  // Pin 9 - RIGHT/INCREASE button (detect falling edge)
  if(button9 == LOW && lastButton9 == HIGH) {
    if(!editMode) {
      // Navigate right: cycle between FPS, Shutter, Motor
      selectedItem++;
      if(selectedItem > 2) selectedItem = 0;  // Wrap to FPS
    } else {
      // Increase selected value (only for FPS and Shutter in edit mode)
      if(selectedItem == 0) {
        fpsIndex++;
        if(fpsIndex > 5) fpsIndex = 0;  // Wrap to start
        fps = fpsValues[fpsIndex];
      } else if(selectedItem == 1) {
        shutterIndex++;
        if(shutterIndex > 3) shutterIndex = 0;  // Wrap to start
        shutterSpeed = shutterValues[shutterIndex];
      }
    }
  }
  
  // Store button states for next iteration
  lastButton7 = button7;
  lastButton8 = button8;
  lastButton9 = button9;
  
  delay(50);  // 20 FPS refresh rate for smooth UI
}