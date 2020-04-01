#define KEY_ROWS 4
#define KEY_COLS 4
#define Enter_mode 0
#define Set_code_mode 1
#define answer_mode 2
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Set pins on I2C chip for LCD connections:
//              addr,en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x3F, 16,2);
char passcode[8] = {'1', '2', '3', '4',' ',' ',' ',' '}; //4-8digit.
int code_length = 4;
char answer_code[9];
int mode = 0;
int correct = 1;
char key;
char keymap[KEY_ROWS][KEY_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte colPins[KEY_COLS] = {9, 8, 7, 6};
// Column pin 1~4
byte rowPins[KEY_ROWS] = {13, 12, 11, 10}; //Initialize keypad.
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight
}

void loop() {
  // put your main code here, to run repeatedly:
  LoginFun(); delay(100);
}
int Setting = 0;
void  LoginFun() {
  DisplayTask();
  //read char.
  static int i = 0;
  key = myKeypad.getKey();
  if (mode == Enter_mode) {
      if (key == '#') {
        if(i==0){ //To Set mode.
              SetupFun();
              i = 0;//reset i.
              Serial.println("To set mode");
              lcd.clear();
              mode = Set_code_mode;
              for (int j = 0; j <= 7; j++) {
                passcode[j] = {' '};
              }
        }
        else{ //To Display mode.
             mode = answer_mode;
             DisplayTask();
             i = 0;//reset.
        }
      }
      else{ //key is not '#'.
          if(isdigit(key)){
              if (i <= 7) {
    //            if (i == code_length- 1)Serial.println(key);
    //            else Serial.print(key);
                answer_code[i] = key;
                if (answer_code[i] != passcode[i]) {
                  correct = 0; //Wrong.
                }
                Serial.print(i);
                i++; //move to next position.
              }
              else { 
                    Serial.println("Reset");
                    i = 0;
                    correct = 1;
                    for (int j = 0; j <= 7; j++) {
                      answer_code[j] = {' '};
                    } 
              }
        }
      }
  }
  else if(mode == Set_code_mode){// Setting == 1
    SetupFun();
  }
}

void SetupFun() {
//  DisplayTask();
  //read char.
  static int i = 0;
  //char key = myKeypad.getKey();
  Serial.println("Set mode");
  //When i==8: means we have press 8 digits. Press the ninth digit would made i == ten.
  if (i == 8) { 
    if(isdigit(key)){
        i = 0;
        for (int j = 0; j <= 7; j++) {
            passcode[j] = {' '};
        }
    }
  }
  else{ //for i = 0 - 7.
        if (key == '#') {
              if (i >= 3) {
                code_length = i;
                mode = Enter_mode;
                i = 0;
                lcd.clear();
              }
        }
        else if (isdigit(key)) {
          Serial.print(key);
          Serial.print("i =  ");
          Serial.println(i);
          passcode[i] = key;
          i++; //move to next position.
        }
  }
}
void DisplayTask() {
  lcd.setCursor(0, 0);  // setting cursor
  if (mode == Enter_mode) { //read and store in answer_code.
//    Serial.println(answer_code);
    lcd.setCursor(0, 0);  // setting cursor
    lcd.print("Enter passcode:");
    lcd.setCursor(0, 1);
    lcd.print(answer_code);
  }
  else if (mode == Set_code_mode) {
    lcd.print("Set passcode:   ");
    lcd.setCursor(0, 1);
    Serial.println(passcode);
    lcd.print(passcode);
    lcd.setCursor(8, 1);
    lcd.print("        ");
  }
  else { //mode == answer_mode
    lcd.clear();
    if (correct)lcd.print("Correct!");
    else lcd.print("Wrong!");
    lcd.setCursor(0, 1);
    lcd.print(answer_code);
    delay(2000);
    lcd.clear();
    for (int j = 0; j <= 7; j++) {
      answer_code[j] = {' '};
    }
    mode = Enter_mode; //return.
    correct = 1; //reset.
    return;
  }
}
