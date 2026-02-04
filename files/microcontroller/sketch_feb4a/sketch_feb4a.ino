#include "Arduino_LED_Matrix.h"

/* BUTTON LAYOUT REFERENCE:
 * 
 * IDLE MODE:
 *   Button 1 (pin 8):  Enter MOTOR 1 MODE
 *   Button 2 (pin 9):  Enter MOTOR 2 MODE
 *   Button 3 (pin 10): (unused)
 *   Button 4 (pin 11): (unused)
 * 
 * MOTOR 1 MODE:
 *   Button 1: RUN Motor 1
 *   Button 2: TOGGLE LOCK Motor 1
 *   Button 3: Enter SPEED submenu for Motor 1
 *   Button 4: BACK to IDLE
 * 
 * MOTOR 1 SPEED SUBMENU:
 *   Button 1: Speed UP for Motor 1 (cycles 0.5s→1s→2s→4s→8s→16s)
 *   Button 2: Speed DOWN for Motor 1 (cycles 16s→8s→4s→2s→1s→0.5s)
 *   Button 3: (unused)
 *   Button 4: BACK to MOTOR 1 MODE
 * 
 * MOTOR 2 MODE:
 *   Button 1: RUN Motor 2
 *   Button 2: TOGGLE LOCK Motor 2
 *   Button 3: Enter SPEED submenu for Motor 2
 *   Button 4: BACK to IDLE
 * 
 * MOTOR 2 SPEED SUBMENU:
 *   Button 1: Speed UP for Motor 2 (cycles 0.5s→1s→2s→4s→8s→16s)
 *   Button 2: Speed DOWN for Motor 2 (cycles 16s→8s→4s→2s→1s→0.5s)
 *   Button 3: (unused)
 *   Button 4: BACK to MOTOR 2 MODE
 * 
 * DISPLAY (Folder-based UI):
 *   Layout (8x12 LED matrix):
 *     Rows 0-2: Motor 1 folder (3 pixels high)
 *     Row 3:    Motor 1 speed indicator (2x1 LED pairs, 1-6 pairs)
 *     Rows 4-6: Motor 2 folder (3 pixels high)
 *     Row 7:    Motor 2 speed indicator (2x1 LED pairs, 1-6 pairs)
 *   
 *   IDLE MODE:
 *     - Both folders closed, speed indicators visible below each
 *   
 *   MOTOR MODE (folder open):
 *     - Selected folder shows "open" with 3 menu item icons:
 *       · RUN icon (play triangle, filled when active)
 *       · LOCK icon (solid when locked, outline when unlocked)
 *       · SPEED icon (bars)
 *     - Speed indicator always visible below folder
 *   
 *   SPEED SUBMENU:
 *     - Open folder with speed indicator below showing current setting
 *     - Speed shown as 2x1 LED pairs: 1 pair=0.5s, 2=1s, 3=2s, 4=4s, 5=8s, 6=16s
 */

ArduinoLEDMatrix matrix;

// Motor 1 Pins
const int en1 = 2, step1 = 3, dir1 = 4;

// Motor 2 Pins
const int en2 = 5, step2 = 6, dir2 = 7;

// Button Pins
const int btn1 = 8, btn2 = 9, btn3 = 10, btn4 = 11;

// State Machine
enum State { IDLE, MOTOR1_MODE, MOTOR1_SPEED, MOTOR2_MODE, MOTOR2_SPEED };
State currentState = IDLE;

// Motor States
bool lock1 = false;
bool lock2 = false;

// Speed Settings (each motor has its own speed)
// Speed index: 0=0.5s, 1=1s, 2=2s, 3=4s, 4=8s, 5=16s (6 total speeds)
int speedIndex1 = 1; // Motor 1 starts at 1s
int speedIndex2 = 1; // Motor 2 starts at 1s

