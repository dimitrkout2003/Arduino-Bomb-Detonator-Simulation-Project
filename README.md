# Arduino-Bomb-Detonator-Simulation-Project

## Description
This project implements an **embedded countdown system** using Arduino and a combination of sensors. It simulates a "bomb" that activates when a button is pressed and can be deactivated using multiple methods with sensors and an IR remote.

The system includes:
- 30-second countdown displayed on a **2-digit 7-segment display**.
- LED that lights up when there are 11 seconds or less remaining.
- Buzzer that sounds when the "bomb" is activated or detonated.
- **IR Remote Receiver** for deactivation with the correct command (`22`).
- **Photoresistor (LDR)** to trigger detonation if too much light is detected.
- **Potentiometer** that must be held in the correct range for 5 seconds to deactivate the bomb.

## Components / Sensors
- Arduino Uno
- 2-digit 7-segment display
- LED
- Buzzer
- Push Button
- Photoresistor (LDR)
- Potentiometer
- IR Receiver
- IR Remote
##
![Circuit Screenshot](https://github.com/dimitrkout2003/Arduino-Bomb-Detonator-Simulation-Project/blob/main/BOMB_CIRCUIT.png?raw=true)


## How the Program Works

### Setup
- Configure all pins as INPUT or OUTPUT with `pinMode()`.
- Initialize the IR receiver:
  ```cpp
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
- Start Serial Monitor for debugging:
  ```cpp
  Serial.begin(9600);
  Serial.println("Push the button");
- Set buzzer LOW initially:
  ```cpp
  digitalWrite(buzzerPin, LOW);

### Main Loop
- Button Press Detection
  ```cpp
  if (buttonState == HIGH && lastButtonState == LOW && !countdownStarted)

 - Activates countdown `(countdownStarted = true).`

 - Sets `countdownValue = 30` and initializes timers `(millis()).`

 ### Countdown Handling

 - Use `millis()` for non-blocking timing:
   
   ```cpp
   if (currentMillis - lastUpdate >= 1000) {
    countdownValue--;
    lastUpdate = currentMillis;}
- LED lights when `countdownValue <= 11.`
- Bomb detonates if:
  1. Countdown reaches 0
  2. Photoresistor detects too much light `(resVal < 200)`
  3. Wrong IR remote command
 
### Deactivation Conditions

- Correct IR command `(22):`
```cpp
 if (IrReceiver.decodedIRData.command == 22) {
          Serial.println("BOMB DEACTIVATED!");
```
- Potentiometer held in range (490-550) for 5 seconds.
```cpp
 if (potVal > 490 && potVal < 550) {
        if (!bombDeactivated && millis() - deactivateTime > 5000) {
          Serial.println("BOMB DEACTIVATED!");
```
### Display Countdown on 2-Digit 7-Segment Display

```cpp
display_N(countdownValue);
```
- Splits number into units and tens:
  ```cpp
  int unt = num % 10;
  int ten = (num / 10) % 10;
  ```
  - Uses `segOutput()` to light the correct segments.
 
###  7-Segment Display
- `segValue[10][7]` array defines which segments (a-g) are ON for each digit (0-9).
- `segClear()` turns off all segments before displaying new digit.
- Multiplexing the two digits creates a stable visual output.

### IR Remote
- Reads data from the remote:
```cpp
if (IrReceiver.decode()) { ... }
```
- Command `22` deactivates the bomb.

### Potentiometer
- Acts as a safety key: must remain in correct range for 5 seconds to deactivate the bomb.

### Photoresistor
- Detects surrounding light: if too bright, the bomb detonates.

### Serial Monitor

- Shows real-time debug information:
  1. Countdown
  2. Detonation events
  3. Deactivation events
 
### Installation
1. Open the `.ino `file in Arduino IDE or Tinkercad.
2. Connect components as defined in the code.
3. Upload the sketch to Arduino.
4. Monitor Serial output for debug messages.

### Operation
1. Press the button to start countdown.
2. Deactivate bomb via:
   - Correct IR command `(22)`
   - Potentiometer held steady for 5 seconds
3. Bomb detonates if:
   - Countdown reaches 0
   - Photoresistor detects high light
   - Wrong IR command received
