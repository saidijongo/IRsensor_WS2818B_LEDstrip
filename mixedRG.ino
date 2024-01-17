#include <FastLED.h>

#define LED_PIN 11
#define IR_PIN 13
#define NUM_LEDS 50

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(900);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(IR_PIN, INPUT);
}

void blinkLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i % 2 == 0) {
      leds[i] = CRGB::Green;
    } else {
      leds[i] = CRGB::Red;
    }
  }
  FastLED.show();
  delay(500);

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(500);
}

voi
d loop() {  
  // Check if IR sensor is interrupted
  if (digitalRead(IR_PIN) == LOW) {
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) { 
      blinkLEDs();
      delay(50);
    }

    fill_solid(leds, NUM_LEDS, CRGB::Black);  // Turn off LEDs
    FastLED.show();
  }
  delay(100);
}
