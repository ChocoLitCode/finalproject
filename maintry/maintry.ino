// Global

#include "dht22.h"
#include "wifi.h"
#include "web.h"

long lastSendTime = 0;
long interval = 2000;



// Create AsyncWebServer object on port 80


String getTemperature() {
  float temperature = bme.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  Serial.println(temperature);
  return String(temperature);
}
  
String getHumidity() {
  float humidity = bme.readHumidity();
  Serial.println(humidity);
  return String(humidity);
}


// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  
   if (var == "TEMPERATURE"){
    return getTemperature();
  }
  else if (var == "HUMIDITY"){
    return getHumidity();
  }
  return String();
}
 


void setup(){
  // Serial port for debugging purposes
 Serial.begin(115200);
  Serial.println("Started");

  if(!initDHT22()){
    Serial.println("Failed to initialize DHT22 sensor!");
  }

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  if(!initWifi()){
    Serial.println("Failed to connect to WiFi!");
  }



}
 


void loop() {
  if(millis() - lastSendTime >= interval){
  float temperature;
  float humidity;
  getDHT22Values(temperature, humidity); 

  Serial.print("Temp:");
  Serial.println(temperature);
  Serial.print("Humidity:");
  Serial.println(humidity);

  lastSendTime = millis();
  }
}





