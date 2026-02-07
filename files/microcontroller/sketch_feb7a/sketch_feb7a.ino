#include <LedControl.h>

// ===== ARDUINO R4 WIFI BUILT-IN MATRIX - START =====
#include "Arduino_LED_Matrix.h"
// ===== ARDUINO R4 WIFI BUILT-IN MATRIX - END =====

// MAX7219 Pins: DIN=12, CLK=13, CS=A0
LedControl lc = LedControl(12, 13, A0, 2);  // 2 displays chained

// ===== ARDUINO R4 WIFI BUILT-IN MATRIX - START =====
// Built-in R4 WiFi LED Matrix
ArduinoLEDMatrix matrix;
// ===== ARDUINO R4 WIFI BUILT-IN MATRIX - END =====

// Motor Pins
const int en1 = 2, step1 = 3, dir1 = 4;
const int en2 = 5, step2 = 6, dir2 = 7;

// D-pad Buttons
const int btnDown = 8;
const int btnRight = 9;
const int btnUp = 10;
const int btnLeft = 11;

// Menu state
int currentMenu = 0;  // 0 = main menu, 1 = submenu shutter, 2 = submenu claw, 3 = shutter speed submenu, 4 = both submenu, 5 = fps submenu, 6 = duration submenu
int selectedLine = 0; // 0 = top line (motor1), 1 = middle line (motor2), 2 = bottom line (both)
int shutterSubSelection = 0; // 0 = lock, 1 = run shutter, 2 = speed settings, 3 = unused
int clawSubSelection = 0; // 0 = lock, 1 = run claw, 2 = skip, 3 = unused
int bothSubSelection = 0; // 0 = lock/unlock both, 1 = run, 2 = fps, 3 = duration
bool motor1Locked = false; // Motor 1 (shutter) lock state
bool motor2Locked = false; // Motor 2 (claw) lock state

// FPS settings for Both menu
const float fpsValues[] = {2.0, 1.0, 0.5}; // Frames per second
const int numFPS = 3;
int fpsIndex = 1; // Default to 1 FPS

// Duration (cycles) settings for Both menu
const int cyclesValues[] = {5, 10, 20}; // Number of cycles
const int numCycles = 3;
int cyclesIndex = 1; // Default to 10 cycles

// Shutter speed array in microseconds: .3 .4 .5 1 2 3 4 5 8 10 16 25 32 50 60 120 180
const long speedValues[] = {
  300000,    // .3
  400000,    // .4
  500000,    // .5
  1000000,   // 1
  2000000,   // 2
  3000000,   // 3
  4000000,   // 4
  5000000,   // 5
  8000000,   // 8
  10000000,  // 10
  16000000,  // 16
  25000000,  // 25
  32000000,  // 32
  50000000,  // 50
  60000000,  // 60
  120000000, // 120
  180000000  // 180
};
const int numSpeeds = 17;
int speedIndex = 2; // Default to .5s (index 2)

unsigned long lastBlink = 0;
bool blinkState = false;

uint8_t frame[8][12] = {0};  // Only using 12x8 space

void setup() {
  // ===== ARDUINO R4 WIFI BUILT-IN MATRIX - START =====
  // Initialize built-in R4 WiFi LED matrix
  matrix.begin();
  // ===== ARDUINO R4 WIFI BUILT-IN MATRIX - END =====
  
  // Initialize both MAX7219 displays
  delay(50);  // Small delay before init
  lc.shutdown(0, false);  // Wake up display 0
  lc.shutdown(1, false);  // Wake up display 1
  lc.setIntensity(0, 8);  // Set brightness (0-15)
  lc.setIntensity(1, 8);
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  delay(50);  // Small delay after init
  
  int outputs[] = {en1, step1, dir1, en2, step2, dir2};
  for(int p : outputs) pinMode(p, OUTPUT);
  
  int inputs[] = {btnDown, btnRight, btnUp, btnLeft};
  for(int p : inputs) pinMode(p, INPUT_PULLUP);

  digitalWrite(en1, LOW);
  digitalWrite(en2, LOW);
}

void loop() {
  handleInput();
  drawUI();
  delay(50);
}

