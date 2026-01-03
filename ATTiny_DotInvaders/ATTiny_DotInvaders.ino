#include <avr/pgmspace.h>

// MAX7219 Register Addresses
#define MAX7219_REG_NOOP 0x00
#define MAX7219_REG_DIGIT0 0x01
#define MAX7219_REG_DIGIT1 0x02
#define MAX7219_REG_DIGIT2 0x03
#define MAX7219_REG_DIGIT3 0x04
#define MAX7219_REG_DIGIT4 0x05
#define MAX7219_REG_DIGIT5 0x06
#define MAX7219_REG_DIGIT6 0x07
#define MAX7219_REG_DIGIT7 0x08

#define MAX7219_REG_DECODE_MODE 0x09
#define MAX7219_REG_INTENSITY 0x0A  // brightness 0-15
#define MAX7219_REG_SCAN_LIMIT 0x0B
#define MAX7219_REG_SHUTDOWN 0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F

// Pin definitions (based on dotris hardware)
const uint8_t DIN = 0;
const uint8_t CLK = 1;
const uint8_t CS = 2;
const uint8_t RIGHT_BUTTON = 3;
const uint8_t LEFT_BUTTON = 4;

// Game configuration
#define STARTING_LIVES 3
#define KILLS_PER_LEVEL 5
#define NUM_SPEED_LEVELS 8

// Speed levels (drop interval in ms) - stored in PROGMEM to save RAM
const uint16_t speedLevels[] PROGMEM = {
  400,  // Level 1: slow
  350,  // Level 2
  300,  // Level 3
  260,  // Level 4
  230,  // Level 5
  200,  // Level 6
  180,  // Level 7
  160,  // Level 8+: max speed
};

// Game state
byte matrix[8];
uint8_t gunPos = 3;  // Gun position at bottom row (column 0-7)
uint8_t invaderCol = 0;  // Invader column position
uint8_t invaderRow = 0;  // Invader row position
bool bulletActive = false;
uint8_t bulletCol = 0;
uint8_t bulletRow = 0;
uint16_t score = 0;

// Level progression
uint8_t lives = STARTING_LIVES;
uint8_t currentLevel = 1;
uint8_t levelKills = 0;

// Timing variables
uint16_t invaderDropCounter = 0;
uint16_t bulletMoveCounter = 0;
bool lastFireState = false;

// Key repeat variables
uint16_t keyHoldTimer = 0;
uint8_t heldButton = 0;  // 0=none, 1=left, 2=right - currently held for repeat
const uint16_t KEY_REPEAT_DELAY = 250;  // Initial delay before repeat starts (ms)
const uint16_t KEY_REPEAT_RATE = 50;    // Repeat interval once holding (ms)

// Button state tracking for release detection
bool lastLeftPressed = false;
bool lastRightPressed = false;
bool chordedThisPress = false;  // True if we fired during this button hold

// Send a byte to MAX7219 via bit-banged SPI
void sendByte(uint8_t b) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(CLK, LOW);
    digitalWrite(DIN, (b >> i) & 1);
    digitalWrite(CLK, HIGH);
  }
}

// Send command to MAX7219
void sendCmd(uint8_t addr, uint8_t data) {
  digitalWrite(CS, LOW);
  sendByte(addr);
  sendByte(data);
  digitalWrite(CS, HIGH);
}

// Update entire display from matrix buffer
void updateDisplay() {
  for (uint8_t row = 0; row < 8; row++) {
    sendCmd(row + 1, matrix[row]);
  }
}

// Get current drop interval based on level (reads from PROGMEM)
uint16_t getDropInterval() {
  uint8_t idx = currentLevel - 1;
  if (idx >= NUM_SPEED_LEVELS) idx = NUM_SPEED_LEVELS - 1;
  return pgm_read_word(&speedLevels[idx]);
}

// Level-up flash effect
void flashLevelUp() {
  for (uint8_t i = 0; i < 3; i++) {
    for (uint8_t row = 0; row < 8; row++) {
      sendCmd(row + 1, 0xFF);
    }
    delay(50);
    for (uint8_t row = 0; row < 8; row++) {
      sendCmd(row + 1, 0x00);
    }
    delay(50);
  }
}

