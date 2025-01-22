#include <Arduino.h>

#define LASER 7
#define LED 13

// Array of laser pulse durations in milliseconds
const int durations[] = {50, 100, 250, 500, 1000, 2000};
const int numDurations = sizeof(durations) / sizeof(durations[0]);
const int numCycles = 20; // Number of times to repeat the entire cycle
const unsigned long interval = 5000; // Interval between pulses in milliseconds

bool done = false;
unsigned long previousMillis = 0; // Stores last time pulse was triggered
int currentCycle = 0;
int currentDurationIndex = 0;
bool pulseActive = false; // New flag to track if we're in the middle of a pulse

void setup() {
  Serial.begin(57600);  // Initialize serial communication
  pinMode(LASER, OUTPUT);   // Set LASER as an output
  pinMode(LED, OUTPUT);   // Set LED as an output
  digitalWrite(LASER, LOW);  // Turn off the LASER initially
  digitalWrite(LED, LOW);  // Turn on the LED to indicate the device is ready
  done = true;  // Set done to true initially
}

void loop() {
  unsigned long currentMillis = millis();

  // check serial for start signal:
  if (Serial.available() > 0) {
    char start = Serial.read();
    if (start == 's') {
      done = false;
      currentCycle = 0;
      currentDurationIndex = 0;
      previousMillis = currentMillis;
      pulseActive = false;
    }
    else if (start == 'e') {
      done = true;
      digitalWrite(LASER, LOW); 
      digitalWrite(LED, LOW);
    }
  }

  if (!done) {
    if (!pulseActive) {
      // Check if it's time for the next pulse
      if (currentMillis - previousMillis >= interval) {
        // Start the pulse
        digitalWrite(LASER, HIGH);
        digitalWrite(LED, HIGH);
        pulseActive = true;
        previousMillis = currentMillis;
      }
    } else {
      // Check if the current pulse duration is complete
      if (currentMillis - previousMillis >= durations[currentDurationIndex]) {
        // End the pulse
        digitalWrite(LASER, LOW);
        digitalWrite(LED, LOW);
        pulseActive = false;
        
        // Move to the next duration
        currentDurationIndex++;
        
        // Check if we have cycled through all durations
        if (currentDurationIndex >= numDurations) {
          currentDurationIndex = 0;
          currentCycle++;
          
          // Check if we have completed the required number of cycles
          if (currentCycle >= numCycles) {
            done = true;
            // Send completion message to the control script
            Serial.println("e");
          }
        }
        
        // Reset previousMillis for the interval between pulses
        previousMillis = currentMillis;
      }
    }
  }
}