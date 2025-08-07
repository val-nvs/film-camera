#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 0.96" OLED: 128x64 pixels total
// Dual color: Top 16 pixels = YELLOW, Bottom 48 pixels = BLUE
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int fps = 24;           // Current FPS value
int shutterSpeed = 50;  // Current shutter speed (Hz)
int selectedItem = 0;   // 0 = FPS selected, 1 = Shutter selected
bool editMode = false;  // true when editing a value

// FPS values (max 48, 2 digits)
int fpsValues[] = {12, 15, 24, 25, 30, 48};
int fpsIndex = 2;

// Shutter speeds (Hz values, 24-60 only)  
int shutterValues[] = {24, 30, 50, 60};
int shutterIndex = 2; // Start at 50Hz

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(2, INPUT_PULLUP);  // Button 1: SELECT/ENTER
  pinMode(3, INPUT_PULLUP);  // Button 2: NAVIGATE/CHANGE
  
  fps = fpsValues[fpsIndex];
  shutterSpeed = shutterValues[shutterIndex];
  
  showCameraSettings();
}

void showCameraSettings() {
  display.clearDisplay();
  
  // Title
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 0);
  display.println("CAMERA");
  
  // FPS Display - Bigger box
  display.setTextSize(3);
  if(selectedItem == 0 && !editMode) {
    display.fillRect(5, 15, 55, 35, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 0 && editMode) {
    display.drawRect(5, 15, 55, 35, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(15, 20);
  display.print(fps);
  
  // FPS label - always white
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 52);
  display.print("fps");
  
  // Shutter Speed Display - Bigger box
  display.setTextSize(3);
  if(selectedItem == 1 && !editMode) {
    display.fillRect(68, 15, 55, 35, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  } else if(selectedItem == 1 && editMode) {
    display.drawRect(68, 15, 55, 35, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE);
  }
  display.setCursor(75, 20);
  display.print(shutterSpeed);
  
  // Hz label - always white
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(90, 52);
  display.print("Hz");
  
  display.display();
}

bool isLongPress(int pin) {
  if(digitalRead(pin) == LOW) {
    delay(50);
    if(digitalRead(pin) == LOW) {
      unsigned long pressStart = millis();
      while(digitalRead(pin) == LOW) {
        if(millis() - pressStart > 500) {
          while(digitalRead(pin) == LOW) {}
          return true;
        }
      }
      return false;
    }
  }
  return false;
}

void loop() {
  // Button 1 - SELECT/ENTER/EXIT
  if(digitalRead(2) == LOW) {
    if(!editMode) {
      // Enter edit mode for selected item
      editMode = true;
    } else {
      // Exit edit mode
      editMode = false;
    }
    showCameraSettings();
    delay(200);
    while(digitalRead(2) == LOW) {}
  }
  
  // Button 2 - NAVIGATE/CHANGE
  if(digitalRead(3) == LOW) {
    if(!editMode) {
      // Navigate between FPS and Shutter
      selectedItem = 1 - selectedItem;  // Toggle between 0 and 1
    } else {
      // Change value of selected item
      if(isLongPress(3)) {
        // Long press - decrease
        if(selectedItem == 0) {
          fpsIndex--;
          if(fpsIndex < 0) fpsIndex = 5;  // Wrap to end
          fps = fpsValues[fpsIndex];
        } else {
          shutterIndex--;
          if(shutterIndex < 0) shutterIndex = 3;  // Wrap to end (4 elements)
          shutterSpeed = shutterValues[shutterIndex];
        }
      } else {
        // Short press - increase
        if(selectedItem == 0) {
          fpsIndex++;
          if(fpsIndex > 5) fpsIndex = 0;  // Wrap to start
          fps = fpsValues[fpsIndex];
        } else {
          shutterIndex++;
          if(shutterIndex > 3) shutterIndex = 0;  // Wrap to start (4 elements)
          shutterSpeed = shutterValues[shutterIndex];
        }
      }
    }
    showCameraSettings();
    delay(200);
  }
}