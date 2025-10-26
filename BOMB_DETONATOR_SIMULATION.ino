#include <IRremote.hpp>

int buttonPin = 2;
int buttonState = 0;
int lastButtonState = 0;
int ledPin = 8;
int buzzerPin = 9;
int resPin = A0;
int resVal = 0;
int RECV_PIN = 10;
int potPin = A1;
int potVal = 0;
bool countdownStarted = false;
bool bombDeactivated = false;
unsigned long deactivateTime = 0;
unsigned long lastUpdate = 0;

byte segValue[10][7] = {
   {0,0,0,0,0,0,1},
   {1,0,0,1,1,1,1},
   {0,0,1,0,0,1,0},
   {0,0,0,0,1,1,0},
   {1,0,0,1,1,0,0},
   {0,1,0,0,1,0,0},
   {0,1,0,0,0,0,0},
   {0,0,0,1,1,1,1},
   {0,0,0,0,0,0,0},
   {0,0,0,0,1,0,0}  
};

byte segPin[8] = {11, 7, 4, 5, 6, 12, 13, 3};
byte digitPin[2] = {A5, A4};

int countdownValue = 30;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(resPin, INPUT);
  pinMode(potPin, INPUT);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  Serial.begin(9600);
  Serial.println("Push the button");

  for (int i = 0; i < 8; i++) {
    pinMode(segPin[i], OUTPUT);
  }
  pinMode(digitPin[0], OUTPUT);
  pinMode(digitPin[1], OUTPUT);

  
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && lastButtonState == LOW && !countdownStarted) {
    
    digitalWrite(buzzerPin, LOW);
    countdownStarted = true;
    bombDeactivated = false;
    deactivateTime = millis();
    lastUpdate = millis();
    countdownValue = 30;
    Serial.println("Countdown started, 30 seconds remaining until the bomb detonates");
  }

  if (countdownStarted) {
    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= 1000) {
      lastUpdate = currentMillis;
      countdownValue--;

     
      Serial.println(countdownValue);

      
      if (countdownValue <= 11) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }

      
      if (countdownValue == 0) {
        digitalWrite(buzzerPin, HIGH);  
        Serial.println("BOMB DETONATED!");
        countdownStarted = false;
      }

      
      resVal = analogRead(resPin);
      if (resVal < 200) {
        Serial.println("BOMB DETONATED BY PHOTORESISTOR!");
        digitalWrite(buzzerPin, HIGH);  
        countdownStarted = false;
      }

      
      if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.command == 22) {
          Serial.println("BOMB DEACTIVATED!");
          bombDeactivated = true;
          digitalWrite(buzzerPin, LOW);  
        } else {
          Serial.println("BOMB DETONATED BY IR REMOTE!");
          digitalWrite(buzzerPin, HIGH);  
        }
        IrReceiver.resume();
        countdownStarted = false;
      }

      
      potVal = analogRead(potPin);
      if (potVal > 490 && potVal < 550) {
        if (!bombDeactivated && millis() - deactivateTime > 5000) {
          Serial.println("BOMB DEACTIVATED!");
          bombDeactivated = true;
          digitalWrite(buzzerPin, LOW);  
          countdownStarted = false;
        }
      } else {
        deactivateTime = millis();
      }
    }

    
    display_N(countdownValue);
  }

  lastButtonState = buttonState;
}


void display_N(int num) {
  int unt = num % 10;
  int ten = (num / 10) % 10;

  segOutput(1,unt, 1);
  segOutput(0,ten, 1);
}


void segClear() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(segPin[i], HIGH);
  }
}


void segOutput(int d, int Number, int dp) {
  segClear();
  digitalWrite(digitPin[d], HIGH);
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPin[i], segValue[Number][i]);
  }
  digitalWrite(segPin[7], dp);
  delayMicroseconds(1000);
  digitalWrite(digitPin[d], LOW);
}
