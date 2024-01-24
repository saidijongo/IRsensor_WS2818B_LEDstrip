#include <FastLED.h>

#define NUM_LEDS_CUBE 50
#define IR_PIN 13
#define LED_PIN_CUBE 11

CRGB ledscube[NUM_LEDS_CUBE];
bool ledStripState = false; // Initial state is OFF
unsigned long irSensorTimer = 0;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, LED_PIN_CUBE, GRB>(ledscube, NUM_LEDS_CUBE);
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

  // Check if the IR state has changed
  if (currentIrState != lastIrState) {
    // Update the timer when the state changes
    sensorInterruptedTime = millis();
    lastIrState = currentIrState;
  }

  // Check if the sensor has been interrupted for more than 10 seconds
  if (currentIrState == LOW && millis() - sensorInterruptedTime >= 10000) {
    // Toggle LED strip state
    ledStripState = !ledStripState;

    // Reset the timer
    sensorInterruptedTime = millis();
  }
}

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
    // Turn off LEDs if the LED strip is OFF
    fill_solid(ledscube, NUM_LEDS_CUBE, CRGB::Black);
    FastLED.show();
  }
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
}
