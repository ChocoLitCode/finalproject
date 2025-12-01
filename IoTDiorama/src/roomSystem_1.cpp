  #include "roomSystem_1.h"
  #include "lcd.h"

  bool setRoomOne(){
  pinMode(ldr,INPUT);
  pinMode(ldrLED1,OUTPUT);
  pinMode(ldrLED2,OUTPUT);
  pinMode(ldrLED3,OUTPUT);
  digitalWrite(ldrLED1,LOW);
  digitalWrite(ldrLED2,LOW);
  digitalWrite(ldrLED3,LOW);

  return true;
  }
  
  void startRoomOne(){
int LDRvalue = analogRead(ldr);
Serial.print("LDR Threshold: ");
Serial.println(LDRvalue);

  if (LDRvalue>ldrThreshold){
    digitalWrite(ldrLED1,HIGH);
    delay(500);
    digitalWrite(ldrLED2,HIGH);
    delay(500);
    digitalWrite(ldrLED3,HIGH);
    lcd.setCursor(16, 0);
    lcd.print("ON ");
  }
  else{
    digitalWrite(ldrLED3,LOW);
    delay(500);
    digitalWrite(ldrLED2,LOW);
    delay(500);
    digitalWrite(ldrLED1,LOW);
    lcd.setCursor(16, 0);
    lcd.print("OFF");
  }
}
