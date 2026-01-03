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
#define MAX7219_REG_INTENSITY 0x0A  // brightness 0–15
#define MAX7219_REG_SCAN_LIMIT 0x0B
#define MAX7219_REG_SHUTDOWN 0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F

// Pin definitions (based on dotris hardware)
const uint8_t DIN = 0;
const uint8_t CLK = 1;
const uint8_t CS = 2;
const uint8_t RIGHT_BUTTON = 3;
const uint8_t LEFT_BUTTON = 4;

// Game state
byte matrix[8];
uint8_t gunPos = 3;  // Gun position at bottom row (column 0-7)
uint8_t invaderCol = 0;  // Invader column position
uint8_t invaderRow = 0;  // Invader row position
bool bulletActive = false;
uint8_t bulletCol = 0;
uint8_t bulletRow = 0;
uint16_t score = 0;
uint8_t speed = 1;

// Timing variables
uint16_t invaderDropCounter = 0;
uint16_t bulletMoveCounter = 0;
uint16_t buttonDebounceTimer = 0;
bool lastFireState = false;

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

  // Initialize random seed using millis() for better randomness
  // The unpredictable power-on time provides entropy
  randomSeed(millis());
  
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
  uint16_t invaderDropInterval = 300 - (speed * 10);  // Gets faster with score
  uint16_t bulletMoveInterval = 30;  // Fast bullet movement
  uint16_t moveInterval = 100;  // Button response rate
  
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
  buttonDebounceTimer++;
  if (buttonDebounceTimer >= moveInterval) {
    buttonDebounceTimer = 0;
    
    bool rightPressed = (digitalRead(RIGHT_BUTTON) == LOW);
    bool leftPressed = (digitalRead(LEFT_BUTTON) == LOW);
    
    // Chording both buttons fires a bullet
    bool firePressed = rightPressed && leftPressed;
    
    if (firePressed && !lastFireState && !bulletActive) {
      // Fire a bullet
      bulletActive = true;
      bulletCol = gunPos;
      bulletRow = 6;  // Start just above the gun
    } else if (!firePressed) {
      // Move gun left or right (only if not chording)
      if (leftPressed && gunPos < 7) {
        gunPos++;
      } else if (rightPressed && gunPos > 0) {
        gunPos--;
      }
    }
    
    lastFireState = firePressed;
  }
  
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
    // Hit!
    bulletActive = false;
    score++;
    speed++;
    if (speed > 20) speed = 20;
    
    // Flash the hit
    for (uint8_t i = 0; i < 3; i++) {
      sendCmd(invaderRow + 1, 0xFF);
      delay(50);
      sendCmd(invaderRow + 1, 0x00);
      delay(50);
    }
    
    // Spawn new invader
    spawnInvader();
    invaderDropCounter = 0;
  }
  
  // Check if invader reached the bottom (game over)
  if (invaderRow >= 7) {
    gameOver();
    return;
  }
}

// Game over sequence
void gameOver() {
  // Flash entire display
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0xFF;
  updateDisplay();
  delay(200);
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0x00;
  updateDisplay();
  delay(200);
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0xFF;
  updateDisplay();
  delay(200);
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0x00;
  updateDisplay();
  
  // Display score pattern (simplified - number of rows lit = score)
  uint8_t scoreDisplay = score;
  if (scoreDisplay > 8) scoreDisplay = 8;
  for (uint8_t i = 0; i < scoreDisplay; i++) {
    matrix[7 - i] = 0xFF;
  }
  updateDisplay();
  delay(2000);
  
  // Reset game
  for (uint8_t i = 0; i < 8; i++) matrix[i] = 0;
  updateDisplay();
  score = 0;
  speed = 1;
  gunPos = 3;
  bulletActive = false;
  invaderDropCounter = 0;
  bulletMoveCounter = 0;
  spawnInvader();
  delay(500);
}