void handleInput() {
  static bool lastBtnDown = true;
  static bool lastBtnRight = true;
  static bool lastBtnUp = true;
  static bool lastBtnLeft = true;
  
  bool currentDown = digitalRead(btnDown) == LOW;
  bool currentRight = digitalRead(btnRight) == LOW;
  bool currentUp = digitalRead(btnUp) == LOW;
  bool currentLeft = digitalRead(btnLeft) == LOW;
  
  // Main menu navigation
  if (currentMenu == 0) {
    // Down button - navigate down in main menu
    if (currentDown && !lastBtnDown) {
      if (selectedLine < 2) {
        selectedLine++;
      }
      delay(50);
    }
    
    // Up button - navigate up in main menu
    if (currentUp && !lastBtnUp) {
      if (selectedLine > 0) {
        selectedLine--;
      }
      delay(50);
    }
    
    // Right button - enter submenu
    if (currentRight && !lastBtnRight) {
      if (selectedLine == 0) {
        currentMenu = 1; // Enter shutter submenu
        shutterSubSelection = 0;
      } else if (selectedLine == 1) {
        currentMenu = 2; // Enter claw submenu
        clawSubSelection = 0;
      } else {
        currentMenu = 4; // Enter both submenu
        bothSubSelection = 0;
      }
      delay(200); // Longer delay to prevent immediate activation in submenu
    }
  }
  // Shutter submenu
  else if (currentMenu == 1) {
    // Left button - go back to main menu
    if (currentLeft && !lastBtnLeft) {
      currentMenu = 0;
      delay(50);
    }
    
    // Down button - navigate down
    if (currentDown && !lastBtnDown) {
      if (shutterSubSelection < 3) {
        shutterSubSelection++;
      }
      delay(50);
    }
    
    // Up button - navigate up
    if (currentUp && !lastBtnUp) {
      if (shutterSubSelection > 0) {
        shutterSubSelection--;
      }
      delay(50);
    }
    
    // Right button - enter action or submenu
    if (currentRight && !lastBtnRight) {
      if (shutterSubSelection == 0) {
        // Toggle lock
        motor1Locked = !motor1Locked;
        digitalWrite(en1, motor1Locked ? HIGH : LOW);
      } else if (shutterSubSelection == 1) {
        // Run shutter motor 200 steps
        runMotor(en1, step1, dir1, 200, speedValues[speedIndex]);
      } else if (shutterSubSelection == 2) {
        // Enter speed settings submenu
        currentMenu = 3;
      }
      // Option 3 does nothing
      delay(200); // Longer delay to prevent immediate activation
    }
  }
  // Claw submenu
  else if (currentMenu == 2) {
    // Left button - go back to main menu
    if (currentLeft && !lastBtnLeft) {
      currentMenu = 0;
      delay(50);
    }
    
    // Down button - navigate down
    if (currentDown && !lastBtnDown) {
      if (clawSubSelection < 3) {
        clawSubSelection++;
      }
      delay(50);
    }
    
    // Up button - navigate up
    if (currentUp && !lastBtnUp) {
      if (clawSubSelection > 0) {
        clawSubSelection--;
      }
      delay(50);
    }
    
    // Right button - action
    if (currentRight && !lastBtnRight) {
      if (clawSubSelection == 0) {
        // Toggle lock
        motor2Locked = !motor2Locked;
        digitalWrite(en2, motor2Locked ? HIGH : LOW);
      } else if (clawSubSelection == 1) {
        // Run claw motor (MOTOR 2!) - 200 steps, 200ms total (1000μs per pulse)
        runMotor(en2, step2, dir2, 200, 200000);
      } else if (clawSubSelection == 2) {
        // Skip - Run claw motor 10x (2000 steps, 2000ms total)
        runMotor(en2, step2, dir2, 2000, 2000000);
      }
      // Option 3 does nothing
      delay(200);
    }
  }
  // Shutter speed settings submenu
  else if (currentMenu == 3) {
    // Left button - go back to shutter submenu
    if (currentLeft && !lastBtnLeft) {
      currentMenu = 1;
      delay(200);
    }
    
    // Up button - increase time (slower shutter)
    if (currentUp && !lastBtnUp) {
      if (speedIndex < numSpeeds - 1) {
        speedIndex++;
      }
      delay(200);
    }
    
    // Down button - decrease time (faster shutter)
    if (currentDown && !lastBtnDown) {
      if (speedIndex > 0) {
        speedIndex--;
      }
      delay(200);
    }
  }
  // Both submenu
  else if (currentMenu == 4) {
    // Left button - go back to main menu
    if (currentLeft && !lastBtnLeft) {
      currentMenu = 0;
      delay(50);
    }
    
    // Down button - navigate down
    if (currentDown && !lastBtnDown) {
      if (bothSubSelection < 3) {
        bothSubSelection++;
      }
      delay(50);
    }
    
    // Up button - navigate up
    if (currentUp && !lastBtnUp) {
      if (bothSubSelection > 0) {
        bothSubSelection--;
      }
      delay(50);
    }
    
    // Right button - enter action or submenu
    if (currentRight && !lastBtnRight) {
      if (bothSubSelection == 0) {
        // Lock/Unlock both motors
        // If both are locked, unlock both
        // If only one or none are locked, lock both
        if (motor1Locked && motor2Locked) {
          // Unlock both
          motor1Locked = false;
          motor2Locked = false;
          digitalWrite(en1, LOW);
          digitalWrite(en2, LOW);
        } else {
          // Lock both
          motor1Locked = true;
          motor2Locked = true;
          digitalWrite(en1, HIGH);
          digitalWrite(en2, HIGH);
        }
      } else if (bothSubSelection == 1) {
        // Run both motors for selected cycles
        runBothCycle();
      } else if (bothSubSelection == 2) {
        // Enter FPS settings submenu
        currentMenu = 5;
      } else if (bothSubSelection == 3) {
        // Enter Duration settings submenu
        currentMenu = 6;
      }
      delay(200);
    }
  }
  // FPS settings submenu
  else if (currentMenu == 5) {
    // Left button - go back to both submenu
    if (currentLeft && !lastBtnLeft) {
      currentMenu = 4;
      delay(200);
    }
    
    // Up button - increase FPS (faster)
    if (currentUp && !lastBtnUp) {
      if (fpsIndex > 0) {
        fpsIndex--;
      }
      delay(200);
    }
    
    // Down button - decrease FPS (slower)
    if (currentDown && !lastBtnDown) {
      if (fpsIndex < numFPS - 1) {
        fpsIndex++;
      }
      delay(200);
    }
  }
  // Duration (cycles) settings submenu
  else if (currentMenu == 6) {
    // Left button - go back to both submenu
    if (currentLeft && !lastBtnLeft) {
      currentMenu = 4;
      delay(200);
    }
    
    // Up button - increase cycles
    if (currentUp && !lastBtnUp) {
      if (cyclesIndex < numCycles - 1) {
        cyclesIndex++;
      }
      delay(200);
    }
    
    // Down button - decrease cycles
    if (currentDown && !lastBtnDown) {
      if (cyclesIndex > 0) {
        cyclesIndex--;
      }
      delay(200);
    }
  }
  
  lastBtnDown = currentDown;
  lastBtnRight = currentRight;
  lastBtnUp = currentUp;
  lastBtnLeft = currentLeft;
}

