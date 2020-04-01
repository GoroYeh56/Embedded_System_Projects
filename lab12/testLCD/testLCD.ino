#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
// Set pins on I2C chip for LCD connections:

LiquidCrystal_I2C lcd(0x3F,16,2);
// Set I2C address and size
void setup() { 
  lcd.init();   // initialize LCD
  lcd.backlight();    // open LCD backlight
  lcd.setCursor(0, 0);  // setting cursor   
  lcd.print("Hello, world!");  
  delay(1000);  lcd.clear(); // clear all 
  lcd.setCursor(0, 1);  
  lcd.print("Type to display");
}
void loop() { }
