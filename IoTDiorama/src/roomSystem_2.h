#ifndef ROOMSYSTEM_2_H
#define ROOMSYSTEM_2_H

const int sound = 34;   
const int soundLED = 2;

extern bool ledState;           // Tracks the current state of the LED (false = OFF, true = ON)
extern bool wasAboveThreshold; 
const int soundThreshold = 1;

bool setRoomTwo();
void startRoomTwo();

#endif