void drawUI() {
  memset(frame, 0, sizeof(frame));
  
  // Handle blinking - faster rate for selected items
  if (millis() - lastBlink > 250) {
    blinkState = !blinkState;
    lastBlink = millis();
  }
  
  if (currentMenu == 0) {
    drawMainMenu();
  } else if (currentMenu == 1) {
    drawShutterMenu();
  } else if (currentMenu == 2) {
    drawClawMenu();
  } else if (currentMenu == 3) {
    drawSpeedMenu();
  } else if (currentMenu == 4) {
    drawBothMenu();
  } else if (currentMenu == 5) {
    drawFPSMenu();
  } else if (currentMenu == 6) {
    drawDurationMenu();
  }
  
  // Always show lock indicators
  // Shutter (motor1) lock at column 10, row 0
  if (motor1Locked) {
    frame[0][10] = 1;
  }
  // Claw (motor2) lock at column 11, row 0
  if (motor2Locked) {
    frame[0][11] = 1;
  }
  
  // ===== ARDUINO R4 WIFI BUILT-IN MATRIX - START =====
  // Render to built-in R4 WiFi LED matrix
  matrix.renderBitmap(frame, 8, 12);
  // ===== ARDUINO R4 WIFI BUILT-IN MATRIX - END =====
  
  // Render to MAX7219 displays - only use 12 columns
  // Display 1 (left) = columns 0-7, Display 0 (right) = columns 8-11
  for (int row = 0; row < 8; row++) {
    byte col0 = 0, col1 = 0;
    
    // Display 1 - left side (columns 0-7)
    for (int col = 0; col < 8; col++) {
      if (frame[row][col]) {
        col1 |= (1 << col);
      }
    }
    
    // Display 0 - right side (columns 8-11 only)
    for (int col = 8; col < 12; col++) {
      if (frame[row][col]) {
        col0 |= (1 << (col - 8));
      }
    }
    
    lc.setRow(0, row, col0);
    lc.setRow(1, row, col1);
  }
}

