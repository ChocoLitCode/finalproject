#ifndef ROOMSYSTEM_2_H
#define ROOMSYSTEM_2_H

const int sound = 35;   
const int soundLED = 2;

extern bool room2_override;   
extern bool room2_state;            
extern bool room2_manualTarget; // manual ON/OFF command
const int soundThreshold = 1;

// Accept a function pointer for notifying clients
void startRoomTwo(void (*notify)(float, float));
void setRoomTwo();

#endif
