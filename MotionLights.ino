// Import Libraries
#include <Adafruit_NeoPixel.h>

// Configure Wiring Connections (PINS)
#define PIR    5
#define LED    8
#define PHOTO A5

// Number of LEDs
#define NUM_LEDS  21

int ambientThreshold = 400;
int calibrationTime  =   5; // Seconds
int pause            =  10; // Seconds

// End User Configurable settings

boolean lockLow     = true;
unsigned long int lowIn;
boolean takeLowTime;

enum FadeDirection {
  IN,
  OUT
};

Adafruit_NeoPixel  strip =  Adafruit_NeoPixel(NUM_LEDS, LED, NEO_GRB + NEO_KHZ800);

void setup() {

  strip.begin();
  strip.show();

  Serial.begin(9600);

  pinMode(PIR, INPUT);

  Serial.print("Calibrating Sensors ");
  for (int i = 0; i < calibrationTime; i++) {
    digitalRead(PIR);
    analogRead(PHOTO);
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" - Completed");
  Serial.println("Sensors Active");
}

void loop() {
  int light = analogRead(PHOTO);

  if (digitalRead(PIR) == HIGH) {
    if (lockLow) {
      lockLow = false;
      if (light < ambientThreshold) {
        fade(0xff, 0xff, 0xff, IN);
      }
    }
    takeLowTime = true;
  }

  if (digitalRead(PIR) == LOW) {
    if (takeLowTime) {
      lowIn = millis();
      takeLowTime = false;
    }

    if (!lockLow && millis() - lowIn > (pause * 1000)) {
      lockLow = true;
      fade(0xff, 0xff, 0xff, OUT);
    }
  }
}

void fade(byte red, byte green, byte blue, FadeDirection fadeDirection) {
  float r, g, b;

  if (fadeDirection == IN) {
    for (int k = 0; k < 256; k++) {
      r = (k / 256.0) * red;
      g = (k / 256.0) * green;
      b = (k / 256.0) * blue;
      for (int i = 0; i < NUM_LEDS; i++ ) {
        strip.setPixelColor(i, strip.Color(r, g, b));
      }
      strip.show();
      delay(5);
    }
  }

  if (fadeDirection == OUT) {
    for (int k = 255; k >= 0; k--) {
      r = (k / 256.0) * red;
      g = (k / 256.0) * green;
      b = (k / 256.0) * blue;
      for (int i = 0; i < NUM_LEDS; i++ ) {
        strip.setPixelColor(i, strip.Color(r, g, b));
      }
      strip.show();
      delay(5);
    }
  }
}
