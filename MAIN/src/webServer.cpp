#include <ESPAsyncWebServer.h>
#include "webServer.h"
#include "pins.h"
#include "doorControl.h"
#include "room1Lights.h"
#include "room2Lights.h"
#include "lcdControl.h"

AsyncWebServer server(80);

const char* ssid = "IoT Diorama";
const char* password = "12345678";

String intruderMessage = "Intruder Alert!";

String getSensorData() {
    String data = "{";
    data += "\"temperature\":" + String(getTemperature(), 1) + ",";
    data += "\"humidity\":" + String(getHumidity(), 1) + ",";
    data += "\"room1\":" + String(isRoom1LightOn() ? "true" : "false") + ",";
    data += "\"room2\":" + String(isRoom2LightOn() ? "true" : "false");
    data += "}";
    return data;
}

void webServerSetup() {
    if(!LittleFS.begin()){
        Serial.println("LittleFS Mount Failed");
        return;
    }

   // Set up ESP32 as Access Point (creates its own WiFi network)
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    
    Serial.println("Access Point Created!");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    // Serve static files from LittleFS
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    // API routes - using the defined constants
    server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "application/json", getSensorData());
    });

    server.on("/api/room1", HTTP_POST, [](AsyncWebServerRequest *request){
        if(request->hasParam("state", true)) {
            String state = request->getParam("state", true)->value();
            setRoom1Light(state == "on");
        }
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/room2", HTTP_POST, [](AsyncWebServerRequest *request){
        if(request->hasParam("state", true)) {
            String state = request->getParam("state", true)->value();
            setRoom2Light(state == "on");
        }
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/intruder", HTTP_GET, [](AsyncWebServerRequest *request){
        if (isIntruderDetected()) {
            request->send(200, "application/json", "{\"intruder\":true,\"message\":\"INTRUDER DETECTED! Failed door attempts!\"}");
            resetIntruder();
        } else {
            request->send(200, "application/json", "{\"intruder\":false}");
        }
    });

    // 404 handler
    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Page not found");
    });

    server.begin();
}

void webServerLoop() {
    // Nothing needed here for async server
}