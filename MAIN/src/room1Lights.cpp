#include "room1Lights.h"
#include "pins.h"

bool room1LightState = false;

void room1LightsSetup() {
    pinMode(ldr, INPUT);
    pinMode(ldrLED1, OUTPUT);
    pinMode(ldrLED2, OUTPUT);
    pinMode(ldrLED3, OUTPUT);
    digitalWrite(ldrLED1, LOW);
    digitalWrite(ldrLED2, LOW);
    digitalWrite(ldrLED3, LOW);
}

void room1LightsLoop() {
    int LDRvalue = analogRead(ldr);
    
    if (LDRvalue > 2500) {
        setRoom1Light(true);
    } else {
        setRoom1Light(false);
    }
}

void setRoom1Light(bool state) {
    room1LightState = state;
    digitalWrite(ldrLED1, state);
    digitalWrite(ldrLED2, state);
    digitalWrite(ldrLED3, state);
}

bool isRoom1LightOn() {
    return room1LightState;
}