/* SPEED CALCULATION MATH:
 * Given: 200 steps total, need timing from step 31 (55°) to step 169 (305°) = 139 steps
 * 
 * For desired duration D (in seconds):
 * - Total microseconds: D × 1,000,000
 * - Microseconds per step: (D × 1,000,000) / 139
 * - Each step has 2 phases (HIGH + LOW), so delay per phase: (D × 1,000,000) / (139 × 2)
 * 
 * Simplified: delay_microseconds = (D × 1,000,000) / 278
 * 
 * For our speeds:
 * 0.5s: (0.5 × 1,000,000) / 278 = 1,799 μs
 * 1s:   (1.0 × 1,000,000) / 278 = 3,597 μs
 * 2s:   (2.0 × 1,000,000) / 278 = 7,194 μs
 * 4s:   (4.0 × 1,000,000) / 278 = 14,388 μs
 * 8s:   (8.0 × 1,000,000) / 278 = 28,776 μs
 * 16s:  (16.0 × 1,000,000) / 278 = 57,554 μs
 */
const unsigned long speedDelays[6] = {
  1799,    // 0.5s
  3597,    // 1s
  7194,    // 2s
  14388,   // 4s
  28776,   // 8s
  57554    // 16s
};

// Display
uint8_t frame[8][12] = {0};

// Button debouncing
bool lastBtn1 = false, lastBtn2 = false, lastBtn3 = false, lastBtn4 = false;

void setup() {
  matrix.begin();
  
  // Setup motor pins
  int outputs[] = {en1, step1, dir1, en2, step2, dir2};
  for(int p : outputs) pinMode(p, OUTPUT);
  
  // Setup button pins
  int inputs[] = {btn1, btn2, btn3, btn4};
  for(int p : inputs) pinMode(p, INPUT_PULLUP);

  // Start both motors unlocked (sleep mode)
  digitalWrite(en1, LOW);
  digitalWrite(en2, LOW);
}

void loop() {
  // Read buttons with debouncing
  bool btn1Pressed = (digitalRead(btn1) == LOW) && !lastBtn1;
  bool btn2Pressed = (digitalRead(btn2) == LOW) && !lastBtn2;
  bool btn3Pressed = (digitalRead(btn3) == LOW) && !lastBtn3;
  bool btn4Pressed = (digitalRead(btn4) == LOW) && !lastBtn4;
  
  lastBtn1 = (digitalRead(btn1) == LOW);
  lastBtn2 = (digitalRead(btn2) == LOW);
  lastBtn3 = (digitalRead(btn3) == LOW);
  lastBtn4 = (digitalRead(btn4) == LOW);

  // State machine logic
  switch(currentState) {
    case IDLE:
      if (btn1Pressed) {
        currentState = MOTOR1_MODE;
      }
      else if (btn2Pressed) {
        currentState = MOTOR2_MODE;
      }
      // Button 3 unused in IDLE
      // Button 4 unused in IDLE
      break;

    case MOTOR1_MODE:
      if (btn1Pressed) {
        // RUN Motor 1
        runMotor(en1, step1, dir1, lock1, speedIndex1);
      }
      else if (btn2Pressed) {
        // TOGGLE LOCK Motor 1
        lock1 = !lock1;
        digitalWrite(en1, lock1 ? HIGH : LOW);
      }
      else if (btn3Pressed) {
        // Enter SPEED submenu for Motor 1
        currentState = MOTOR1_SPEED;
      }
      else if (btn4Pressed) {
        // BACK to IDLE
        currentState = IDLE;
      }
      break;

    case MOTOR1_SPEED:
      if (btn1Pressed) {
        // Speed UP for Motor 1
        speedIndex1 = (speedIndex1 + 1) % 6;
      }
      else if (btn2Pressed) {
        // Speed DOWN for Motor 1
        speedIndex1 = (speedIndex1 - 1 + 6) % 6;
      }
      else if (btn4Pressed) {
        // BACK to MOTOR1_MODE
        currentState = MOTOR1_MODE;
      }
      break;

    case MOTOR2_MODE:
      if (btn1Pressed) {
        // RUN Motor 2
        runMotor(en2, step2, dir2, lock2, speedIndex2);
      }
      else if (btn2Pressed) {
        // TOGGLE LOCK Motor 2
        lock2 = !lock2;
        digitalWrite(en2, lock2 ? HIGH : LOW);
      }
      else if (btn3Pressed) {
        // Enter SPEED submenu for Motor 2
        currentState = MOTOR2_SPEED;
      }
      else if (btn4Pressed) {
        // BACK to IDLE
        currentState = IDLE;
      }
      break;

    case MOTOR2_SPEED:
      if (btn1Pressed) {
        // Speed UP for Motor 2
        speedIndex2 = (speedIndex2 + 1) % 6;
      }
      else if (btn2Pressed) {
        // Speed DOWN for Motor 2
        speedIndex2 = (speedIndex2 - 1 + 6) % 6;
      }
      else if (btn4Pressed) {
        // BACK to MOTOR2_MODE
        currentState = MOTOR2_MODE;
      }
      break;
  }

  drawInterface();
  delay(50); // Small delay for stability
}