void drawMainMenu() {
  // Draw motor1 selector (top box, rows 0-1, cols 0-1)
  // Always show, blink if selected
  if (selectedLine != 0 || (selectedLine == 0 && blinkState)) {
    frame[0][0] = 1; frame[0][1] = 1;
    frame[1][0] = 1; frame[1][1] = 1;
  }
  
  // Draw motor2 selector (2nd box, rows 2-3, cols 0-1) - touching the top box
  // Always show, blink if selected
  if (selectedLine != 1 || (selectedLine == 1 && blinkState)) {
    frame[2][0] = 1; frame[2][1] = 1;
    frame[3][0] = 1; frame[3][1] = 1;
  }
  
  // Draw both selector (3rd box, rows 4-5, cols 0-1) - selectable
  if (selectedLine != 2 || (selectedLine == 2 && blinkState)) {
    frame[4][0] = 1; frame[4][1] = 1;
    frame[5][0] = 1; frame[5][1] = 1;
  }
  
  // Draw 4th box (rows 6-7, cols 0-1) - useless for now, always solid
  frame[6][0] = 1; frame[6][1] = 1;
  frame[7][0] = 1; frame[7][1] = 1;
  
  // Draw rolling text starting at column 3
  if (selectedLine == 0) {
    drawRollingText("Shutter", 3);
  } else if (selectedLine == 1) {
    drawRollingText("Claw", 3);
  } else {
    drawRollingText("Both", 3);
  }
}

void drawRollingText(const char* text, int startCol) {
  static unsigned long lastScroll = 0;
  static int scrollOffset = 0;
  static int lastMenu = -1;
  static int lastSelection = -1;
  
  // Reset scroll when menu or selection changes
  int currentSelection = (currentMenu == 0) ? selectedLine : 
                        (currentMenu == 1) ? shutterSubSelection : 
                        (currentMenu == 2) ? clawSubSelection : 
                        (currentMenu == 4) ? bothSubSelection :
                        (currentMenu == 5) ? fpsIndex :
                        (currentMenu == 6) ? cyclesIndex : 0;
  
  if (currentMenu != lastMenu || currentSelection != lastSelection) {
    scrollOffset = 0;
    lastMenu = currentMenu;
    lastSelection = currentSelection;
    lastScroll = millis(); // Reset timer too
  }
  
  // Update scroll position every 100ms
  if (millis() - lastScroll > 100) {
    scrollOffset++;
    lastScroll = millis();
  }
  
  // Calculate total text width with variable spacing
  int totalWidth = 0;
  int textLen = strlen(text);
  for (int i = 0; i < textLen; i++) {
    totalWidth += getCharWidth(text[i]) + 1; // char width + 1px gap
  }
  
  // Reset scroll when text completely scrolled off the left side
  if (scrollOffset >= totalWidth + 12) {
    scrollOffset = 0;
  }
  
  // Draw each character with variable spacing
  int currentX = 12 - scrollOffset;
  for (int i = 0; i < textLen; i++) {
    int charWidth = getCharWidth(text[i]);
    
    // Only draw if character is visible on screen (between startCol and 12)
    if (currentX + charWidth > startCol && currentX < 12) {
      drawChar(text[i], currentX, 0);
    }
    
    currentX += charWidth + 1; // Move by char width + 1px gap
  }
}

int getCharWidth(char c) {
  // Return the actual pixel width of each character (rightmost - leftmost + 1)
  switch(c) {
    case 'S': return 5;
    case 'h': return 5;
    case 'u': return 5;
    case 't': return 3;
    case 'e': return 5;
    case 'r': return 5;
    case 'C': return 5;
    case 'l': return 2;
    case 'a': return 5;
    case 'w': return 5;
    case 'L': return 5;
    case 'o': return 5;
    case 'c': return 5;
    case 'k': return 4;
    case 'U': return 5;
    case 'n': return 5;
    case 'R': return 5;
    case 'T': return 5;
    case 'i': return 1;
    case 'm': return 5;
    case 'p': return 5;
    case 'B': return 5;
    case 'F': return 5;
    case 'P': return 5;
    case 'D': return 5;
    default: return 3; // Default for unknown chars
  }
}

