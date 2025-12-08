#ifndef ROOMSYSTEM_3_H
#define ROOMSYSTEM_3_H

#include <DHT.h>

// Pins
const int DHT22_PIN = 18;
const int trig = 19;
const int echo = 23;

extern DHT dht22;

// Initialize module
bool setRoomThree();

// Read DHT sensor
void getDHT(float* humidity, float* temperature);

// Non-blocking update function
void startRoomThree(float* temperature, float* humidity, float* distance);

#endif
