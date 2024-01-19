#include <FastLED.h>

#define PULL_PIN 46    // PULL
#define DIR_PIN 45     // Direction
#define IR_PIN 13      // IR sensor
#define LED_PIN 11     // LED strip
#define NUM_LEDS 32    // Number of LEDs

CRGB leds[NUM_LEDS]; 

const int stepsPerRev = 6400;


void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PULL_PIN, OUTPUT);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  // Initialize FastLED
}



void blinkLEDs() {
  const int blinkDuration = 10000;  // 10 seconds
  const int blinkInterval = 500;   // 500 milliseconds (0.5 seconds)
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
  int stepsToMove = 6400; //stepsPerRev * angle / 360;
  int direction = (angle >= 0) ? HIGH : LOW;
  angle = abs(angle);

  digitalWrite(DIR_PIN, direction);

  for (int i = 0; i < stepsToMove; ++i) {
    digitalWrite(PULL_PIN, HIGH);  // Pulse the motor step
    delayMicroseconds(500);  
    digitalWrite(PULL_PIN, LOW);
    delayMicroseconds(500);

    // Check if the IR sensor is interrupted
    if (digitalRead(IR_PIN) == LOW) {
      // IR sensor interrupted, stop the stepper motor
      digitalWrite(PULL_PIN, LOW);

      // Blink the LED strip for 20 seconds
      blinkLEDs();

      // Turn off the LED strip
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      return;  // Exit the function
    }
  }

  // Turn off the stepper motor after completing the rotation
  digitalWrite(PULL_PIN, LOW);
  //delay(1000);
}


void loop() {
  runStepper(10, 1000); 
  delay(1000);
}
