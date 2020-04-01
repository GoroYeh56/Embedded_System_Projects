#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
// Set pins on I2C chip for LCD connections:
//              addr,en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3,           POSITIVE);
void setup() { 
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight
  lcd.setCursor(0, 0);  // setting cursor   
  lcd.print("Hello, world!");  
  delay(1000);  lcd.clear();
  lcd.setCursor(0, 1);  
  lcd.print("Type to display");
}
void loop() { 
  lcd.setCursor(0, 0);  // setting cursor   
  lcd.print("Microelectronics");  
  lcd.setCursor(0, 1);  
  lcd.print("Let me pass!");
  delay(1000); 
//  lcd.setCursor(0, 1);  
//  lcd.print("Let me pass!");
//  delay(1000); 
  lcd.clear();
  delay(1000);
  }
