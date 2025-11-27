/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-littlefs/ 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

// Import required libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <LittleFS.h>
#include <DHT.h>

const int DHT22_PIN = 18; // ESP32 pin GPIO18 connected to DHT22 sensor

// Indicate Type of Component
DHT dht22(DHT22_PIN, DHT22);



// Replace with your network credentials
const char* ssid = "IoT Diorama";
const char* password = "12345678";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getTemperature() {
 
  float t = dht22.readTemperature(true);
  Serial.println(t);
  return String(t);
}
  
String getHumidity() {
  float humidity = dht22.readHumidity();
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
 
  dht22.begin();

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getHumidity().c_str());
  });
  
  

  // Start server
  server.begin();
}
 
void loop(){
  
}