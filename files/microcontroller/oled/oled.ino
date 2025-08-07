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
  pinMode(7, INPUT_PULLUP);  // LEFT/DECREASE button
  pinMode(8, INPUT_PULLUP);  // ENTER/EXIT button (center)
  pinMode(9, INPUT_PULLUP);  // RIGHT/INCREASE button
  
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

// Removed isLongPress function - no longer needed with dedicated +/- buttons

void loop() {
  // Pin 7 - LEFT/DECREASE button
  if(digitalRead(7) == LOW) {
    if(!editMode) {
      // Navigate left: cycle between FPS and Shutter
      selectedItem = 1 - selectedItem;  // Toggle between 0 and 1
    } else {
      // Decrease selected value
      if(selectedItem == 0) {
        fpsIndex--;
        if(fpsIndex < 0) fpsIndex = 5;  // Wrap to end
        fps = fpsValues[fpsIndex];
      } else {
        shutterIndex--;
        if(shutterIndex < 0) shutterIndex = 3;  // Wrap to end
        shutterSpeed = shutterValues[shutterIndex];
      }
    }
    showCameraSettings();
    delay(200);
    while(digitalRead(7) == LOW) {}
  }
  
  // Pin 8 - CENTER/ENTER/EXIT button
  if(digitalRead(8) == LOW) {
    if(!editMode) {
      // Enter edit mode for selected item
      editMode = true;
    } else {
      // Exit edit mode
      editMode = false;
    }
    showCameraSettings();
    delay(200);
    while(digitalRead(8) == LOW) {}
  }
  
  // Pin 9 - RIGHT/INCREASE button
  if(digitalRead(9) == LOW) {
    if(!editMode) {
      // Navigate right: cycle between FPS and Shutter  
      selectedItem = 1 - selectedItem;  // Toggle between 0 and 1
    } else {
      // Increase selected value
      if(selectedItem == 0) {
        fpsIndex++;
        if(fpsIndex > 5) fpsIndex = 0;  // Wrap to start
        fps = fpsValues[fpsIndex];
      } else {
        shutterIndex++;
        if(shutterIndex > 3) shutterIndex = 0;  // Wrap to start
        shutterSpeed = shutterValues[shutterIndex];
      }
    }
    showCameraSettings();
    delay(200);
    while(digitalRead(9) == LOW) {}
  }
}