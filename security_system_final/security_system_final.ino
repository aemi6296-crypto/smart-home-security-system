#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// ---------- OLED CONFIG ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- KEYPAD CONFIG ----------
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// FIX: avoided GPIO2 and GPIO15 (boot strapping pins) — moved to safe pins
byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 25, 33};   // FIX: 2 -> 25, 15 -> 33
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------- BUZZER CONFIG ----------
#define BUZZER_PIN 13

// ---------- SERVO CONFIG ----------
#define SERVO_PIN 14   // FIX: kept away from keypad pins (26 was reused/confused across your two files)
Servo doorServo;

// ---------- PASSWORD CONFIG ----------
String password = "994";
String inputPassword = "";
const unsigned int MAX_LEN = 6;      // FIX: cap input length

// FIX: brute-force lockout
byte failedAttempts = 0;
const byte MAX_FAILS = 3;
const unsigned long LOCKOUT_MS = 15000;

// ---------- FUNCTIONS ----------
void beep(int duration) {
  tone(BUZZER_PIN, 1000, duration);
  delay(duration / 2);
  noTone(BUZZER_PIN);
}

void successTone() {
  tone(BUZZER_PIN, 1500, 100);
  delay(150);
  tone(BUZZER_PIN, 2000, 100);
  delay(150);
  noTone(BUZZER_PIN);
}

void errorTone() {
  tone(BUZZER_PIN, 400, 250);
  delay(100);
  tone(BUZZER_PIN, 400, 250);
  delay(100);
  noTone(BUZZER_PIN);
}

void smoothMove(int startPos, int endPos, int delayTime) {
  if (startPos < endPos) {
    for (int pos = startPos; pos <= endPos; pos++) {
      doorServo.write(pos);
      delay(delayTime);
    }
  } else {
    for (int pos = startPos; pos >= endPos; pos--) {
      doorServo.write(pos);
      delay(delayTime);
    }
  }
}

void showMessage(const char* line1, const char* line2 = nullptr) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println(line1);
  if (line2) {
    display.setCursor(0, 45);
    display.println(line2);
  }
  display.display();
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);

  doorServo.attach(SERVO_PIN, 500, 2400);
  doorServo.write(0); // locked

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  showMessage("Enter PIN:");
  Serial.println("System Ready. Waiting for input...");
}

// ---------- MAIN LOOP ----------
void loop() {
  static unsigned long lockoutUntil = 0;
  if (lockoutUntil != 0) {
    if (millis() < lockoutUntil) {
      return;
    } else {
      lockoutUntil = 0;
      failedAttempts = 0;
      showMessage("Enter PIN:");
    }
  }

  char key = keypad.getKey();

  if (key) {
    beep(80);
    Serial.print("Key pressed: ");
    Serial.println(key);

    // FIX: '#' is now ACTUALLY the confirm key (not just in a comment)
    if (key == '#') {
      if (inputPassword == password) {
        showMessage("Access", "Granted");
        Serial.println("Access Granted!");
        successTone();
        failedAttempts = 0;

        smoothMove(0, 90, 10);
        delay(3000);
        smoothMove(90, 0, 10);
        Serial.println("Door closed.");

      } else {
        showMessage("Access", "Denied");
        Serial.println("Access Denied!");
        errorTone();

        failedAttempts++;
        if (failedAttempts >= MAX_FAILS) {
          lockoutUntil = millis() + LOCKOUT_MS;
          showMessage("Locked out", "Wait 15s");
          inputPassword = "";
          delay(1500);
          return;
        }
      }

      delay(2000);
      inputPassword = "";
      showMessage("Enter PIN:");

    } else if (key == '*') {
      inputPassword = "";
      showMessage("Cleared");
      Serial.println("Input cleared");
      delay(800);
      showMessage("Enter PIN:");

    } else {
      if (inputPassword.length() < MAX_LEN) {
        inputPassword += key;
      }
      Serial.print("Current input: ");
      Serial.println(inputPassword);

      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(10, 20);
      for (unsigned int i = 0; i < inputPassword.length(); i++) {
        display.print('*');
      }
      display.display();
    }
  }
}

