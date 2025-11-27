// Global

#include "dht22.h"
#include "wifi.h"
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <LittleFS.h>

long lastSendTime = 0;
long interval = 2000;



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

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



  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getHumidity().c_str());
  });
  
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getPressure().c_str());
  });

  // Start server
  server.begin();
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





