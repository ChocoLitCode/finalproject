#include "roomSystem_2.h"   
#include "lcd.h" 

 bool ledState = false;           // Tracks the current state of the LED (false = OFF, true = ON)
 bool wasAboveThreshold = false; 

bool setRoomTwo(){
  pinMode(sound,INPUT);
  pinMode(soundLED, OUTPUT);
  digitalWrite(soundLED,LOW);
  return true;
}

void startRoomTwo(){

  int soundValue = analogRead(sound);   
  Serial.println("Sound Value: ");
  Serial.print(soundValue);

// Check if the sound intensity is above the threshold
  if (soundValue > soundThreshold) {
    // If the sound is above the threshold and it was NOT above the threshold
    // in the previous loop cycle, this is a "rising edge" event.
    if (!wasAboveThreshold) {
      // Invert the LED's state
      ledState = !ledState;

      // Update the LED to the new state
      digitalWrite(soundLED, ledState);

      // Print a message to the Serial Monitor
      if (ledState) {
        Serial.println("LED is now ON.");
        lcd.setCursor(16, 1);
        lcd.print("ON ");
      } else {
        Serial.println("LED is now OFF.");
        lcd.setCursor(16, 1);
        lcd.print("OFF");
      }

      // Set the flag to true to prevent the toggle from happening
      // repeatedly while the sound is still above the threshold.
      wasAboveThreshold = true;
    }
  } else {
    // If the sound is below the threshold, reset the flag.
    // This allows the toggle to happen again the next time the threshold is met.
    wasAboveThreshold = false;
  }
}