void runMotor(int en, int step, int dir, bool isLocked, int motorSpeedIndex) {
  digitalWrite(en, HIGH); // Wake up motor
  digitalWrite(dir, HIGH);
  
  unsigned long delayTime = speedDelays[motorSpeedIndex];
  
  // Run 200 steps at constant speed
  for(int x = 0; x < 200; x++) {
    digitalWrite(step, HIGH);
    safeDelay(delayTime);
    digitalWrite(step, LOW);
    safeDelay(delayTime);
  }
  
  // Return to locked/unlocked state
  digitalWrite(en, isLocked ? HIGH : LOW);
}

// Safe delay function that handles both small and large delays
// delayMicroseconds() can be unreliable above ~3000 microseconds on some boards
void safeDelay(unsigned long microseconds) {
  if (microseconds > 3000) {
    // Use delay() for milliseconds part
    unsigned long ms = microseconds / 1000;
    unsigned long us = microseconds % 1000;
    delay(ms);
    if (us > 0) {
      delayMicroseconds(us);
    }
  } else {
    delayMicroseconds(microseconds);
  }
}

void drawInterface() {
  memset(frame, 0, sizeof(frame));
  
  switch(currentState) {
    case IDLE:
      // Motor 1: closed folder + speed indicator
      drawFolder(0, 0, false);
      drawSpeedBar(3, speedIndex1);
      
      // Motor 2: closed folder + speed indicator
      drawFolder(0, 4, false);
      drawSpeedBar(7, speedIndex2);
      break;
      
    case MOTOR1_MODE:
      // Motor 1: open folder with menu items
      drawFolder(0, 0, true);
      drawMenuItem(5, 0, 0, (digitalRead(btn1) == LOW)); // RUN
      drawMenuItem(7, 0, 1, lock1);                       // LOCK
      drawMenuItem(9, 0, 2, false);                       // SPEED
      drawSpeedBar(3, speedIndex1);
      
      // Motor 2: closed folder + speed indicator
      drawFolder(0, 4, false);
      drawSpeedBar(7, speedIndex2);
      break;
      
    case MOTOR1_SPEED:
      // Motor 1: open folder showing speed adjustment
      drawFolder(0, 0, true);
      drawSpeedBar(3, speedIndex1); // Highlight speed
      
      // Motor 2: closed folder + speed indicator
      drawFolder(0, 4, false);
      drawSpeedBar(7, speedIndex2);
      break;
      
    case MOTOR2_MODE:
      // Motor 1: closed folder + speed indicator
      drawFolder(0, 0, false);
      drawSpeedBar(3, speedIndex1);
      
      // Motor 2: open folder with menu items
      drawFolder(0, 4, true);
      drawMenuItem(5, 4, 0, (digitalRead(btn1) == LOW)); // RUN
      drawMenuItem(7, 4, 1, lock2);                       // LOCK
      drawMenuItem(9, 4, 2, false);                       // SPEED
      drawSpeedBar(7, speedIndex2);
      break;
      
    case MOTOR2_SPEED:
      // Motor 1: closed folder + speed indicator
      drawFolder(0, 0, false);
      drawSpeedBar(3, speedIndex1);
      
      // Motor 2: open folder showing speed adjustment
      drawFolder(0, 4, true);
      drawSpeedBar(7, speedIndex2); // Highlight speed
      break;
  }
  
  matrix.renderBitmap(frame, 8, 12);
}

