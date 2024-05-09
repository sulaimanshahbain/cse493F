#include <Keyboard.h>

const int buttonAPin = 4;      // Button A
const int buttonBPin = 3;      // Button B
const int buttonCPin = 2;      // Button C
const int buttonDPin = 1;      // Button D

bool lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH}; // Initial state HIGH because of INPUT_PULLUP
bool currentButtonState[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
unsigned long debounceDelay = 50; // Debounce time in milliseconds

void setup() {
  pinMode(buttonAPin, INPUT_PULLUP);
  pinMode(buttonBPin, INPUT_PULLUP);
  pinMode(buttonCPin, INPUT_PULLUP);
  pinMode(buttonDPin, INPUT_PULLUP);

  Keyboard.begin();
}

void loop() {
  handleButton(buttonAPin, 0, 'A');
  handleButton(buttonBPin, 1, 'B');
  handleButton(buttonCPin, 2, 'C');
  handleButton(buttonDPin, 3, 'D');
}

void handleButton(int pin, int index, char key) {
  int reading = digitalRead(pin);
  if (reading != lastButtonState[index]) {
    lastDebounceTime[index] = millis();
  }
  if ((millis() - lastDebounceTime[index]) > debounceDelay) {
    if (reading != currentButtonState[index]) {
      currentButtonState[index] = reading;
      if (currentButtonState[index] == LOW) {
        Keyboard.write(key);
      }
    }
  }
  lastButtonState[index] = reading;
}
