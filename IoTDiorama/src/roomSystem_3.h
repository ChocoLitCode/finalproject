#ifndef ROOMSYSTEM_3_H
#define ROOMSYSTEM_3_H

#include <DHT.h>

const int DHT22_PIN = 18; // ESP32 pin GPIO18 connected to DHT22 sensor
const int trig = 19; // ESP32 pin GPIO19 connected to Ultrasonic Sensor's TRIG pin
const int echo = 23; // ESP32 pin GPIO23 connected to Ultrasonic Sensor's ECHO pin

extern DHT dht22;

bool setRoomThree();
void startRoomThree();
void getDHT(float* humidity, float* temperature);

#endif