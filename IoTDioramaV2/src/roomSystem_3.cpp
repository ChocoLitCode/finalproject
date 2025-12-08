#include "roomSystem_3.h"
#include "lcd.h"

DHT dht22(DHT22_PIN, DHT22);

// Timing for greeting
static unsigned long lastGreetingTime = 0;
static const unsigned long greetingDuration = 5000; // 5 seconds
static bool greetingActive = false;
static bool presenceDetected = false; // track if someone is present

bool setRoomThree(){
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    dht22.begin();
    return true;
}

void getDHT(float* humidity, float* temperature){
    *humidity = dht22.readHumidity();
    *temperature = dht22.readTemperature();
}

void startRoomThree(float* temperature, float* humidity, float* distance){
    // Read DHT
    getDHT(humidity, temperature);

    // Ultrasonic
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    float duration_us = pulseIn(echo, HIGH, 30000); // 30ms timeout

    // Convert only if valid pulse was read
    if(duration_us > 0){
        *distance = 0.017 * duration_us; // cm
    } else {
        *distance = NAN; // invalid reading
    }

    unsigned long now = millis();

    // ---- DETECT PRESENCE ----
    bool detected = (!isnan(*distance) && *distance < 10);

    if(detected && !presenceDetected){
        // Trigger greeting
        lcd.clear();
        lcd.setCursor(5,0);
        lcd.print("<Greetings>");
        greetingActive = true;
        lastGreetingTime = now;
        presenceDetected = true; // lock presence
        return; // skip LCD updates while greeting is active
    }

    // Reset presence flag when user leaves
    if(!detected){
        presenceDetected = false;
    }

    // ---- END GREETING ----
    if(greetingActive && now - lastGreetingTime >= greetingDuration){
        greetingActive = false;
        showLCD(); // restore static labels
    }

    // ---- UPDATE DYNAMIC VALUES ----
    if(!greetingActive){
        if(!isnan(*temperature)){
            lcd.setCursor(6,2);
            lcd.print(*temperature,1);       // 1 decimal
            lcd.setCursor(11,2);
            lcd.print((char)223);             // degree symbol
        }

        if(!isnan(*humidity)){
            lcd.setCursor(10,3);
            lcd.print(*humidity,1);           // 1 decimal
        }
    }
}
