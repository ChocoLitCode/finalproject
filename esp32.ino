// Libraries
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Indicate Type of Component
DHT dht22(DHT22_PIN, DHT22);
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 column and 4 rows

// Pin Declarations
const int touch1 = 33;
const int touch2 = 32;
const int doorLED1 = 14;
const int doorLED2 = 27;
const int accessLED = 26;    
const int intruderLED = 25;
const int buzzer = 13;
const int ldr = 35;
const int sound = 34;              
const int ldrLED1 = 4;
const int ldrLED2 = 16;
const int ldrLED3 = 17;
const int soundLED = 2;
const int DHT22_PIN = 18; // ESP32 pin GPIO18 connected to DHT22 sensor
const int trig = 19; // ESP32 pin GPIO19 connected to Ultrasonic Sensor's TRIG pin
const int echo = 23; // ESP32 pin GPIO23 connected to Ultrasonic Sensor's ECHO pin


// Definitions
int failAttempts = 0;
unsigned long touchStart = 0;
const int soundThreshold = 1;
bool ledState = false;           // Tracks the current state of the LED (false = OFF, true = ON)
bool wasAboveThreshold = false; 

void setup() {
  Serial.begin(115200);

  pinMode(doorLED1, OUTPUT);
  pinMode(doorLED2, OUTPUT);
  pinMode(accessLED, OUTPUT);   
  pinMode(intruderLED, OUTPUT);  
  pinMode(buzzer, OUTPUT);
  digitalWrite(doorLED1, LOW);
  digitalWrite(doorLED2, LOW);
  digitalWrite(accessLED, LOW);
  digitalWrite(intruderLED, LOW);

  Serial.println("Door System Ready");

  pinMode(ldr,INPUT);
  pinMode(ldrLED1,OUTPUT);
  pinMode(ldrLED2,OUTPUT);
  pinMode(ldrLED3,OUTPUT);
  digitalWrite(ldrLED1,LOW);
  digitalWrite(ldrLED2,LOW);
  digitalWrite(ldrLED3,LOW);

  pinMode(sound,INPUT);
  pinMode(soundLED, OUTPUT);
  digitalWrite(soundLED,LOW);

  pinMode(trig, OUTPUT);// configure the trigger pin to output mode
  pinMode(echo, INPUT);// configure the echo pin to input mode

  dht22.begin(); // initialize the DHT22 sensor
  lcd.init(); // initialize the lcd
  lcd.backlight();
}

void loop() {
  doorControl();
  greetingsLCD();
  room1Lights();
  room2Lights();
  delay(100);
}
 
void doorControl(){

  int t1 = digitalRead(touch1);
  int t2 = digitalRead(touch2);

  bool bothTouched = (t1 == HIGH && t2 == HIGH);

  if (bothTouched) {

    if (touchStart == 0) {
      touchStart = millis();
    }

    if (millis() - touchStart >= 3000) {
      unlockDoor();
      touchStart = 0;
      failAttempts = 0;
    }
  } 
  else {
    if (touchStart != 0) {
      failAttempts++;

      if (failAttempts >= 3)
        intruderAlert();

      touchStart = 0;
    }
  }
}

void unlockDoor() {
  Serial.println("Access Granted -");

  tone(buzzer, 1000, 150);

  digitalWrite(doorLED1, HIGH);
  digitalWrite(doorLED2, HIGH);

  digitalWrite(accessLED, HIGH);     
  digitalWrite(intruderLED, LOW);  
}


void intruderAlert() {
  Serial.println("INTRUDER DETECTED");

  for (int i = 0; i < 3; i++) {
    tone(buzzer, 2000, 200);
    delay(250);
  }

  digitalWrite(doorLED1, LOW);
  digitalWrite(doorLED2, LOW);

  digitalWrite(accessLED, LOW);     
  digitalWrite(intruderLED, HIGH); 

  failAttempts = 0;
}

void room1Lights(){
int LDRvalue = analogRead(ldr);
Serial.println("LDR Threshold: ");
Serial.print(LDRvalue);

  if (LDRvalue>2500){
    digitalWrite(LED1_PIN,HIGH);
    delay(500);
    digitalWrite(LED2_PIN,HIGH);
    delay(500);
    digitalWrite(LED3_PIN,HIGH);
    lcd.setCursor(16, 0);
    lcd.print("ON");
  }
  else{
    digitalWrite(LED3_PIN,LOW);
    delay(500);
    digitalWrite(LED2_PIN,LOW);
    delay(500);
    digitalWrite(LED1_PIN,LOW);
    lcd.setCursor(16, 1);
    lcd.print("OFF");
  }
}

void room2Lights(){

  int soundValue = analogRead(sound);   
  Serial.println("Sound Value: ");
  Serial.print(soundValue);

// Check if the sound intensity is above the threshold
  if (soundValue > soundThreshold) {
    // If the sound is above the threshold and it was NOT above the threshold
    // in the previous loop cycle, this is a "rising edge" event.
    if (!wasAboveThreshold) {
      // Invert the LED's state
      ledState = !ledState;

      // Update the LED to the new state
      digitalWrite(soundLED, ledState);

      // Print a message to the Serial Monitor
      if (ledState) {
        Serial.println("LED is now ON.");
        lcd.setCursor(16, 1);
        lcd.print("ON");
      } else {
        Serial.println("LED is now OFF.");
        lcd.setCursor(16, 1);
        lcd.print("OFF");
      }

      // Set the flag to true to prevent the toggle from happening
      // repeatedly while the sound is still above the threshold.
      wasAboveThreshold = true;
    }
  } else {
    // If the sound is below the threshold, reset the flag.
    // This allows the toggle to happen again the next time the threshold is met.
    wasAboveThreshold = false;
  }
}

void greetingsLCD(){
 // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(echo, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;
  Serial.println("Distance: ");
  Serial.print(distance_cm);

  if(distance_cm < 10){
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("<Greetings>");
    delay(5000);
    lcd.clear();
  }
  else{
    defaultLCD();
  }
}


void defaultLCD(){
    // read humidity
    float humi  = dht22.readHumidity();
    // read temperature in Celsius
    float tempC = dht22.readTemperature();

   if ( isnan(tempC) || isnan(humi)) {
    Serial.println("Failed to read from DHT22 sensor!");
  } else {

    lcd.setCursor(0, 0); 
    lcd.print("Room #1 Lights: ");

    lcd.setCursor(0, 1); 
    lcd.print("Room #2 Lights: ");

    lcd.setCursor(0, 2);           
    lcd.print("Temp: ");

    lcd.setCursor(6, 2); 
    lcd.print(tempC);

    lcd.setCursor(11, 2); 
    lcd.print((char)223);

    lcd.setCursor(12, 2); 
    lcd.print("C");

    lcd.setCursor(0,3);
    lcd.print("Humidity: ");

    lcd.setCursor(10, 3); 
    lcd.print(humi);

    lcd.setCursor(15,3);
    lcd.print("%");
  }
}