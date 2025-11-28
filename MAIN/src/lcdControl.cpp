#include "lcdControl.h"
#include "pins.h"
#include "room1Lights.h"
#include "room2Lights.h"

DHT dht22(DHT22_PIN, DHT22);
LiquidCrystal_I2C lcd(0x27, 20, 4);

float currentTemperature = 0;
float currentHumidity = 0;

void lcdControlSetup() {
    dht22.begin();
    lcd.init();
    lcd.backlight();
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

void greetingsLCD() {
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    float duration_us = pulseIn(echo, HIGH);
    float distance_cm = 0.017 * duration_us;

    if (distance_cm < 10) {
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("<Greetings>");
        delay(5000);
        lcd.clear();
    }
}

void defaultLCD() {
    currentHumidity = dht22.readHumidity();
    currentTemperature = dht22.readTemperature();

    if (isnan(currentTemperature) || isnan(currentHumidity)) {
        Serial.println("Failed to read from DHT22 sensor!");
        return;
    }

    lcd.setCursor(0, 0); 
    lcd.print("Room #1 Lights: ");
    lcd.setCursor(16, 0);
    lcd.print(isRoom1LightOn() ? "ON " : "OFF");

    lcd.setCursor(0, 1); 
    lcd.print("Room #2 Lights: ");
    lcd.setCursor(16, 1);
    lcd.print(isRoom2LightOn() ? "ON " : "OFF");

    lcd.setCursor(0, 2);           
    lcd.print("Temp: ");
    lcd.setCursor(6, 2); 
    lcd.print(currentTemperature, 1);
    lcd.setCursor(11, 2); 
    lcd.print((char)223);
    lcd.setCursor(12, 2); 
    lcd.print("C");

    lcd.setCursor(0,3);
    lcd.print("Humidity: ");
    lcd.setCursor(10, 3); 
    lcd.print(currentHumidity, 1);
    lcd.setCursor(15,3);
    lcd.print("%");
}

void lcdControlLoop() {
    greetingsLCD();
    defaultLCD();
}

float getTemperature() {
    return currentTemperature;
}

float getHumidity() {
    return currentHumidity;
}