// Show remaining lives (flash N times)
void showLivesRemaining() {
  for (uint8_t i = 0; i < lives; i++) {
    for (uint8_t row = 0; row < 8; row++) matrix[row] = 0xFF;
    updateDisplay();
    delay(200);
    for (uint8_t row = 0; row < 8; row++) matrix[row] = 0x00;
    updateDisplay();
    delay(200);
  }
  delay(300);
}

// Forward declaration
void gameOver();

// Lose a life - returns true if game continues, false if game over
bool loseLife() {
  lives--;

  if (lives == 0) {
    gameOver();
    return false;
  }

  // Show remaining lives
  showLivesRemaining();

  // Reset invader position
  invaderCol = random(0, 8);
  invaderRow = 0;
  invaderDropCounter = 0;

  return true;
}

// Initialize MAX7219
void setup() {
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);

  // Initialize MAX7219
  sendCmd(MAX7219_REG_DISPLAY_TEST, 0x00);
  sendCmd(MAX7219_REG_DECODE_MODE, 0x00);
  sendCmd(MAX7219_REG_SCAN_LIMIT, 0x07);
  sendCmd(MAX7219_REG_INTENSITY, 0x08);  // brightness
  sendCmd(MAX7219_REG_SHUTDOWN, 0x01);

  // Clear display
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
  updateDisplay();

  // Initialize random seed - combine sources for better entropy
  randomSeed(analogRead(A0) ^ millis());

  // Show starting lives
  showLivesRemaining();

  // Spawn first invader
  spawnInvader();
}

// Spawn a new invader at a random column
void spawnInvader() {
  invaderCol = random(0, 8);
  invaderRow = 0;
}

