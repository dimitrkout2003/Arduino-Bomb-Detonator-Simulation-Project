#include <IRremote.hpp>

// ========== CONSTANTS & CONFIGURATION ==========
// Pin definitions
const int BUTTON_PIN = 2;
const int LED_PIN = 8;
const int BUZZER_PIN = 9;
const int PHOTO_RESISTOR_PIN = A0;
const int IR_RECEIVER_PIN = 10;
const int POTENTIOMETER_PIN = A1;
const int DIGIT_1_PIN = A5;
const int DIGIT_2_PIN = A4;

// 7-segment display pins (A-G, DP)
const int SEGMENT_PINS[8] = {11, 7, 4, 5, 6, 12, 13, 3};

// Game configuration
const int INITIAL_COUNTDOWN = 30;
const int PHOTO_RESISTOR_TRIGGER = 200;
const int POT_SAFE_MIN = 490;
const int POT_SAFE_MAX = 550;
const unsigned long DEACTIVATE_DELAY = 5000;
const int BLINK_THRESHOLD = 11;
const unsigned long DISPLAY_REFRESH_INTERVAL = 2; // ms per digit

// Corrected 7-segment patterns for common cathode (0=ON, 1=OFF)
const byte SEGMENT_PATTERNS[10][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};

// ========== GAME STATE VARIABLES ==========
enum GameState {
  STATE_IDLE,
  STATE_COUNTING,
  STATE_DEACTIVATED,
  STATE_DETONATED
};

// Variables
int countdownValue;
unsigned long lastSecondUpdate;
unsigned long deactivateTimer;
unsigned long lastDisplayUpdate;
GameState currentState;
int lastButtonState;
bool buttonPressed;

// ========== SETUP ==========
void setup() {
  // Initialize pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PHOTO_RESISTOR_PIN, INPUT);
  pinMode(POTENTIOMETER_PIN, INPUT);
  
  // Initialize display
  for (int i = 0; i < 8; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
  }
  pinMode(DIGIT_1_PIN, OUTPUT);
  pinMode(DIGIT_2_PIN, OUTPUT);
  
  // Initialize IR receiver
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);
  
  // Initialize serial
  Serial.begin(9600);
  Serial.println("Bomb Defusal Game Ready - Push button to start");
  
  // Initial state
  resetGame();
}

// ========== MAIN LOOP ==========
void loop() {
  unsigned long currentTime = millis();
  
  // Read inputs
  readButton();
  readIRRemote();
  
  // State machine
  switch (currentState) {
    case STATE_IDLE:
      handleIdleState();
      break;
      
    case STATE_COUNTING:
      handleCountingState(currentTime);
      break;
      
    case STATE_DEACTIVATED:
      // Game completed successfully
      displayNumber(countdownValue);
      break;
      
    case STATE_DETONATED:
      // Game over state
      digitalWrite(BUZZER_PIN, HIGH);
      displayNumber(0);
      break;
  }
  
  // Always update display (non-blocking)
  updateDisplay(currentTime);
}

// ========== STATE HANDLERS ==========
void handleIdleState() {
  if (buttonPressed) {
    startGame();
    buttonPressed = false;
  }
}

void handleCountingState(unsigned long currentTime) {
  // Update countdown every second
  if (currentTime - lastSecondUpdate >= 1000) {
    lastSecondUpdate = currentTime;
    countdownValue--;
    
    Serial.print("Countdown: ");
    Serial.println(countdownValue);
    
    // Update warning LED
    digitalWrite(LED_PIN, countdownValue <= BLINK_THRESHOLD);
    
    // Check for game over conditions
    checkGameConditions(currentTime);
  }
}

// ========== GAME LOGIC ==========
void startGame() {
  currentState = STATE_COUNTING;
  countdownValue = INITIAL_COUNTDOWN;
  lastSecondUpdate = millis();
  deactivateTimer = millis();
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Countdown started! 30 seconds until detonation!");
  Serial.println("Defuse by: IR remote command 22 OR keep potentiometer in safe zone for 5 seconds");
}

void resetGame() {
  currentState = STATE_IDLE;
  countdownValue = INITIAL_COUNTDOWN;
  lastButtonState = LOW;
  buttonPressed = false;
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void checkGameConditions(unsigned long currentTime) {
  // Check countdown reached zero
  if (countdownValue <= 0) {
    detonateBomb("TIME'S UP! BOMB DETONATED!");
    return;
  }
  
  // Check photoresistor (light trigger)
  if (analogRead(PHOTO_RESISTOR_PIN) < PHOTO_RESISTOR_TRIGGER) {
    detonateBomb("BOMB DETONATED BY LIGHT EXPOSURE!");
    return;
  }
  
  // Check potentiometer defusal
  int potValue = analogRead(POTENTIOMETER_PIN);
  if (potValue > POT_SAFE_MIN && potValue < POT_SAFE_MAX) {
    if (currentTime - deactivateTimer >= DEACTIVATE_DELAY) {
      deactivateBomb("BOMB DEACTIVATED VIA POTENTIOMETER!");
    }
  } else {
    deactivateTimer = currentTime; // Reset timer if not in safe zone
  }
}

void deactivateBomb(const char* message) {
  currentState = STATE_DEACTIVATED;
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  Serial.println(message);
}

void detonateBomb(const char* message) {
  currentState = STATE_DETONATED;
  Serial.println(message);
}

// ========== INPUT HANDLERS ==========
void readButton() {
  int currentButtonState = digitalRead(BUTTON_PIN);
  
  // Detect rising edge
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    buttonPressed = true;
  }
  
  lastButtonState = currentButtonState;
}

void readIRRemote() {
  if (IrReceiver.decode()) {
    if (currentState == STATE_COUNTING) {
      if (IrReceiver.decodedIRData.command == 22) {
        deactivateBomb("BOMB DEACTIVATED VIA IR REMOTE!");
      } else {
        detonateBomb("WRONG IR COMMAND! BOMB DETONATED!");
      }
    }
    IrReceiver.resume();
  }
}

// ========== DISPLAY FUNCTIONS ==========
void updateDisplay(unsigned long currentTime) {
  if (currentTime - lastDisplayUpdate >= DISPLAY_REFRESH_INTERVAL) {
    lastDisplayUpdate = currentTime;
    displayNumber(countdownValue);
  }
}

void displayNumber(int number) {
  static bool currentDigit = 0;
  
  int units = number % 10;
  int tens = (number / 10) % 10;
  
  // Turn off both digits temporarily
  digitalWrite(DIGIT_1_PIN, HIGH);
  digitalWrite(DIGIT_2_PIN, HIGH);
  
  // Display the appropriate digit
  if (currentDigit == 0) {
    showDigit(units, false);
    digitalWrite(DIGIT_1_PIN, LOW);
  } else {
    showDigit(tens, false);
    digitalWrite(DIGIT_2_PIN, LOW);
  }
  
  currentDigit = !currentDigit;
}

void showDigit(int digit, bool decimalPoint) {
  // Set segments for the digit
  for (int i = 0; i < 7; i++) {
    digitalWrite(SEGMENT_PINS[i], SEGMENT_PATTERNS[digit][i]);
  }
  // Decimal point
  digitalWrite(SEGMENT_PINS[7], decimalPoint ? LOW : HIGH);
}
