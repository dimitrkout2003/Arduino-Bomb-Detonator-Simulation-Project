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
![Circuit Screenshot](screenshot.png)


## How the Program Works

### Setup
- Configure all pins as INPUT or OUTPUT with `pinMode()`.
- Initialize the IR receiver:
  ```cpp
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);


