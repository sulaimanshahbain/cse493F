// IMPORTANT: ALL CODE IN THIS FILE WAS GENERATED WITH GPT-4, MULTIPLE PROMPTS AND DEBUGGING

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin not used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonPin = 4;  // Button pin
const int ledPin = 9;     // LED pin
const int buzzerPin = 10; // Buzzer pin
bool buttonPressed = false;

struct Block {
  int x;      // X position of the left side of the block
  int width;  // Width of the block
  int y;      // Y position from bottom of the display
  int direction; // Direction of movement: 1 for right, -1 for left
};

Block blocks[100]; // Array to store blocks
int blockCount = 0; // Current number of placed blocks
bool isGameOver = false;
bool gameStarted = false;
int score = 0;
int gameSpeed = 25; // Initial game speed

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);  // Set the LED pin as an output
  pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as an output
  digitalWrite(ledPin, LOW); // Ensure LED is off initially
  digitalWrite(buzzerPin, LOW); // Ensure Buzzer is off initially
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
}

void loop() {
  if (!gameStarted) {
    showStartScreen();
    if (digitalRead(buttonPin) == LOW && !buttonPressed) {
      buttonPressed = true;
      gameStarted = true;
      initGame();
    } else if (digitalRead(buttonPin) == HIGH) {
      buttonPressed = false;
    }
  } else if (!isGameOver) {
    display.clearDisplay();
    if (digitalRead(buttonPin) == LOW && !buttonPressed) {
      buttonPressed = true;
      placeBlock();
      digitalWrite(ledPin, HIGH); // Turn on the LED when a block is placed
      digitalWrite(buzzerPin, HIGH); // Turn on the Buzzer when a block is placed
    } else if (digitalRead(buttonPin) == HIGH) {
      buttonPressed = false;
      digitalWrite(ledPin, LOW); // Turn off the LED when no block is placed
      digitalWrite(buzzerPin, LOW); // Turn off the Buzzer when no block is placed
    }

    for (int i = 0; i < blockCount; i++) {
      drawBlock(blocks[i].x, blocks[i].y, blocks[i].width, 2);
    }
    if (blockCount > 0) {
      moveCurrentBlock();
      drawBlock(blocks[blockCount].x, blocks[blockCount].y, blocks[blockCount].width, 2);
    }

    // Display score
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Score: ");
    display.println(score);

    display.display();
    delay(gameSpeed);
  } else {
    showGameOverScreen();
    delay(3000); // Delay for 3 seconds
    gameStarted = false;
  }
}

void initGame() {
  blockCount = 1;
  blocks[0] = { (SCREEN_WIDTH - 30) / 2, 30, SCREEN_HEIGHT - 5 };  // Initial large block at very bottom
  blocks[1] = { 0, 30, SCREEN_HEIGHT - 15, 1 };  // First moving block
  isGameOver = false;
  score = 0;
  gameSpeed = 25; // Reset game speed
}

void moveCurrentBlock() {
  Block& currentBlock = blocks[blockCount];
  currentBlock.x += currentBlock.direction * (2 + (blockCount / 5)); // Increase speed with block count
  if (currentBlock.x + currentBlock.width >= SCREEN_WIDTH || currentBlock.x <= 0) {
    currentBlock.direction *= -1; // Reverse direction
  }
}

void placeBlock() {
  Block& lastBlock = blocks[blockCount - 1];
  Block& currentBlock = blocks[blockCount];
  if ((currentBlock.x > lastBlock.x + lastBlock.width) || (currentBlock.x + currentBlock.width < lastBlock.x)) {
    isGameOver = true;
  } else {
    int overlapLeft = max(currentBlock.x, lastBlock.x);
    int overlapRight = min(currentBlock.x + currentBlock.width, lastBlock.x + lastBlock.width);
    currentBlock.width = overlapRight - overlapLeft;
    currentBlock.x = overlapLeft;

    // Move all blocks up by a fixed amount
    for (int i = 0; i <= blockCount; i++) {
      blocks[i].y -= 5;
    }

    // Add a new block
    if (currentBlock.width > 0) {
      blocks[++blockCount] = { 0, currentBlock.width, SCREEN_HEIGHT - 5, 1 };
      score++;
      
      // Increase game speed gradually
      if (gameSpeed > 10) {
        gameSpeed -= 1;
      }

      // Turn on the LED and Buzzer when a block is placed
      digitalWrite(ledPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(100); // Keep the LED and Buzzer on for 100 milliseconds to make it noticeable
      digitalWrite(ledPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }
  }
}

void drawBlock(int x, int y, int width, int height) {
  display.fillRect(x, y - height, width, height, SSD1306_WHITE);
}

void showStartScreen() {
  display.clearDisplay();
  
  // Display "Stack Game" in large and bold font
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("FastBelow");
  
  // Display "Press button to start" in smaller font below
  display.setTextSize(1);
  display.setCursor(10, 40);
  display.println("Press button");
  display.setCursor(10, 50);
  display.println("to start game...");
  
  display.display();
}

void showGameOverScreen() {
  display.clearDisplay();
  
  // Display "Game Over!" in large and bold font
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("Game Over");

  // Display score
  display.setTextSize(1);
  display.setCursor(10, 30);
  display.print("Score: ");
  display.println(score);

  // Display "New game loading..." in smaller font below
  display.setCursor(10, 50);
  display.println("New game loading...");

  display.display();
  
  while (digitalRead(buttonPin) != LOW); // Wait for button press
  delay(500); // Debounce delay after press
}