void drawChar(char c, int x, int y) {
  // Simple variable-width font patterns (7 pixels tall)
  byte charData[7];
  
  switch(c) {
    case 'S':
      charData[0] = 0b01111;
      charData[1] = 0b10000;
      charData[2] = 0b01110;
      charData[3] = 0b00001;
      charData[4] = 0b00001;
      charData[5] = 0b10001;
      charData[6] = 0b01110;
      break;
    case 'h':
      charData[0] = 0b10000;
      charData[1] = 0b10000;
      charData[2] = 0b10110;
      charData[3] = 0b11001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b10001;
      break;
    case 'u':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b10001;
      charData[3] = 0b10001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b01111;
      break;
    case 't':
      charData[0] = 0b010;
      charData[1] = 0b111;
      charData[2] = 0b010;
      charData[3] = 0b010;
      charData[4] = 0b010;
      charData[5] = 0b010;
      charData[6] = 0b001;
      break;
    case 'e':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b01110;
      charData[3] = 0b10001;
      charData[4] = 0b11111;
      charData[5] = 0b10000;
      charData[6] = 0b01111;
      break;
    case 'r':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b10110;
      charData[3] = 0b11001;
      charData[4] = 0b10000;
      charData[5] = 0b10000;
      charData[6] = 0b10000;
      break;
    case 'C':
      charData[0] = 0b01110;
      charData[1] = 0b10001;
      charData[2] = 0b10000;
      charData[3] = 0b10000;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b01110;
      break;
    case 'l':
      charData[0] = 0b10;
      charData[1] = 0b10;
      charData[2] = 0b10;
      charData[3] = 0b10;
      charData[4] = 0b10;
      charData[5] = 0b10;
      charData[6] = 0b01;
      break;
    case 'a':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b01110;
      charData[3] = 0b00001;
      charData[4] = 0b01111;
      charData[5] = 0b10001;
      charData[6] = 0b01111;
      break;
    case 'w':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b10001;
      charData[3] = 0b10001;
      charData[4] = 0b10101;
      charData[5] = 0b10101;
      charData[6] = 0b01011;
      break;
    case 'L':
      charData[0] = 0b10000;
      charData[1] = 0b10000;
      charData[2] = 0b10000;
      charData[3] = 0b10000;
      charData[4] = 0b10000;
      charData[5] = 0b10000;
      charData[6] = 0b11111;
      break;
    case 'o':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b01110;
      charData[3] = 0b10001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b01110;
      break;
    case 'c':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b01110;
      charData[3] = 0b10001;
      charData[4] = 0b10000;
      charData[5] = 0b10001;
      charData[6] = 0b01110;
      break;
    case 'k':
      charData[0] = 0b1000;
      charData[1] = 0b1000;
      charData[2] = 0b1001;
      charData[3] = 0b1010;
      charData[4] = 0b1100;
      charData[5] = 0b1010;
      charData[6] = 0b1001;
      break;
    case 'U':
      charData[0] = 0b10001;
      charData[1] = 0b10001;
      charData[2] = 0b10001;
      charData[3] = 0b10001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b01110;
      break;
    case 'n':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b11110;
      charData[3] = 0b10001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b10001;
      break;
    case 'R':
      charData[0] = 0b11110;
      charData[1] = 0b10001;
      charData[2] = 0b11110;
      charData[3] = 0b10001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b10001;
      break;
    case 'T':
      charData[0] = 0b11111;
      charData[1] = 0b00100;
      charData[2] = 0b00100;
      charData[3] = 0b00100;
      charData[4] = 0b00100;
      charData[5] = 0b00100;
      charData[6] = 0b00100;
      break;
    case 'i':
      charData[0] = 0b1;
      charData[1] = 0b0;
      charData[2] = 0b1;
      charData[3] = 0b1;
      charData[4] = 0b1;
      charData[5] = 0b1;
      charData[6] = 0b1;
      break;
    case 'm':
      charData[0] = 0b00000;
      charData[1] = 0b00000;
      charData[2] = 0b11010;
      charData[3] = 0b10101;
      charData[4] = 0b10101;
      charData[5] = 0b10001;
      charData[6] = 0b10001;
      break;
    case 'p':
      charData[0] = 0b00000;
      charData[1] = 0b10110;
      charData[2] = 0b11001;
      charData[3] = 0b10001;
      charData[4] = 0b11110;
      charData[5] = 0b10000;
      charData[6] = 0b10000;
      break;
    case 'B':
      charData[0] = 0b11110;
      charData[1] = 0b10001;
      charData[2] = 0b11110;
      charData[3] = 0b10001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b11110;
      break;
    case 'F':
      charData[0] = 0b11111;
      charData[1] = 0b10000;
      charData[2] = 0b11110;
      charData[3] = 0b10000;
      charData[4] = 0b10000;
      charData[5] = 0b10000;
      charData[6] = 0b10000;
      break;
    case 'P':
      charData[0] = 0b11110;
      charData[1] = 0b10001;
      charData[2] = 0b10001;
      charData[3] = 0b11110;
      charData[4] = 0b10000;
      charData[5] = 0b10000;
      charData[6] = 0b10000;
      break;
    case 'D':
      charData[0] = 0b11110;
      charData[1] = 0b10001;
      charData[2] = 0b10001;
      charData[3] = 0b10001;
      charData[4] = 0b10001;
      charData[5] = 0b10001;
      charData[6] = 0b11110;
      break;
    default:
      // Space or unknown character
      for (int i = 0; i < 7; i++) charData[i] = 0;
      break;
  }
  
  // Draw the character starting at row 1 - only draw pixels that are >= column 3 and < column 12
  // Get the width of this character to determine how many columns to check
  int charWidth = getCharWidth(c);
  
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < charWidth; col++) {
      int pixelX = x + col;
      if (pixelX >= 3 && pixelX < 12) {  // Clip to start at column 3
        if (charData[row] & (1 << (charWidth - 1 - col))) {
          frame[row + 1][pixelX] = 1;  // Start at row 1 (2nd row)
        }
      }
    }
  }
}