// Main game loop
void loop() {
  uint16_t invaderDropInterval = getDropInterval();  // Level-based speed
  uint16_t bulletMoveInterval = 30;  // Fast bullet movement

  // Clear matrix for fresh frame
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;

  // Draw gun at bottom row
  matrix[7] |= (1 << gunPos);

  // Draw invader if still alive
  if (invaderRow < 8) {
    matrix[invaderRow] |= (1 << invaderCol);
  }

  // Draw bullet if active
  if (bulletActive) {
    if (bulletRow < 8) {
      matrix[bulletRow] |= (1 << bulletCol);
    }
  }

  // Update display
  updateDisplay();

  // Small delay for game timing
  delay(1);

  // Handle button input for gun movement and firing
  bool rightPressed = (digitalRead(RIGHT_BUTTON) == LOW);
  bool leftPressed = (digitalRead(LEFT_BUTTON) == LOW);

  // Chording both buttons fires a bullet
  bool firePressed = rightPressed && leftPressed;

  if (firePressed && !lastFireState && !bulletActive) {
    // Fire a bullet
    bulletActive = true;
    bulletCol = gunPos;
    bulletRow = 6;  // Start just above the gun
    chordedThisPress = true;  // Mark that we fired during this press
  }
  lastFireState = firePressed;

  // Detect button releases for single-tap movement
  // Move on release ONLY if we didn't chord (fire) during this press
  if (!firePressed && !chordedThisPress) {
    // Left button released - was it a single tap?
    if (lastLeftPressed && !leftPressed && !rightPressed) {
      // Left was just released and right isn't held - move left
      if (gunPos < 7) gunPos++;
      heldButton = 0;
      keyHoldTimer = 0;
    }
    // Right button released - was it a single tap?
    if (lastRightPressed && !rightPressed && !leftPressed) {
      // Right was just released and left isn't held - move right
      if (gunPos > 0) gunPos--;
      heldButton = 0;
      keyHoldTimer = 0;
    }
  }

  // Reset chord flag when all buttons released
  if (!leftPressed && !rightPressed) {
    chordedThisPress = false;
  }

  // Handle key repeat for held buttons (only single button, not chord)
  if (!firePressed) {
    uint8_t currentButton = 0;
    if (leftPressed && !rightPressed) currentButton = 1;
    else if (rightPressed && !leftPressed) currentButton = 2;

    if (currentButton != 0) {
      if (currentButton == heldButton) {
        // Same button held - check for repeat
        keyHoldTimer++;
        if (keyHoldTimer == KEY_REPEAT_DELAY) {
          // First repeat after initial delay
          if (currentButton == 1 && gunPos < 7) gunPos++;
          else if (currentButton == 2 && gunPos > 0) gunPos--;
        } else if (keyHoldTimer > KEY_REPEAT_DELAY) {
          // Continuous repeat at faster rate
          if ((keyHoldTimer - KEY_REPEAT_DELAY) % KEY_REPEAT_RATE == 0) {
            if (currentButton == 1 && gunPos < 7) gunPos++;
            else if (currentButton == 2 && gunPos > 0) gunPos--;
          }
        }
      } else {
        // New button or switching - start hold timer
        heldButton = currentButton;
        keyHoldTimer = 0;
      }
    } else {
      // No single button held
      heldButton = 0;
      keyHoldTimer = 0;
    }
  }

  // Save button states for next frame
  lastLeftPressed = leftPressed;
  lastRightPressed = rightPressed;

  // Move invader down
  invaderDropCounter++;
  if (invaderDropCounter >= invaderDropInterval) {
    invaderDropCounter = 0;
    invaderRow++;
  }

  // Move bullet up
  if (bulletActive) {
    bulletMoveCounter++;
    if (bulletMoveCounter >= bulletMoveInterval) {
      bulletMoveCounter = 0;

      if (bulletRow > 0) {
        bulletRow--;
      } else {
        // Bullet went off screen
        bulletActive = false;
      }
    }
  }

  // Check for collision (bullet hits invader)
  if (bulletActive && bulletRow == invaderRow && bulletCol == invaderCol) {
    // Hit! Apply score multiplier (level = multiplier)
    bulletActive = false;
    score += currentLevel;
    levelKills++;

    // Flash the hit
    for (uint8_t i = 0; i < 3; i++) {
      sendCmd(invaderRow + 1, 0xFF);
      delay(50);
      sendCmd(invaderRow + 1, 0x00);
      delay(50);
    }

    // Check for level up
    if (levelKills >= KILLS_PER_LEVEL && currentLevel < NUM_SPEED_LEVELS) {
      currentLevel++;
      levelKills = 0;
      flashLevelUp();
    }

    // Spawn new invader
    spawnInvader();
    invaderDropCounter = 0;
  }

  // Check if invader reached the bottom (lose a life)
  if (invaderRow >= 7) {
    if (!loseLife()) {
      return;  // Game over was triggered
    }
  }
}

// Game over sequence
void gameOver() {
  // Flash entire display (game over effect)
  for (uint8_t flash = 0; flash < 4; flash++) {
    for (uint8_t i = 0; i < 8; i++) matrix[i] = 0xFF;
    updateDisplay();
    delay(150);
    for (uint8_t i = 0; i < 8; i++) matrix[i] = 0x00;
    updateDisplay();
    delay(150);
  }

  // Display final level reached (columns lit = level)
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
  uint8_t levelDisplay = currentLevel;
  if (levelDisplay > 8) levelDisplay = 8;
  for (uint8_t i = 0; i < levelDisplay; i++) {
    for (uint8_t row = 0; row < 8; row++) {
      matrix[row] |= (1 << i);
    }
  }
  updateDisplay();
  delay(2000);

  // Reset game state
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
  updateDisplay();
  score = 0;
  lives = STARTING_LIVES;
  currentLevel = 1;
  levelKills = 0;
  gunPos = 3;
  bulletActive = false;
  invaderDropCounter = 0;
  bulletMoveCounter = 0;
  keyHoldTimer = 0;
  heldButton = 0;
  lastLeftPressed = false;
  lastRightPressed = false;
  chordedThisPress = false;

  // Show lives and start new game
  showLivesRemaining();
  spawnInvader();
}
