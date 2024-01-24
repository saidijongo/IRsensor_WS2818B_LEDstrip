#include <FastLED.h>
#include <elapsedMillis.h>

#define NUM_LEDS_CUBE 15
#define IR_PIN 13
#define LED_PIN_CUBE 11

#define LED_PIN 12
#define NUM_LEDS 17
#define GROUP_SIZE 3

CRGB leds[NUM_LEDS];
elapsedMillis elapsedTime; // Declare elapsedTime as a static variable
bool continueLoop = true;  // Flag to control loop continuation

CRGB ledscube[NUM_LEDS_CUBE];
bool ledStripState = false; // Initial state is OFF
unsigned long irSensorTimer = 0;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, LED_PIN_CUBE, GRB>(ledscube, NUM_LEDS_CUBE);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  pinMode(IR_PIN, INPUT);
}

void movingRainbowEffect(int speed = 150, bool defaultState = false) {
  static uint8_t startIndex = 0;
  static uint8_t hue = 0;

  // Fill the LED strip with a moving rainbow gradient
  fill_rainbow(ledscube, NUM_LEDS_CUBE, hue, 1);

  // Move the rainbow effect from left to right
  for (int i = 0; i < NUM_LEDS_CUBE; i++) {
    ledscube[i] = ledscube[(i + startIndex) % NUM_LEDS_CUBE];
  }

  FastLED.show();

  // Increment the rainbow hue to change colors
  hue++;

  // Move the rainbow gradient index
  startIndex++;
  if (startIndex >= NUM_LEDS_CUBE) {
    startIndex = 0;
  }

  // Introduce a delay to control the speed
  delay(speed);
}

void toggleLEDState() {
  static bool lastIrState = HIGH;
  static unsigned long sensorInterruptedTime = 0;

  // Read the current state of the IR sensor
  bool currentIrState = digitalRead(IR_PIN);
  Serial.println("IR Sensor State: " + String(currentIrState));

  // Check if the IR state has changed
  if (currentIrState != lastIrState) {
    // Update the timer when the state changes
    sensorInterruptedTime = millis();
    lastIrState = currentIrState;
  }

  // Check if the sensor has been interrupted for more than 10 seconds
  if (currentIrState == LOW && millis() - sensorInterruptedTime >= 1000) {
    // Toggle LED strip state
    ledStripState = !ledStripState;

    // Reset the timer
    sensorInterruptedTime = millis();
  }
}

void ledStripWithBlink(int speed, int runTime) {
  unsigned long startTime = millis();
  unsigned long currentTime;

  const int blinkDuration = 10000;  // 10 seconds
  const int blinkInterval = 500;   // 500 milliseconds (0.5 seconds)

  while (continueLoop && (millis() - startTime < runTime)) {
    // Move the group of 3 blue LEDs from left to right
    for (int i = 0; i <= NUM_LEDS - GROUP_SIZE; ++i) {
      fill_solid(leds + i, GROUP_SIZE, CRGB::Blue); // Set a group of 3 LEDs to blue
      fill_solid(leds, i, CRGB::Red); // Set the background to red
      FastLED.show();
      delay(speed);
      fill_solid(leds + i, GROUP_SIZE, CRGB::Red); // Set the same group back to red
    }

    // Move the group of 3 blue LEDs from right to left
    for (int i = NUM_LEDS - GROUP_SIZE; i >= 0; --i) {
      fill_solid(leds + i, GROUP_SIZE, CRGB::Blue); // Set a group of 3 LEDs to blue
      fill_solid(leds, i, CRGB::Red); // Set the background to red
      FastLED.show();
      delay(speed);
      fill_solid(leds + i, GROUP_SIZE, CRGB::Red); // Set the same group back to red
    }

    // Check if runtime exceeded
    currentTime = millis(); // Update currentTime
    if (currentTime - startTime >= runTime) {
      break;
    }
  }

  // Turn off LEDs after the runTime
  fill_solid(leds, NUM_LEDS, CRGB::Black); // Set all LEDs to black (off)
  FastLED.show();

  delay(3000);

  // Reset elapsedTime for blinking
  elapsedTime = 0;

  startTime = millis(); // Reset the start time for blinking

  while (continueLoop && elapsedTime < blinkDuration) {
    // Turn on all LEDs to green
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(blinkInterval);

    // Turn off all LEDs
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(blinkInterval);
  }

  // Turn off LEDs after the specified duration
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  continueLoop = false;  // Stop further looping
}


void processCommand(String command) {
  // Parse serial command for speed and LED strip state
  int speed = 150; // Default speed
  bool receivedState = false; // Default state is OFF

  if (command.length() > 0) {
    // Split the command into parts
    int commaIndex = command.indexOf(',');
    if (commaIndex != -1) {
      String speedString = command.substring(0, commaIndex);
      speed = speedString.toInt();

      String stateString = command.substring(commaIndex + 1);
      receivedState = stateString.toInt() == 1;
    } else {
      // Only speed is provided
      speed = command.toInt();
    }
  }

  // Set LED strip state based on serial command
  ledStripState = receivedState;

  // Display the moving rainbow pattern with the received speed
  movingRainbowEffect(speed, false);
  //ledStripWithBlink(100,20000);
}


/*
void loop() {
  // Check for serial command
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    processCommand(data);
  }

  // Check and toggle LED state based on IR sensor
  toggleLEDState();

  // Control LED strip pattern
  if (ledStripState) {
    // Display the moving rainbow pattern if the LED strip is ON with a default speed of 150
    movingRainbowEffect(150, true);
  } else {
    ledStripWithBlink(100, 10000);
    // Turn off LEDs if the LED strip is OFF
    fill_solid(ledscube, NUM_LEDS_CUBE, CRGB::Black);
    FastLED.show();
  }
}
*/
void loop() {
  // Check for serial command
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    processCommand(data);
  }

  // Check and toggle LED state based on IR sensor
  toggleLEDState();

  // Control LED strip pattern
  if (ledStripState) {
    Serial.println("LED Strip is ON");
    // Display the moving rainbow pattern if the LED strip is ON with a default speed of 150
    movingRainbowEffect(150, true);
  } else {
    Serial.println("LED Strip is OFF");
    // Turn on LED strip on LED_PIN 12
    ledStripWithBlink(100, 10000);
  }
}