void drawShutterMenu() {
  // No border - full 12x8 area available
  // Light up first column (column 0)
  for (int row = 0; row < 8; row++) {
    frame[row][0] = 1;
  }
  
  // Squares at column 1, stacked vertically
  
  // First option: Lock/Unlock (top box, rows 0-1, cols 1-2)
  if (shutterSubSelection != 0 || (shutterSubSelection == 0 && blinkState)) {
    frame[0][1] = 1; frame[0][2] = 1;
    frame[1][1] = 1; frame[1][2] = 1;
  }
  
  // Second option: Run shutter (box 2, rows 2-3, cols 1-2)
  if (shutterSubSelection != 1 || (shutterSubSelection == 1 && blinkState)) {
    frame[2][1] = 1; frame[2][2] = 1;
    frame[3][1] = 1; frame[3][2] = 1;
  }
  
  // Third option: Speed settings (box 3, rows 4-5, cols 1-2)
  if (shutterSubSelection != 2 || (shutterSubSelection == 2 && blinkState)) {
    frame[4][1] = 1; frame[4][2] = 1;
    frame[5][1] = 1; frame[5][2] = 1;
  }
  
  // Fourth option: Unused (box 4, rows 6-7, cols 1-2)
  if (shutterSubSelection != 3 || (shutterSubSelection == 3 && blinkState)) {
    frame[6][1] = 1; frame[6][2] = 1;
    frame[7][1] = 1; frame[7][2] = 1;
  }
  
  // Draw scrolling text for the selected option starting at column 3
  if (shutterSubSelection == 0) {
    // "Lock" or "Unlock"
    if (motor1Locked) {
      drawRollingText("Unlock", 3);
    } else {
      drawRollingText("Lock", 3);
    }
  } else if (shutterSubSelection == 1) {
    // "Run"
    drawRollingText("Run", 3);
  } else if (shutterSubSelection == 2) {
    // "Time"
    drawRollingText("Time", 3);
  }
  // Option 3 shows no text (unused)
}

void drawClawMenu() {
  // No border - full 12x8 area available
  // Light up first column (column 0)
  for (int row = 0; row < 8; row++) {
    frame[row][0] = 1;
  }
  
  // Squares at column 1, stacked vertically
  
  // First option: Lock/Unlock (top box, rows 0-1, cols 1-2)
  if (clawSubSelection != 0 || (clawSubSelection == 0 && blinkState)) {
    frame[0][1] = 1; frame[0][2] = 1;
    frame[1][1] = 1; frame[1][2] = 1;
  }
  
  // Second option: Run claw (box 2, rows 2-3, cols 1-2)
  if (clawSubSelection != 1 || (clawSubSelection == 1 && blinkState)) {
    frame[2][1] = 1; frame[2][2] = 1;
    frame[3][1] = 1; frame[3][2] = 1;
  }
  
  // Third option: Skip (box 3, rows 4-5, cols 1-2)
  if (clawSubSelection != 2 || (clawSubSelection == 2 && blinkState)) {
    frame[4][1] = 1; frame[4][2] = 1;
    frame[5][1] = 1; frame[5][2] = 1;
  }
  
  // Fourth option: Unused (box 4, rows 6-7, cols 1-2)
  if (clawSubSelection != 3 || (clawSubSelection == 3 && blinkState)) {
    frame[6][1] = 1; frame[6][2] = 1;
    frame[7][1] = 1; frame[7][2] = 1;
  }
  
  // Draw scrolling text for the selected option starting at column 3
  if (clawSubSelection == 0) {
    // "Lock" or "Unlock"
    if (motor2Locked) {
      drawRollingText("Unlock", 3);
    } else {
      drawRollingText("Lock", 3);
    }
  } else if (clawSubSelection == 1) {
    // "Run"
    drawRollingText("Run", 3);
  } else if (clawSubSelection == 2) {
    // "Skip"
    drawRollingText("Skip", 3);
  }
  // Option 3 shows no text (unused)
}