// Draw a folder icon (closed or open) - 3 pixels high
void drawFolder(int x, int y, bool open) {
  if (open) {
    // Open folder shape (3 high)
    frame[y][x+1] = 1; frame[y][x+2] = 1; // Tab
    frame[y+1][x] = 1; frame[y+1][x+1] = 1; frame[y+1][x+2] = 1; frame[y+1][x+3] = 1; // Top (open)
    frame[y+2][x] = 1; frame[y+2][x+1] = 1; frame[y+2][x+2] = 1; frame[y+2][x+3] = 1; // Bottom
  } else {
    // Closed folder shape (3 high)
    frame[y][x+1] = 1; frame[y][x+2] = 1; // Tab
    frame[y+1][x] = 1; frame[y+1][x+1] = 1; frame[y+1][x+2] = 1; frame[y+1][x+3] = 1; // Middle (filled)
    frame[y+2][x] = 1; frame[y+2][x+1] = 1; frame[y+2][x+2] = 1; frame[y+2][x+3] = 1; // Bottom (filled)
  }
}

// Draw speed indicator as 2x1 LED blocks (6 speeds = 6 pairs max)
// row: which row to draw on (3 for motor1, 7 for motor2)
void drawSpeedBar(int row, int speedIdx) {
  // Draw (speedIdx + 1) pairs of 2x1 LEDs
  int numPairs = speedIdx + 1; // 1 to 6 pairs
  for (int i = 0; i < numPairs && i < 6; i++) {
    int x = i * 2; // Each pair is 2 pixels wide
    frame[row][x] = 1;
    frame[row][x + 1] = 1;
  }
}

// Draw a menu item icon
// itemType: 0=RUN, 1=LOCK, 2=SPEED
void drawMenuItem(int x, int y, int itemType, bool active) {
  switch(itemType) {
    case 0: // RUN - Play triangle (2 high)
      frame[y+1][x] = 1;
      frame[y+2][x] = 1;
      if (active) {
        frame[y+1][x+1] = 1; // Filled when active
        frame[y+2][x+1] = 1;
      }
      break;
      
    case 1: // LOCK - Lock icon (2 high)
      if (active) {
        // Locked (solid 2x2)
        frame[y+1][x] = 1; frame[y+1][x+1] = 1;
        frame[y+2][x] = 1; frame[y+2][x+1] = 1;
      } else {
        // Unlocked (outline)
        frame[y+1][x] = 1; frame[y+1][x+1] = 1;
        frame[y+2][x] = 1; frame[y+2][x+1] = 1;
      }
      break;
      
    case 2: // SPEED - Three horizontal bars (2 high)
      frame[y+1][x] = 1; frame[y+1][x+1] = 1;
      frame[y+2][x] = 1; frame[y+2][x+1] = 1;
      break;
  }
}

// Draw speed indicator as horizontal bars
void drawSpeedIndicator(int x, int y, int speedIdx) {
  // Draw (speedIdx + 1) bars horizontally
  int numBars = speedIdx + 1; // 1 to 6 bars
  for (int i = 0; i < numBars && i < 6; i++) {
    frame[y][x + i] = 1;
    frame[y + 1][x + i] = 1;
  }
}

void drawBox(int x, int y, bool filled) {
  for(int i=0; i<4; i++) {
    frame[y][x + i] = 1;         // Top
    frame[y + 3][x + i] = 1;     // Bottom
    frame[y + i][x] = 1;         // Left
    frame[y + i][x + 3] = 1;     // Right
  }
  if (filled) {
    frame[y+1][x+1] = 1; 
    frame[y+1][x+2] = 1;
    frame[y+2][x+1] = 1; 
    frame[y+2][x+2] = 1;
  }
}