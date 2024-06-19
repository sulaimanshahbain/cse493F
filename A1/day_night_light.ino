// Pin Definitions
const int redPin = 4;
const int greenPin = 5;
const int bluePin = 6;
const int buttonPin = 7;
const int photocellPin = A0;

// Variable to store the button state
bool buttonPressed = false;
int buttonState;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// For RGB LED control
int brightness;

// Variables for color modes
enum ColorMode {
  BLUE,
  GREEN,
  RED,
  RAINBOW
};
ColorMode currentMode = BLUE;

// Variables for rainbow effect
int rainbowHue = 0;
int rainbowStep = 5;

void setup() {
  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize RGB LED to off
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Debounce the button press
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        buttonPressed = true;
      }
    }
  }
  lastButtonState = reading;

  // Read the photocell value
  int sensorValue = analogRead(photocellPin);

  // Dramatically adjust brightness based on light level
  brightness = map(sensorValue, 0, 1023, 0, 255); // Map the photocell value to the full PWM range
  
  // Apply a power function for more dramatic brightness variation
  float powerFactor = 3.0; // Adjust this value to control the dramaticness (higher values make it more dramatic)
  brightness = (int)(pow((float)brightness / 255.0, powerFactor) * 255);

  // Debug output for monitoring changes and values
  Serial.print("Button state: ");
  Serial.print(buttonPressed);
  Serial.print(", Photocell value: ");
  Serial.print(sensorValue);
  Serial.print(", Computed brightness: ");
  Serial.println(brightness);

  // Control RGB LED based on button state
  if (buttonPressed) {
    // Change color mode
    currentMode = (ColorMode)((currentMode + 1) % 4);
    buttonPressed = false; // Reset the button state
  }

  switch (currentMode) {
    case BLUE:
      setColor(0, 0, 255, brightness);
      break;
    case GREEN:
      setColor(0, 255, 0, brightness);
      break;
    case RED:
      setColor(255, 0, 0, brightness);
      break;
    case RAINBOW:
      // Calculate RGB values based on the current hue
      int red, green, blue;
      hsvToRgb(rainbowHue, 255, brightness, &red, &green, &blue);
      setColor(red, green, blue, 255); // Set color with full brightness

      // Update the hue for the next iteration
      rainbowHue = (rainbowHue + rainbowStep) % 360;
      break;
  }

  delay(50); // Adjust delay for desired responsiveness
}

void setColor(int red, int green, int blue, int brightness) {
  int scaledRed = (red * brightness) / 255;
  int scaledGreen = (green * brightness) / 255;
  int scaledBlue = (blue * brightness) / 255;

  analogWrite(redPin, scaledRed);
  analogWrite(greenPin, scaledGreen);
  analogWrite(bluePin, scaledBlue);

  // Additional debug output to check scaled color values
  Serial.print("Red: ");
  Serial.print(scaledRed);
  Serial.print(", Green: ");
  Serial.print(scaledGreen);
  Serial.print(", Blue: ");
  Serial.println(scaledBlue);
}

// Function to convert HSV color to RGB color
void hsvToRgb(int hue, int saturation, int value, int* red, int* green, int* blue) {
  int hi = (hue / 60) % 6;
  int f = hue % 60;
  int p = value * (255 - saturation) / 255;
  int q = value * (255 - (saturation * f) / 60) / 255;
  int t = value * (255 - (saturation * (60 - f)) / 60) / 255;

  switch (hi) {
    case 0:
      *red = value;
      *green = t;
      *blue = p;
      break;
    case 1:
      *red = q;
      *green = value;
      *blue = p;
      break;
    case 2:
      *red = p;
      *green = value;
      *blue = t;
      break;
    case 3:
      *red = p;
      *green = q;
      *blue = value;
      break;
    case 4:
      *red = t;
      *green = p;
      *blue = value;
      break;
    case 5:
      *red = value;
      *green = p;
      *blue = q;
      break;
  }
}
