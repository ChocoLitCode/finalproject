
#include "wifi.h"

  // Connect to Wi-Fi
  bool initWifi(){
     WiFi.begin(ssid, password);
     return true;
  }
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
