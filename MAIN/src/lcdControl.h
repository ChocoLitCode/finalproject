#ifndef LCDCONTROL_H
#define LCDCONTROL_H

#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

void lcdControlSetup();
void lcdControlLoop();
float getTemperature();
float getHumidity();

#endif