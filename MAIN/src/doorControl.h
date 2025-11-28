#ifndef DOORCONTROL_H
#define DOORCONTROL_H

#include <Arduino.h>

void doorControlSetup();
void doorControlLoop();
bool isIntruderDetected();
void resetIntruder();

#endif