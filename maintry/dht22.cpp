// For definitions/functions 

#include "dht22.h"
DHT dht22(DHT22_PIN, DHT22);// Indicate Type of Component

bool initDHT22(){
    dht22.begin(); 
    return true;
  }

void getDHT22Values(float tempValue,float humiValue){
    // read temperature in Celsius
    tempValue = dht22.readTemperature();
   
     // read humidity
    humiValue  = dht22.readHumidity();
   
  }

