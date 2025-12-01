#include "lcd.h"

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 column and 4 rows

bool setLCD(){
  lcd.init();
  lcd.backlight();
  
  return true;
}

void showLCD(){
  lcd.setCursor(0, 0); 
  lcd.print("Room #1 Lights: ");
  lcd.setCursor(0, 1); 
  lcd.print("Room #2 Lights: ");
  lcd.setCursor(0, 2);           
  lcd.print("Temp: ");
  lcd.setCursor(0,3);
  lcd.print("Humidity: ");
}