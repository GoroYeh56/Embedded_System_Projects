#include <Arduino_FreeRTOS.h>
//#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define KEY_ROWS 4
#define KEY_COLS 4
#define Enter_mode 0
#define answer_mode 1
#include "AnalogMatrixKeypad.h"
// include the file in your lab12 folder
// └── lab12
//      ├── lab12.ino
//      ├── AnalogMatrixKeypad.h
//      └── AnalogMatrixKeypad.cpp
AnalogMatrixKeypad keypad(A0);
// init keypad and keypad input is A0

// Set pins on I2C chip for LCD connections:
//              addr,en,rw,rs,d4,d5,d6,d7,bl,blpol

LiquidCrystal_I2C lcd(0x3F,16,2);
int redPin = 6, greenPin = 3, bluePin =5; //for RGB LED pins.
char passcode[16] = {'8', '0', '8', '0'}; //4-8digit.
int code_length = 4;
char answer_code[16];
int mode = 0;
int correct = 1;
int isSetting = 0;
int buzzer = 9;
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
//Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);

void taskKeypad(void *pvParameters);
void taskLEDTone(void *pvParameters);
//void taskRFID(void *pvParameters);
void DisplayTask(void *pvParameters);
TaskHandle_t LogIn, LED, Display;

void setup() {
    // Now set up three tasks to run independently
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect.
      }
   pinMode(buzzer, OUTPUT);  
   noTone(buzzer);  
  pinMode(redPin, OUTPUT); //set RGB LED pins.
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);   
   analogWrite(redPin, 0);
   analogWrite(greenPin, 0);
   analogWrite(bluePin, 0);       
  xTaskCreate( //Task1 : Login.
    taskKeypad,   // Pointer to function for thse task
    (const portCHAR *) "Task1", // Name for the task
    128,      // Stack size
    NULL,     // NULL task parameter
    1,        // This task will run at First priority 1
    &LogIn );   // Do not use the task handle
//  xTaskCreate(taskRFID, (const portCHAR *) "Task4", 128, NULL, 1, NULL);//Task2 : RFID
  xTaskCreate(DisplayTask, (const portCHAR *) "Task3", 128, NULL, 1, &Display);//Task3 : Display.
  xTaskCreate(taskLEDTone, (const portCHAR *) "Task2", 128, NULL, 1, &LED);//Task3 :RGB&tone. 
  lcd.init();
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight 
}

void loop() {
  // put your main code here, to run repeatedly:
}

void taskKeypad(void *pvParameters) { //measure the light intensity of a photoresistor every 5 ticks (using vTaskDelay()) and store the value in a global variable Light.
  (void) pvParameters;
   unsigned int uxPriority;
  uxPriority = uxTaskPriorityGet(LogIn); 
  for( ;; ) // A Task shall never return or exit.
   {
            //read char.
        static int i = 0;
//                Serial.print(uxPriority);
//                  Serial.println("Enter mode");
        key = keypad.readKey();
//        key = myKeypad.getKey();
        if (mode == Enter_mode) {
            if (key == '#') {
              if(i==0){ //To Set mode.
//                    SetupFun();
                    i = 0;//reset i.
                    isSetting = 1;
                    Serial.println("To set mode");
                    lcd.clear();
//                    mode = Set_code_mode;
                    for (int j = 0; j <= 8; j++) {
                      passcode[j] = {' '};
                    }
//                     vTaskPrioritySet(LogIn, 1);
//                     vTaskPrioritySet(SetUp, 3);
              }
              
              else if(i>=4){ //To Display mode.
                   i = 0;//reset.
                   mode = answer_mode;
                   vTaskSuspend(LogIn);
//                   vTaskPrioritySet(LogIn, 2);
//                   vTaskPrioritySet(Display, 3);
              }
            }
            else{ //key is not '#'.
                if(isdigit(key)){
                    if (i <= 8) {
                      if (i == code_length- 1)Serial.println(key);
                      else Serial.print(key);
                      answer_code[i] = key;
                      if (answer_code[i] != passcode[i]) {
                        Serial.print("Your input : ");
                        Serial.print(key);
                        Serial.print("passcode : ");
                        Serial.println(passcode[i]);
                        correct = 0; //Wrong.
                      }
//                      Serial.print(i);
                      i++; //move to next position.
                    }
                    else { 
                          Serial.println("Reset");
                          i = 0;
                          correct = 1;
                          for (int j = 0; j <= 15; j++) {
                            answer_code[j] = {' '};
                          } 
                    }
              }
            }
        }
//     vTaskPrioritySet(LogIn, 2);
//     vTaskPrioritySet(Display, 3);
    vTaskDelay(5);
   }
}
 int beep = 0;

void DisplayTask(void *pvParameters) { //seven segment.
  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   {            
        lcd.setCursor(0, 0);  // setting cursor
        if (mode == Enter_mode) { //read and store in answer_code.         
          lcd.setCursor(0, 0);  // setting cursor
          lcd.print("Input:");
          lcd.setCursor(0, 1);
          lcd.print(answer_code);          
//          vTaskPrioritySet(Display, 2);
//          vTaskPrioritySet(LogIn, 3);
        }
        else { //mode == answer_mode
          lcd.clear();
          if (correct)lcd.print("Correct!");
          else lcd.print("Wrong!");
          beep = 0;
          lcd.setCursor(0, 1);
          lcd.print(answer_code);
          vTaskSuspend(Display);
          delay(2000);
          lcd.clear();
          for (int j = 0; j <= 7; j++) {
            answer_code[j] = {' '};
          }
          mode = Enter_mode; //return.
          correct = 1; //reset.
//          vTaskPrioritySet(Display, 2);
//          vTaskPrioritySet(LogIn,3);
          }
        vTaskDelay(5);
   }     
}
void taskLEDTone(void *pvParameters) { //seven segment.
  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   {            
        if(mode==answer_mode){ //mode == answer_mode
           if(!beep){
              beep = 1;
                if(correct){
                    tone(buzzer, 500);
                    analogWrite(redPin, 0);
                    analogWrite(greenPin, 255);
                    analogWrite(bluePin, 0);                                  
                }
                else { 
                    tone(buzzer, 1000);
                    analogWrite(redPin, 255);
                    analogWrite(greenPin, 0);
                    analogWrite(bluePin, 0); 
                }
              vTaskDelay(1000/portTICK_PERIOD_MS);//delay 3 sec             
              analogWrite(redPin, 0);
             analogWrite(greenPin, 0);
             analogWrite(bluePin, 0);  
             noTone(buzzer);
              vTaskResume(Display);
              vTaskResume(LogIn);          
            }    
        }           
        vTaskDelay(5);
   }     
}