void drawSpeedMenu() {
  // No border - full 12x8 area available
  // Display as plain text starting at 1,1 (row 1, column 1)
  
  // Get current speed value from array
  long currentSpeed = speedValues[speedIndex];
  
  // Convert to seconds
  float seconds = (float)currentSpeed / 1000000.0;
  
  int x = 1; // Start at column 1 instead of 0
  int y = 1; // Start at row 1 instead of 0
  
  if (seconds < 1.0) {
    // Display as .X (e.g., .5, .3, .4)
    int decimalDigit = (int)((seconds * 10.0) + 0.1);
    if (decimalDigit > 9) decimalDigit = 9;
    
    // Draw dot at position x, row y+5 (1px up from bottom of 7px tall font)
    frame[y + 5][x] = 1;
    x += 2;
    
    // Draw the decimal digit
    drawTextDigit(decimalDigit, x, y);
  } else {
    // Display as whole number (e.g., 1, 2, 16, 50, 60, 120, 180)
    int wholeNumber = (int)(seconds + 0.1);
    
    if (wholeNumber >= 100) {
      // Three digits
      drawTextDigit(wholeNumber / 100, x, y);
      x += 4;
      drawTextDigit((wholeNumber / 10) % 10, x, y);
      x += 4;
      drawTextDigit(wholeNumber % 10, x, y);
    } else if (wholeNumber >= 10) {
      // Two digits
      drawTextDigit(wholeNumber / 10, x, y);
      x += 4;
      drawTextDigit(wholeNumber % 10, x, y);
    } else {
      // Single digit
      drawTextDigit(wholeNumber, x, y);
    }
  }
}

void drawBothMenu() {
  // No border - full 12x8 area available
  // Light up first column (column 0)
  for (int row = 0; row < 8; row++) {
    frame[row][0] = 1;
  }
  
  // Squares at column 1, stacked vertically
  
  // First option: Lock/Unlock both (top box, rows 0-1, cols 1-2)
  if (bothSubSelection != 0 || (bothSubSelection == 0 && blinkState)) {
    frame[0][1] = 1; frame[0][2] = 1;
    frame[1][1] = 1; frame[1][2] = 1;
  }
  
  // Second option: Run (box 2, rows 2-3, cols 1-2)
  if (bothSubSelection != 1 || (bothSubSelection == 1 && blinkState)) {
    frame[2][1] = 1; frame[2][2] = 1;
    frame[3][1] = 1; frame[3][2] = 1;
  }
  
  // Third option: FPS (box 3, rows 4-5, cols 1-2)
  if (bothSubSelection != 2 || (bothSubSelection == 2 && blinkState)) {
    frame[4][1] = 1; frame[4][2] = 1;
    frame[5][1] = 1; frame[5][2] = 1;
  }
  
  // Fourth option: Duration (box 4, rows 6-7, cols 1-2)
  if (bothSubSelection != 3 || (bothSubSelection == 3 && blinkState)) {
    frame[6][1] = 1; frame[6][2] = 1;
    frame[7][1] = 1; frame[7][2] = 1;
  }
  
  // Draw scrolling text for the selected option starting at column 3
  if (bothSubSelection == 0) {
    // "Lock" or "Unlock" based on motor states
    if (motor1Locked && motor2Locked) {
      drawRollingText("Unlock", 3);
    } else {
      drawRollingText("Lock", 3);
    }
  } else if (bothSubSelection == 1) {
    drawRollingText("Run", 3);
  } else if (bothSubSelection == 2) {
    drawRollingText("FPS", 3);
  } else if (bothSubSelection == 3) {
    drawRollingText("Duration", 3);
  }
}

void drawFPSMenu() {
  // No border - full 12x8 area available
  // Display FPS value starting at 1,1
  
  int x = 1;
  int y = 1;
  
  float currentFPS = fpsValues[fpsIndex];
  
  if (currentFPS >= 1.0) {
    // Display as whole number (2 or 1)
    int wholeFPS = (int)(currentFPS + 0.1);
    drawTextDigit(wholeFPS, x, y);
  } else {
    // Display as .5
    frame[y + 5][x] = 1; // Dot 1px up from bottom
    x += 2;
    drawTextDigit(5, x, y);
  }
}

void drawDurationMenu() {
  // No border - full 12x8 area available
  // Display duration cycles value starting at 1,1
  
  int x = 1;
  int y = 1;
  
  int currentCycles = cyclesValues[cyclesIndex];
  
  if (currentCycles >= 10) {
    // Two digits (10, 20)
    drawTextDigit(currentCycles / 10, x, y);
    x += 4;
    drawTextDigit(currentCycles % 10, x, y);
  } else {
    // Single digit (5)
    drawTextDigit(currentCycles, x, y);
  }
}

