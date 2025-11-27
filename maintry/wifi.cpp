
#include "wifi.h"

bool initWifi(){
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi");
        Serial.println(WiFi.localIP());
        return true;
    } 
}
