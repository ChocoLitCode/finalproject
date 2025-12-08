#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <LittleFS.h>
#include "doorSystem.h"
#include "roomSystem_1.h"
#include "roomSystem_2.h"
#include "roomSystem_3.h"
#include "lcd.h"

// ===== Wi-Fi Credentials =====
// Wi-Fi credentials
const char* ssid = "GFiber_2.4_Coverage_24579";
const char* password = "wZwV9vgg";

// ===== Server & WebSocket =====
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ===== Sensor Values =====
float temperature = NAN;
float humidity = NAN;
float distance = NAN;

// ===== Timing =====
unsigned long lastDHTRead = 0;
const unsigned long DHT_INTERVAL = 2000;          // 2s safe DHT read
unsigned long lastBroadcast = 0;
const unsigned long WS_BROADCAST_INTERVAL = 1000; // 1s

// ===== WebSocket Helper (FIXED) =====
void notifyClients(float temp, float hum) {
    // Use last known values if NAN is passed
    if (!isnan(temp)) temperature = temp;
    if (!isnan(hum)) humidity = hum;
    
    String tempStr = isnan(temperature) ? "0" : String(temperature, 1);
    String humStr = isnan(humidity) ? "0" : String(humidity, 1);
    String room1Str = room1_state ? "ON" : "OFF";
    String room2Str = room2_state ? "ON" : "OFF";

    String json = "{";
    json += "\"temperature\":" + tempStr + ",";
    json += "\"humidity\":" + humStr + ",";
    json += "\"room1\":\"" + room1Str + "\",";
    json += "\"room2\":\"" + room2Str + "\"";
    json += "}";

    ws.textAll(json);
}


void onWsEvent(AsyncWebSocket *serverPtr, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch(type){
        case WS_EVT_CONNECT:
            // Send initial readings
            notifyClients(temperature, humidity);
            break;

        case WS_EVT_DISCONNECT:
            break;

        case WS_EVT_DATA: {
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if(info->final && info->opcode == WS_TEXT){
                String msg = String((char*)data, len);

                if(msg == "getReadings") {
                    notifyClients(temperature, humidity);
                }

                // Room 1 Control
                if(msg.startsWith("room1:")){
                    String state = msg.substring(6);
                    if(state == "ON") {
                        room1_override = true;
                        room1_manualTarget = true;
                    } else if(state == "OFF") {
                        room1_override = true;
                        room1_manualTarget = false;
                    } else if(state == "AUTO") {
                        room1_override = false;
                    }
                    notifyClients(temperature, humidity);
                }

                // Room 2 Control
                if(msg.startsWith("room2:")){
                    String state = msg.substring(6);
                    if(state == "ON") {
                        room2_override = true;
                        room2_manualTarget = true;
                    } else if(state == "OFF") {
                        room2_override = true;
                        room2_manualTarget = false;
                    } else if(state == "AUTO") {
                        room2_override = false;
                        // RESET to OFF when entering AUTO mode
                        room2_state = false;
                    }
                    notifyClients(temperature, humidity);
                }
            }
            break;
        }

        default: break;
    }
}


// ===== Setup =====
void setup() {
    Serial.begin(115200);
    delay(100);

    // Initialize LCD & Room Sensors
    setLCD();
    showLCD();       // display static labels first
    setRoomOne();
    setRoomTwo();
    setRoomThree();
    setDoorPins();

    if(!LittleFS.begin()) Serial.println("LittleFS mount failed!");

    // Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(ssid,password);
    Serial.print("Connecting to WiFi");
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

    // WebSocket
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // Serve files
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/script.js", "application/javascript");
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
    });

    // REST endpoint
    server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
        String tempStr = isnan(temperature) ? "0" : String(temperature,1);
        String humStr = isnan(humidity) ? "0" : String(humidity,1);
        String json = "{\"temperature\":" + tempStr + ",\"humidity\":" + humStr + "}";
        request->send(200,"application/json",json);
    });

    server.begin();
    Serial.println("HTTP server started");
}

// ===== Loop =====
void loop() {
    unsigned long now = millis();

    // ----- Read DHT22 -----
    if(now - lastDHTRead >= DHT_INTERVAL){
        lastDHTRead = now;
        float t,h;
        getDHT(&h,&t);
        if(!isnan(t)) temperature = t;
        if(!isnan(h)) humidity = h;
    }

    // ----- Update LCD & Ultrasonic -----
    startRoomThree(&temperature, &humidity, &distance);

    // DoorSystem
    startDoor(ws);

    startRoomOne(notifyClients);
startRoomTwo(notifyClients);


    // ----- WebSocket Broadcast -----
    if(now - lastBroadcast >= WS_BROADCAST_INTERVAL){
        lastBroadcast = now;
        notifyClients(temperature, humidity);
    }

    // ----- Cleanup WS Clients -----
    static unsigned long lastCleanup = 0;
    if(now - lastCleanup > 5000){
        lastCleanup = now;
        ws.cleanupClients();
    }

    delay(1); // yield
}
