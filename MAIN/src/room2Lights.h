#ifndef ROOM2LIGHTS_H
#define ROOM2LIGHTS_H

#include <Arduino.h>

void room2LightsSetup();
void room2LightsLoop();
void setRoom2Light(bool state);
bool isRoom2LightOn();

#endif