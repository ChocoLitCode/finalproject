#include "roomSystem_2.h"
#include "lcd.h"

bool room2_override = false;
bool room2_state = false;
bool room2_manualTarget = false;

static bool wasAboveThreshold = false;

void setRoomTwo() {
    pinMode(sound, INPUT);
    pinMode(soundLED, OUTPUT);
    digitalWrite(soundLED, LOW);
}

void startRoomTwo(void (*notify)(float,float)) {
    static bool lastState = false;
    static bool lastOverride = false;
    static unsigned long lastNotifyTime = 0;
    unsigned long now = millis();

    if(room2_override){
        // Manual override: use manualTarget
        room2_state = room2_manualTarget;
        wasAboveThreshold = false; // Reset toggle detection
    } else {
        // Auto mode: toggle on sound events
        // FIXED: Reset wasAboveThreshold when switching from override to auto
        if(lastOverride){
            wasAboveThreshold = false;
        }
        
        int soundValue = analogRead(sound);
        if(soundValue > soundThreshold){
            if(!wasAboveThreshold){
                room2_state = !room2_state; // toggle LED
                wasAboveThreshold = true;
            }
        } else {
            wasAboveThreshold = false;
        }
    }

    lastOverride = room2_override;
    
    digitalWrite(soundLED, room2_state);
    lcd.setCursor(16,1);
    lcd.print(room2_state ? "ON " : "OFF");

    // Notify WebSocket if state changed
    if(room2_state != lastState && now - lastNotifyTime > 200){
        if(notify) notify(NAN, NAN);
        lastState = room2_state;
        lastNotifyTime = now;
    }
}