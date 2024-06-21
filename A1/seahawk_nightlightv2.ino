// Code in this file was generated using Claude AI

// Pin Definitions
const int redPin = 4;
const int greenPin = 5;
const int bluePin = 6;
const int buttonPin = 7;
const int photocellPin = A0;
const int graphitePin = A1;

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
  PHOTOCELL,
  MORSE_CODE,
  RAINBOW,
  OFF
};
ColorMode currentMode = PHOTOCELL;

// Variables for rainbow effect
int rainbowHue = 0;
int rainbowStep = 5;

// Variables for Morse code mode
const int dotDelay = 200;
const int dashDelay = dotDelay * 3;
const int letterSpacing = dotDelay;
const int wordSpacing = dotDelay * 7;

const char* morseCode = "... . . - .... . - .-- -.- ...";

void setup() {
  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(graphitePin, INPUT);

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
  brightness = map(sensorValue, 0, 1023, 0, 255);
  float powerFactor = 5.0;
  brightness = (int)(pow((float)brightness / 255.0, powerFactor) * 255);

  // Debug output for monitoring changes and values
  Serial.print("Button state: ");
  Serial.print(buttonPressed);
  Serial.print(", Photocell value: ");
  Serial.print(sensorValue);
  Serial.print(", Computed brightness: ");
  Serial.println(brightness);

  // Check if the graphite strip is touched
  int graphiteValue = analogRead(graphitePin);
  if (graphiteValue < 100) {
    currentMode = OFF;
  }

  // Control RGB LED based on button state
  if (buttonPressed) {
    currentMode = (ColorMode)((currentMode + 1) % 4);
    buttonPressed = false;
  }

  switch (currentMode) {
    case PHOTOCELL:
      // Map the photocell value to the color range (blue to green)
      int colorValue = map(sensorValue, 0, 1023, 0, 255);
      int red = 0;
      int green = colorValue;
      int blue = 255 - colorValue;
      setColor(red, green, blue);
      break;
    case MORSE_CODE:
      blinkMorseCode();
      currentMode = RAINBOW;
      break;
    case RAINBOW:
      int red, green, blue;
      hsvToRgb(rainbowHue, 255, 255, &red, &green, &blue);
      setColor(red, green, blue);
      rainbowHue += rainbowStep;
      if (rainbowHue >= 360) {
        rainbowHue -= 360;
      }
      break;
    case OFF:
      setColor(0, 0, 0);
      break;
  }

  delay(50);
}

void setColor(int red, int green, int blue) {
  int scaledRed = (red * brightness) / 255;
  int scaledGreen = (green * brightness) / 255;
  int scaledBlue = (blue * brightness) / 255;

  analogWrite(redPin, scaledRed);
  analogWrite(greenPin, scaledGreen);
  analogWrite(bluePin, scaledBlue);

  // Debug output to check scaled color values
  Serial.print("Red: ");
  Serial.print(scaledRed);
  Serial.print(", Green: ");
  Serial.print(scaledGreen);
  Serial.print(", Blue: ");
  Serial.println(scaledBlue);
}

void hsvToRgb(int hue, int saturation, int value, int* red, int* green, int* blue) {
  float hh, p, q, t, ff;
  long i;
  float r, g, b;

  if (saturation <= 0.0) {
    *red = value;
    *green = value;
    *blue = value;
    return;
  }
  hh = hue;
  if (hh >= 360.0) hh = 0.0;
  hh /= 60.0;
  i = (long)hh;
  ff = hh - i;
  p = value * (1.0 - ((float)saturation / 255.0));
  q = value * (1.0 - ((float)saturation / 255.0) * ff);
  t = value * (1.0 - ((float)saturation / 255.0) * (1.0 - ff));

  switch (i) {
    case 0:
      r = value;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = value;
      b = p;
      break;
    case 2:
      r = p;
      g = value;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = value;
      break;
    case 4:
      r = t;
      g = p;
      b = value;
      break;
    case 5:
    default:
      r = value;
      g = p;
      b = q;
      break;
  }
  *red = (int)(r * 255.0);
  *green = (int)(g * 255.0);
  *blue = (int)(b * 255.0);
}

void blinkMorseCode() {
  for (int i = 0; morseCode[i] != '\0'; i++) {
    if (morseCode[i] == '.') {
      setColor(0, 0, 255);
      delay(dotDelay);
    } else if (morseCode[i] == '-') {
      setColor(0, 255, 0);
      delay(dashDelay);
    } else if (morseCode[i] == ' ') {
      setColor(0, 0, 0);
      delay(wordSpacing);
    }
    setColor(0, 0, 0);
    delay(letterSpacing);
  }
  delay(wordSpacing);
}
