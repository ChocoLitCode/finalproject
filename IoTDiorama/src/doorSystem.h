#ifndef DOORSYSTEM_H
#define DOORSYSTEM_H

#include <Arduino.h>

// Pin Declarations
const int touch1 = 33;
const int touch2 = 32;
const int doorLED1 = 14;
const int doorLED2 = 27;
const int accessLED = 26;    
const int intruderLED = 25;
const int buzzer = 13;

extern int failAttempts;
extern unsigned long touchStart;

bool setDoorPins();
void startDoor();


#endif