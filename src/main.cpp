#include <Arduino.h>

#define LASER 7

// Array of laser pulse durations in milliseconds
const int durations[] = {10, 25, 50, 100, 150, 200, 250, 500, 1000};
const int numDurations = sizeof(durations) / sizeof(durations[0]);
const int numCycles = 20; // Number of times to repeat the entire cycle
const unsigned long interval = 10000; // Interval between pulses in milliseconds

bool done = false;
unsigned long previousMillis = 0; // Stores last time pulse was triggered
int currentCycle = 0;
int currentDurationIndex = 0;

void setup() {
  Serial.begin(57600);  // Initialize serial communication
  pinMode(LASER, OUTPUT);   // Set LASER as an output
  digitalWrite(LASER, LOW);  // Turn off the LASER initially
  done = true;  // Set done to true initially
}

void loop() {
  unsigned long currentMillis = millis();

  // ceck serial for start signal:
  if (Serial.available() > 0) {
    char start = Serial.read();
    if (start == 's') {
      done = false;
      currentCycle = 0;
      currentDurationIndex = 0;
      previousMillis = currentMillis;
    }
    else if (start == 'e') {
      done = true;
      digitalWrite(LASER, LOW); 
    }
  }
  if (!done) {
    if (currentMillis - previousMillis >= interval) {
      // Update the previousMillis to the current time
      previousMillis = currentMillis;

      // Trigger the laser pulse
      digitalWrite(LASER, HIGH);
      delay(durations[currentDurationIndex]);
      digitalWrite(LASER, LOW);

      // Move to the next duration
      currentDurationIndex++;
      
      // Check if we have cycled through all durations
      if (currentDurationIndex >= numDurations) {
        currentDurationIndex = 0;
        currentCycle++;
      }

      // Check if we have completed the required number of cycles
      if (currentCycle >= numCycles) {
        done = true;
        // Send completion message to the control script
        Serial.println("e");
        digitalWrite(LASER, LOW);  // Turn off the LASER (optional)
      }
    }
  }
}