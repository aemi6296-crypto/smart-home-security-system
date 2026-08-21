# ESP32 Keypad-Based Home Security System

A microcontroller-based access control system built using an ESP32, a 4x4 matrix keypad, an OLED display, a buzzer, and a servo motor. The system authenticates a user via a PIN entered on the keypad and actuates a servo to unlock a door mechanism upon successful authentication.

---

## Problem It Solves

Traditional mechanical locks (key-based) have well-known limitations: keys can be lost, duplicated, or picked, and there is no way to track or restrict access attempts. This project addresses that by replacing a physical key with a PIN-based digital lock that:

- Requires a correct numeric PIN to unlock, removing the need for a physical key
- Provides real-time visual and audio feedback so the user always knows the system's state
- Detects repeated failed attempts and temporarily locks the system, reducing the risk of brute-force PIN guessing
- Can be extended into a broader smart home / IoT access control setup, since it runs on a WiFi-capable microcontroller (ESP32)

In short, it is a low-cost, DIY prototype of the kind of electronic access control used in smart locks and secured doors.

---

## Features

- 4x4 matrix keypad for PIN entry
- OLED display for real-time status feedback (Enter PIN, Access Granted, Access Denied, Locked Out)
- Buzzer feedback on key press and on success/failure
- Servo motor with smooth (non-abrupt) motion to simulate the door lock mechanism
- Automatic lockout after 3 consecutive failed attempts (15-second cooldown) as a basic anti-brute-force measure
- Input length capping to prevent malformed or overflow input

---

## Hardware Used

| Component | Purpose |
|---|---|
| ESP32 Dev Board | Main microcontroller |
| 4x4 Matrix Keypad | PIN entry |
| SSD1306 OLED Display (128x64, I2C) | Status/feedback screen |
| Servo Motor (SG90 or equivalent) | Door lock actuator |
| Buzzer | Audio feedback |
| Breadboard and jumper wires | Circuit connections |

---

## Circuit and Wiring

![Circuit Diagram](images/circuit.jpg)

![Breadboard Setup](images/breadboard.jpg)

**Pin connections:**

| Module | ESP32 Pin |
|---|---|
| Keypad Rows | 19, 18, 5, 17 |
| Keypad Columns | 16, 4, 25, 33 |
| OLED SDA / SCL | 21 / 22 |
| Buzzer | 13 |
| Servo | 14 |

Note: GPIO2 and GPIO15 were deliberately avoided for the keypad, as these are ESP32 boot strapping pins and can interfere with startup if held low/high externally.

---

## How It Works

1. On startup, the OLED displays `Enter PIN:` and the servo initializes to the locked position (0 degrees).
2. As the user presses digits on the keypad, each entry is registered and displayed on the OLED as a masked character (`*`), so the actual PIN is never shown on screen.
3. Pressing `#` submits the entered PIN for verification:
   - If the PIN matches the stored value, the OLED displays `Access Granted`, a success tone plays, and the servo smoothly rotates to the unlocked position, holds for a few seconds, then returns to the locked position.
   - If the PIN does not match, the OLED displays `Access Denied` and an error tone plays.
4. Pressing `*` clears the current input.
5. After three consecutive failed attempts, the system enters a 15-second lockout state during which all keypad input is ignored, before resetting and allowing new attempts.

---

## Demo Video

[Watch the demo](https://www.youtube.com/shorts/98UHolFIUHg?feature=share)

![Working Demo](images/demo.jpg)

---

## Code

The full source code is available in [`security_system_final.ino`](./security_system_final.ino).

**Required libraries** (install via Arduino Library Manager):
- Keypad
- Adafruit GFX Library
- Adafruit SSD1306
- ESP32Servo

---

## Development Note

This project was developed using vibe coding — an iterative, AI-assisted development workflow — alongside hands-on hardware assembly and testing.

---

## Possible Improvements

- Store the PIN in non-volatile memory (EEPROM/Preferences) instead of hardcoding it in source
- Add WiFi-based logging of access attempts
- Add a camera module for visual verification
- Support an admin-changeable PIN entered via the keypad itself

---

## License

This project is open for personal and educational use. Attribution is appreciated but not required.
