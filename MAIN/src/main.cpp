#include <Arduino.h>
#include "pins.h"
#include "doorControl.h"
#include "room1Lights.h"
#include "room2Lights.h"
#include "lcdControl.h"
#include "webServer.h"

void setup() {
    Serial.begin(115200);
    
    doorControlSetup();
    room1LightsSetup();
    room2LightsSetup();
    lcdControlSetup();
    webServerSetup();
    
    Serial.println("System Ready");
}

void loop() {
    doorControlLoop();
    room1LightsLoop();
    room2LightsLoop();
    lcdControlLoop();
    webServerLoop();
    
    delay(100);
}