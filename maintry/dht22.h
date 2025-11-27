// For libraries and declarations

#include <DHT.h>
const int DHT22_PIN = 18; // ESP32 pin GPIO18 connected to DHT22 sensor

bool initDHT22();
void getDHT22Values(float tempValue,float humiValue);