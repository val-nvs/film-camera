#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Motor 1 Pins
const int en1 = 2, step1 = 3, dir1 = 4;
const int btnMove1 = 8, btnLock1 = 9;

// Motor 2 Pins
const int en2 = 5, step2 = 6, dir2 = 7;
const int btnMove2 = 10, btnLock2 = 11;

bool lock1 = false;
bool lock2 = false;
uint8_t frame[8][12] = {0};

void setup() {
  matrix.begin();
  
  int outputs[] = {en1, step1, dir1, en2, step2, dir2};
  for(int p : outputs) pinMode(p, OUTPUT);
  
  int inputs[] = {btnMove1, btnLock1, btnMove2, btnLock2};
  for(int p : inputs) pinMode(p, INPUT_PULLUP);

  // Start both unlocked (Sleep mode)
  digitalWrite(en1, LOW);
  digitalWrite(en2, LOW);
}

void loop() {
  // --- MOTOR 1 LOGIC ---
  handleMotor(btnMove1, btnLock1, lock1, en1, step1, dir1, 0); // Row 0

  // --- MOTOR 2 LOGIC ---
  handleMotor(btnMove2, btnLock2, lock2, en2, step2, dir2, 4); // Row 4 (2nd row of boxes)

  drawInterface();
}

void handleMotor(int moveBtn, int lockBtn, bool &isLocked, int en, int step, int dir, int yOffset) {
  // Toggle Lock
  static bool lastLockState[12] = {false}; // Simple debouncing array
  bool currentLockBtn = (digitalRead(lockBtn) == LOW);
  
  if (currentLockBtn && !lastLockState[lockBtn]) {
    isLocked = !isLocked;
    digitalWrite(en, isLocked ? HIGH : LOW);
    delay(50); 
  }
  lastLockState[lockBtn] = currentLockBtn;

  // Move Motor
  if (digitalRead(moveBtn) == LOW) {
    digitalWrite(en, HIGH); // Wake up
    digitalWrite(dir, HIGH);
    
    // Draw "Moving" state for this motor
    drawBox(0, yOffset, true); 
    matrix.renderBitmap(frame, 8, 12);

    for(int x = 0; x < 400; x++) {
      digitalWrite(step, HIGH);
      delayMicroseconds(1200);
      digitalWrite(step, LOW);
      delayMicroseconds(1200);
    }
    digitalWrite(en, isLocked ? HIGH : LOW);
  }
}

void drawInterface() {
  memset(frame, 0, sizeof(frame));
  // Motor 1 (Top Row)
  drawBox(0, 0, (digitalRead(btnMove1) == LOW));
  drawBox(4, 0, lock1);
  
  // Motor 2 (Bottom Row)
  drawBox(0, 4, (digitalRead(btnMove2) == LOW));
  drawBox(4, 4, lock2);
  
  matrix.renderBitmap(frame, 8, 12);
}

void drawBox(int x, int y, bool filled) {
  for(int i=0; i<4; i++) {
    frame[y][x + i] = 1;         // Top
    frame[y + 3][x + i] = 1;     // Bottom
    frame[y + i][x] = 1;         // Left
    frame[y + i][x + 3] = 1;     // Right
  }
  if (filled) {
    frame[y+1][x+1] = 1; frame[y+1][x+2] = 1;
    frame[y+2][x+1] = 1; frame[y+2][x+2] = 1;
  }
}