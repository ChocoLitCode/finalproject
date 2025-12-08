#ifndef ROOMSYSTEM_1_H
#define ROOMSYSTEM_1_H

const int ldr = 34;
const int ldrLED1 = 4;
const int ldrLED2 = 16;
const int ldrLED3 = 17;

const int ldrThreshold = 4000;

extern bool room1_override;   // true if web overrides
extern bool room1_state;   
extern bool room1_manualTarget;   // desired state: true=ON, false=OFF


void setRoomOne();
void startRoomOne(void (*notify)(float,float));

#endif
