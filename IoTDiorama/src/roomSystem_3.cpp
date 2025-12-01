#include "roomSystem_3.h"
#include "lcd.h"

DHT dht22(DHT22_PIN, DHT22);

bool setRoomThree(){
  pinMode(trig, OUTPUT);// configure the trigger pin to output mode
  pinMode(echo, INPUT);// configure the echo pin to input mode
  dht22.begin(); // initialize the DHT22 sensor
  return true;
}

void getDHT(float* humidity, float* temperature){
  *humidity = dht22.readHumidity();
  *temperature = dht22.readTemperature();
}

void startRoomThree(){
  float humi, tempC;
  getDHT(&humi, &tempC);

  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  float duration_us = pulseIn(echo, HIGH);// measure duration of pulse from ECHO pin
  float distance_cm = 0.017 * duration_us;// calculate the distance
  Serial.println("Distance: ");
  Serial.print(distance_cm);

     if ( isnan(tempC) || isnan(humi)) {
    Serial.println("Failed to read from DHT22 sensor!");
  } else {
    lcd.setCursor(6, 2); 
    lcd.print(tempC);
    lcd.setCursor(11, 2); 
    lcd.print((char)223);
    lcd.setCursor(12, 2); 
    lcd.print("C");
    lcd.setCursor(10, 3); 
    lcd.print(humi);
    lcd.setCursor(15,3);
    lcd.print("%");
  }

  if(distance_cm < 10){
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("<Greetings>");
    delay(5000);
    lcd.clear();
  }
}



