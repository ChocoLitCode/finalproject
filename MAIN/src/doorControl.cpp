#include "doorControl.h"
#include "pins.h"

int failAttempts = 0;
unsigned long touchStart = 0;
bool intruderDetected = false;

void doorControlSetup() {
    pinMode(doorLED1, OUTPUT);
    pinMode(doorLED2, OUTPUT);
    pinMode(accessLED, OUTPUT);   
    pinMode(intruderLED, OUTPUT);  
    pinMode(buzzer, OUTPUT);
    digitalWrite(doorLED1, LOW);
    digitalWrite(doorLED2, LOW);
    digitalWrite(accessLED, LOW);
    digitalWrite(intruderLED, LOW);
}

void unlockDoor() {
    Serial.println("Access Granted");
    tone(buzzer, 1000, 150);
    digitalWrite(doorLED1, HIGH);
    digitalWrite(doorLED2, HIGH);
    digitalWrite(accessLED, HIGH);     
    digitalWrite(intruderLED, LOW);
    intruderDetected = false;
}

void intruderAlert() {
    Serial.println("INTRUDER DETECTED");
    for (int i = 0; i < 3; i++) {
        tone(buzzer, 2000, 200);
        delay(250);
    }
    digitalWrite(doorLED1, LOW);
    digitalWrite(doorLED2, LOW);
    digitalWrite(accessLED, LOW);     
    digitalWrite(intruderLED, HIGH);
    failAttempts = 0;
    intruderDetected = true;
}

void doorControlLoop() {
    int t1 = digitalRead(touch1);
    int t2 = digitalRead(touch2);
    bool bothTouched = (t1 == HIGH && t2 == HIGH);

    if (bothTouched) {
        if (touchStart == 0) {
            touchStart = millis();
        }

        if (millis() - touchStart >= 3000) {
            unlockDoor();
            touchStart = 0;
            failAttempts = 0;
        }
    } else {
        if (touchStart != 0) {
            failAttempts++;
            if (failAttempts >= 3) {
                intruderAlert();
            }
            touchStart = 0;
        }
    }
}

bool isIntruderDetected() {
    return intruderDetected;
}

void resetIntruder() {
    intruderDetected = false;
    digitalWrite(intruderLED, LOW);
}