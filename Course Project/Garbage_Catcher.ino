// Claude AI helped write this file 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int JOYSTICK_X_PIN = A0;
const int JOYSTICK_Y_PIN = A1;
const int JOYSTICK_SW_PIN = 2;
const int BUTTON_PIN = 3;
const int BUZZER_PIN = 4;

const int JOYSTICK_THRESHOLD = 256;
const int BIN_COUNT = 3;
const int MAX_LIVES = 3;

int score = 0;
int lives = MAX_LIVES;
int currentBin = 0;

void setup() {
  Serial.begin(9600);
  pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  int joystickX = analogRead(JOYSTICK_X_PIN);

  if (joystickX < JOYSTICK_THRESHOLD) {
    currentBin = max(currentBin - 1, 0);
  } else if (joystickX > 1023 - JOYSTICK_THRESHOLD) {
    currentBin = min(currentBin + 1, BIN_COUNT - 1);
  }

  if (digitalRead(JOYSTICK_SW_PIN) == LOW) {
    sortItem();
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    resetGame();
  }

  updateDisplay();
  delay(100);
}

void sortItem() {
  if (currentBin == getCorrectBin()) {
    score++;
    tone(BUZZER_PIN, 1000, 200);
  } else {
    lives--;
    tone(BUZZER_PIN, 500, 200);
    if (lives == 0) {
      gameOver();
    }
  }
}

int getCorrectBin() {
  // Placeholder function, replace with actual machine learning output
  return random(BIN_COUNT);
}

void resetGame() {
  score = 0;
  lives = MAX_LIVES;
}

void gameOver() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Game Over!");
  display.print("Score: ");
  display.println(score);
  display.display();
  delay(3000);
  resetGame();
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Score: ");
  display.println(score);
  display.print("Lives: ");
  display.println(lives);
  display.print("Bin: ");
  switch (currentBin) {
    case 0:
      display.println("Recycling");
      break;
    case 1:
      display.println("Compost");
      break;
    case 2:
      display.println("Landfill");
      break;
  }
  display.display();
}
