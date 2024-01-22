#include <FastLED.h>

#define PULL_PIN 46    // PULL
#define DIR_PIN 45     // Direction
#define IR_PIN 13      // IR sensor
#define LED_PIN 11
#define NUM_LEDS 32

CRGB leds[NUM_LEDS];

const int stepsPerRev = 5000; // 6400;
const float STEP_ANGLE = 1.8; // degrees per step

void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PULL_PIN, OUTPUT);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  // Initialize FastLED
}

void blinkLEDs() {
  const int blinkDuration = 5000; // 5 seconds blink
  const int blinkInterval = 500;  // 500 milliseconds (0.5 seconds)
  unsigned long startTime = millis();
  while (millis() - startTime < blinkDuration) {
    // Turn on all LEDs
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(blinkInterval);

    // Turn off all LEDs
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(blinkInterval);
  }
}

void runStepper(int angle, int speed) {
  int step_target_position = static_cast<int>(angle / STEP_ANGLE);
  int direction = (angle >= 0) ? HIGH : LOW;
  angle = abs(angle);

  digitalWrite(DIR_PIN, direction);

  int i = 0;
  while (i < step_target_position && digitalRead(IR_PIN) == HIGH) {
    digitalWrite(PULL_PIN, HIGH);  // Pulse the motor step
    delayMicroseconds(speed);
    digitalWrite(PULL_PIN, LOW);
    delayMicroseconds(speed);

    i++; // Increment step count
  }

  // Turn off the stepper motor after completing the rotation
  digitalWrite(PULL_PIN, LOW);

  // Check if the IR sensor is interrupted or the desired angle is reached
  if (digitalRead(IR_PIN) == LOW || i < step_target_position) {
    // Blink the LED strip for 5 seconds
    blinkLEDs();

    // Turn off the LED strip
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}

void loop() {
  runStepper(100, 1000);
}
