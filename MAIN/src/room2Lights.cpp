#include "room2Lights.h"
#include "pins.h"

bool room2LightState = false;
bool wasAboveThreshold = false;

void room2LightsSetup() {
    pinMode(sound, INPUT);
    pinMode(soundLED, OUTPUT);
    digitalWrite(soundLED, LOW);
}

void room2LightsLoop() {
    int soundValue = analogRead(sound);

    if (soundValue > soundThreshold) {
        if (!wasAboveThreshold) {
            room2LightState = !room2LightState;
            digitalWrite(soundLED, room2LightState);
            wasAboveThreshold = true;
        }
    } else {
        wasAboveThreshold = false;
    }
}

void setRoom2Light(bool state) {
    room2LightState = state;
    digitalWrite(soundLED, state);
}

bool isRoom2LightOn() {
    return room2LightState;
}