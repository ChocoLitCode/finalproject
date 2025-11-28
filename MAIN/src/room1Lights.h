#ifndef ROOM1LIGHTS_H
#define ROOM1LIGHTS_H

#include <Arduino.h>

void room1LightsSetup();
void room1LightsLoop();
void setRoom1Light(bool state);
bool isRoom1LightOn();

#endif