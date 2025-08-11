#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 0.96" OLED: 128x64 pixels total
// Dual color: Top 16 pixels = YELLOW, Bottom 48 pixels = BLUE
Adafruit_SSD1306 display(128, 64, &Wire, -1);

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

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(7, INPUT_PULLUP);  // LEFT/DECREASE button
  pinMode(8, INPUT_PULLUP);  // ENTER/EXIT button (center)
  pinMode(9, INPUT_PULLUP);  // RIGHT/INCREASE button
  
  fps = fpsValues[fpsIndex];
  shutterSpeed = shutterValues[shutterIndex];
  
  showCameraSettings();
}

void showCameraSettings() {
  display.clearDisplay();
  
  // Button indicators in yellow area (top 16 pixels)
  // Left button (pin 7) circle
  if(digitalRead(7) == LOW) {
    display.fillCircle(85, 8, 4, SSD1306_WHITE);  // Filled circle when pressed
  } else {
    display.drawCircle(85, 8, 4, SSD1306_WHITE);  // Hollow circle when not pressed
  }
  
  // Center button (pin 8) circle  
  if(digitalRead(8) == LOW) {
    display.fillCircle(100, 8, 4, SSD1306_WHITE);  // Filled circle when pressed
  } else {
    display.drawCircle(100, 8, 4, SSD1306_WHITE);  // Hollow circle when not pressed
  }
  
  // Right button (pin 9) circle
  if(digitalRead(9) == LOW) {
    display.fillCircle(115, 8, 4, SSD1306_WHITE);  // Filled circle when pressed
  } else {
    display.drawCircle(115, 8, 4, SSD1306_WHITE);  // Hollow circle when not pressed
  }
  
  // FPS Display - Left box
  display.setTextSize(2);
  if(selectedItem == 0 && !editMode) {
    display.fillRect(5, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 0 && editMode) {
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
  if(selectedItem == 1 && !editMode) {
    display.fillRect(46, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 1 && editMode) {
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
  if(selectedItem == 2 && !editMode) {
    display.fillRect(87, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 2 && editMode) {
    display.drawRect(87, 18, 35, 25, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(95, 22);
  display.print("MT");
  
  // Motor label
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(95, 45);
  display.print("motor");
  
  display.display();
}

void loop() {
  // Update display continuously to show button states
  showCameraSettings();
  
  // Pin 7 - LEFT/DECREASE button
  if(digitalRead(7) == LOW) {
    if(!editMode) {
      // Navigate left: cycle between FPS, Shutter, Motor
      selectedItem--;
      if(selectedItem < 0) selectedItem = 2;  // Wrap to Motor
    } else {
      // Decrease selected value (only for FPS and Shutter)
      if(selectedItem == 0) {
        fpsIndex--;
        if(fpsIndex < 0) fpsIndex = 5;  // Wrap to end
        fps = fpsValues[fpsIndex];
      } else if(selectedItem == 1) {
        shutterIndex--;
        if(shutterIndex < 0) shutterIndex = 3;  // Wrap to end
        shutterSpeed = shutterValues[shutterIndex];
      }
      // Motor control not implemented yet
    }
    delay(200);
    while(digitalRead(7) == LOW) {}
  }
  
  // Pin 8 - CENTER/ENTER/EXIT button
  if(digitalRead(8) == LOW) {
    if(!editMode) {
      // Enter edit mode for selected item (only FPS and Shutter for now)
      if(selectedItem == 0 || selectedItem == 1) {
        editMode = true;
      }
      // Motor edit mode not implemented yet
    } else {
      // Exit edit mode
      editMode = false;
    }
    delay(200);
    while(digitalRead(8) == LOW) {}
  }
  
  // Pin 9 - RIGHT/INCREASE button
  if(digitalRead(9) == LOW) {
    if(!editMode) {
      // Navigate right: cycle between FPS, Shutter, Motor
      selectedItem++;
      if(selectedItem > 2) selectedItem = 0;  // Wrap to FPS
    } else {
      // Increase selected value (only for FPS and Shutter)
      if(selectedItem == 0) {
        fpsIndex++;
        if(fpsIndex > 5) fpsIndex = 0;  // Wrap to start
        fps = fpsValues[fpsIndex];
      } else if(selectedItem == 1) {
        shutterIndex++;
        if(shutterIndex > 3) shutterIndex = 0;  // Wrap to start
        shutterSpeed = shutterValues[shutterIndex];
      }
      // Motor control not implemented yet
    }
    delay(200);
    while(digitalRead(9) == LOW) {}
  }
  
  delay(50);  // Small delay for smooth updates
}