void runBothCycle() {
  // Run shutter for full 360° rotation, then advance claw, repeat
  // Total cycle time (shutter + claw) must match FPS timing
  int numCycles = cyclesValues[cyclesIndex];
  float cycleTime = 1.0 / fpsValues[fpsIndex]; // Time per cycle in seconds
  long totalCycleTimeMicros = (long)(cycleTime * 1000000.0);
  
  // Claw advance settings
  int clawSteps = 150;
  long clawTimeMicros = 150000; // 150ms for 150 steps
  
  // Remaining time for shutter (200 steps per rotation)
  long shutterTimeMicros = totalCycleTimeMicros - clawTimeMicros;
  
  for (int cycle = 0; cycle < numCycles; cycle++) {
    // Run shutter for full 360° rotation (200 steps)
    runMotor(en1, step1, dir1, 200, shutterTimeMicros);
    
    // Run claw advance
    runMotor(en2, step2, dir2, clawSteps, clawTimeMicros);
  }
}

void drawTextDigit(int digit, int x, int y) {
  // 3x7 font for digits (7 pixels tall)
  byte charData[7];
  
  switch(digit) {
    case 0:
      charData[0] = 0b111;
      charData[1] = 0b101;
      charData[2] = 0b101;
      charData[3] = 0b101;
      charData[4] = 0b101;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    case 1:
      charData[0] = 0b010;
      charData[1] = 0b110;
      charData[2] = 0b010;
      charData[3] = 0b010;
      charData[4] = 0b010;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    case 2:
      charData[0] = 0b111;
      charData[1] = 0b001;
      charData[2] = 0b111;
      charData[3] = 0b100;
      charData[4] = 0b100;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    case 3:
      charData[0] = 0b111;
      charData[1] = 0b001;
      charData[2] = 0b111;
      charData[3] = 0b001;
      charData[4] = 0b001;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    case 4:
      charData[0] = 0b101;
      charData[1] = 0b101;
      charData[2] = 0b111;
      charData[3] = 0b001;
      charData[4] = 0b001;
      charData[5] = 0b001;
      charData[6] = 0b000;
      break;
    case 5:
      charData[0] = 0b111;
      charData[1] = 0b100;
      charData[2] = 0b111;
      charData[3] = 0b001;
      charData[4] = 0b001;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    case 6:
      charData[0] = 0b111;
      charData[1] = 0b100;
      charData[2] = 0b111;
      charData[3] = 0b101;
      charData[4] = 0b101;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    case 7:
      charData[0] = 0b111;
      charData[1] = 0b001;
      charData[2] = 0b001;
      charData[3] = 0b001;
      charData[4] = 0b001;
      charData[5] = 0b001;
      charData[6] = 0b000;
      break;
    case 8:
      charData[0] = 0b111;
      charData[1] = 0b101;
      charData[2] = 0b111;
      charData[3] = 0b101;
      charData[4] = 0b101;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    case 9:
      charData[0] = 0b111;
      charData[1] = 0b101;
      charData[2] = 0b111;
      charData[3] = 0b001;
      charData[4] = 0b001;
      charData[5] = 0b111;
      charData[6] = 0b000;
      break;
    default:
      for (int i = 0; i < 7; i++) charData[i] = 0;
      break;
  }
  
  // Draw the digit (7 pixels tall)
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 3; col++) {
      if (x + col < 12) {
        if (charData[row] & (1 << (2 - col))) {
          frame[y + row][x + col] = 1;
        }
      }
    }
  }
}

void runMotor(int en, int step, int dir, int steps, long totalTimeMicros) {
  digitalWrite(en, HIGH);
  digitalWrite(dir, HIGH);
  
  // Calculate delay per step (divide by 2 because we delay for both HIGH and LOW)
  long delayPerStep = totalTimeMicros / (steps * 2);
  
  for(int x = 0; x < steps; x++) {
    digitalWrite(step, HIGH);
    
    // Handle long delays
    if (delayPerStep > 16000) {
      delay(delayPerStep / 1000);
      delayMicroseconds(delayPerStep % 1000);
    } else {
      delayMicroseconds(delayPerStep);
    }
    
    digitalWrite(step, LOW);
    
    // Handle long delays
    if (delayPerStep > 16000) {
      delay(delayPerStep / 1000);
      delayMicroseconds(delayPerStep % 1000);
    } else {
      delayMicroseconds(delayPerStep);
    }
  }
  
  // Restore motor to locked state if it was locked
  if (en == en1) {
    digitalWrite(en, motor1Locked ? HIGH : LOW);
  } else if (en == en2) {
    digitalWrite(en, motor2Locked ? HIGH : LOW);
  }
}