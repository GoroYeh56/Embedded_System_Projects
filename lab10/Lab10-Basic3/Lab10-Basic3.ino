#include <Arduino_FreeRTOS.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define KEY_ROWS 4
#define KEY_COLS 4
#define Enter_mode 0
#define Set_code_mode 1
#define answer_mode 2
// Set pins on I2C chip for LCD connections:
//              addr,en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3,           POSITIVE);
char passcode[16] = {'1', '2', '3', '4'}; //4-8digit.
int code_length = 4;
char answer_code[9];
int mode = 0;
int correct = 1;
int isSetting = 0;
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

void LoginTask(void *pvParameters);
void SetupTask(void *pvParameters);
void DisplayTask(void *pvParameters);
TaskHandle_t LogIn, SetUp, Display;

void setup() {
    // Now set up three tasks to run independently
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect.
      }
  xTaskCreate( //Task1 : Login.
    LoginTask,   // Pointer to function for thse task
    (const portCHAR *) "Task1", // Name for the task
    128,      // Stack size
    NULL,     // NULL task parameter
    1,        // This task will run at First priority 1
    &LogIn );   // Do not use the task handle
  xTaskCreate(SetupTask, (const portCHAR *) "Task2", 128, NULL, 1, &SetUp);//Task2 : Set code.
  xTaskCreate(DisplayTask, (const portCHAR *) "Task3", 128, NULL, 1, &Display);//Task3 : Display.
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight
}

void loop() {
  // put your main code here, to run repeatedly:
}

void LoginTask(void *pvParameters) { //measure the light intensity of a photoresistor every 5 ticks (using vTaskDelay()) and store the value in a global variable Light.
  (void) pvParameters;
   unsigned int uxPriority;
  uxPriority = uxTaskPriorityGet(LogIn); 
  for( ;; ) // A Task shall never return or exit.
   {
            //read char.
        static int i = 0;
//                Serial.print(uxPriority);
//                  Serial.println("Enter mode");
        key = myKeypad.getKey();
        if (mode == Enter_mode) {
            if (key == '#') {
              if(i==0){ //To Set mode.
//                    SetupFun();
                    i = 0;//reset i.
                    isSetting = 1;
                    Serial.println("To set mode");
                    lcd.clear();
                    mode = Set_code_mode;
                    for (int j = 0; j <= 7; j++) {
                      passcode[j] = {' '};
                    }
//                     vTaskPrioritySet(LogIn, 1);
//                     vTaskPrioritySet(SetUp, 3);
              }
              
              else if(i>=4){ //To Display mode.
                   i = 0;//reset.
                   mode = answer_mode;
//                   vTaskPrioritySet(LogIn, 2);
//                   vTaskPrioritySet(Display, 3);
              }
            }
            else{ //key is not '#'.
                if(isdigit(key)){
                    if (i <= 7) {
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
                          for (int j = 0; j <= 7; j++) {
                            answer_code[j] = {' '};
                          } 
                    }
              }
            }
        }
        else if(mode == Set_code_mode){// Setting == 1
//          SetupFun();
        }
//     vTaskPrioritySet(LogIn, 2);
//     vTaskPrioritySet(Display, 3);
    vTaskDelay(5);
   }
}
void SetupTask(void *pvParameters) {//Setup new passcode. <3 : ignore, >8 : reset to zero.
  (void) pvParameters;
  unsigned int uxPriority;
  uxPriority = uxTaskPriorityGet(SetUp); 
  for( ;; ) // A Task shall never return or exit.
   {
      if(isSetting==1){
        //read char.
        static int i = 0;
//        Serial.print(uxPriority);
//        Serial.println("Set mode");
        //When i==8: means we have press 8 digits. Press the ninth digit would made i == ten.
        if (i == 8) { 
          if(isdigit(key)){
              i = 0;
              for (int j = 0; j <= 7; j++) {
                  passcode[j] = {' '};
              }
          }
          else if(key == '#'){
                    isSetting = 0;
                    code_length = i;
                    mode = Enter_mode;
                    i = 0;
                    lcd.clear();
//                     vTaskPrioritySet(SetUp, 1);
//                     vTaskPrioritySet(LogIn, 3);
          }
        }
        else{ //for i = 0 - 7.
              if (key == '#') {
                    if (i >= 3) {
                      isSetting = 0;
                      code_length = i;
                      mode = Enter_mode;
                      i = 0;
                      lcd.clear();
//                       vTaskPrioritySet(SetUp, 1);
//                       vTaskPrioritySet(LogIn, 3);
                    }
              }
              else if (isdigit(key)) {
//                Serial.print(key);
//                Serial.print("i =  ");
//                Serial.println(i);
                passcode[i] = key;
                i++; //move to next position.
              }
        }
      }
//      vTaskPrioritySet(SetUp,2);
//      vTaskPrioritySet(Display, 3);
      vTaskDelay(5);
   }
}

void DisplayTask(void *pvParameters) { //seven segment.
  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   {
        lcd.setCursor(0, 0);  // setting cursor
        if (mode == Enter_mode) { //read and store in answer_code.
          lcd.setCursor(0, 0);  // setting cursor
          lcd.print("Enter passcode:");
          lcd.setCursor(0, 1);
          lcd.print(answer_code);
//          vTaskPrioritySet(Display, 2);
//          vTaskPrioritySet(LogIn, 3);
        }
        else if (mode == Set_code_mode) {
          lcd.print("Set passcode:   ");
          lcd.setCursor(0, 1);
          Serial.println(passcode);
          lcd.print(passcode);
          lcd.setCursor(8, 1);
          lcd.print("        ");
//          vTaskPrioritySet(Display, 2);
//          vTaskPrioritySet(SetUp, 3);
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
//          vTaskPrioritySet(Display, 2);
//          vTaskPrioritySet(LogIn,3);
          }
        vTaskDelay(5);
   }     
}
