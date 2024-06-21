// Code in this file was generated using Claude AI

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
  RAINBOW,
  MORSE_CODE,
  BLUE,
  GREEN,
  OFF
};
ColorMode currentMode = RAINBOW;

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

  // Initialize RGB LED to off
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
}

void loop() {
  int reading = digitalRead(buttonPin);
  int sensorValue = analogRead(photocellPin);
  brightness = map(sensorValue, 0, 1023, 255, 0); // Inverse brightness adjustment

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

  if (buttonPressed) {
    currentMode = (ColorMode)((currentMode + 1) % 5);
    buttonPressed = false;
  }

  switch (currentMode) {
    case RAINBOW:
      int red, green, blue;
      hsvToRgb(rainbowHue, 255, brightness, &red, &green, &blue);
      setColor(red, green, blue);
      rainbowHue += rainbowStep;
      if (rainbowHue >= 360) {
        rainbowHue -= 360;
      }
      break;
    case MORSE_CODE:
      blinkMorseCode();
      break;
    case BLUE:
      setColor(0, 0, 255);
      break;
    case GREEN:
      // Crossfade between green and blue based on photocell reading
      int greenValue = map(sensorValue, 0, 1023, 0, 255);
      int blueValue = 255 - greenValue; // Reverse proportion for blue
      setColor(0, greenValue, blueValue);
      break;
    case OFF:
      setColor(0, 0, 0);
      break;
  }

  delay(50);
}

void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);

  // Debug output to check scaled color values
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print(", Green: ");
  Serial.print(green);
  Serial.print(", Blue: ");
  Serial.println(blue);
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
