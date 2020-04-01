#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define KEY_ROWS 4
#define KEY_COLS 4
#define Enter_mode 0
#define answer_mode 1
#include "AnalogMatrixKeypad.h"
#include <semphr.h>
SemaphoreHandle_t  gatekeeper = 0; /* global handler */

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
///////////////////////////////////////////////////////
///////////////////// For RFID  ///////////////////////
///////////////////////////////////////////////////////
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN      A1        // reset pin
#define SS_PIN       10        // select pin for the chip
#define IRQ_PIN         2           // Configurable, depends on hardware

MFRC522 mfrc522(SS_PIN, RST_PIN);  // create MFRC522 object
MFRC522::MIFARE_Key KEY;

byte myUID[4] = {0xE9,0x42,0x8B,0xAB};
//Interrupt.
volatile bool bNewInt = false;
byte regVal = 0x7F;
void activateRec(MFRC522 mfrc522);
void clearInt(MFRC522 mfrc522);
int nowKeypad = 0;

struct RFIDTag {    // 定義結構
   byte uid[4];
   char *name;
};
//struct RFIDTag myUID = {  // 初始化結構資料
//{0xE9,0x42,0x8B,0xAB}, "MyCard"
//};
// use buffer size 18 to store the value
byte buffer[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
MFRC522::StatusCode status;
///////////////////////////////////////////////////////

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
void taskRFID(void *pvParameters);
void DisplayTask(void *pvParameters);
TaskHandle_t LogIn, LED, Display;


void readCard();
void activateRec(MFRC522 mfrc522);
void clearInt(MFRC522 mfrc522);
void dump_byte_array(byte *buffer, byte bufferSize);
void writeBlock(byte _sector, byte _block, byte _blockData[]);
void readBlock(byte _sector, byte _block, byte _blockData[]);
void printRFIDdata();
void setup() {
    // Now set up three tasks to run independently
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect.
      }
    gatekeeper = xSemaphoreCreateMutex();
      
/////////////////////////////////////////      
  /* Set up for RFID interrupt. */
    SPI.begin();          // Init SPI bus

  mfrc522.PCD_Init(); // Init MFRC522 card
    
    // init the key with 0XFF
    for (byte i = 0; i < 6; i++) {
    KEY.keyByte[i] = 0xFF;
    }
  /* read and printout the MFRC522 version (valid values 0x91 & 0x92)*/
  Serial.print(F("Ver: 0x"));
  byte readReg = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.println(readReg, HEX);

  /* setup the IRQ pin*/
  pinMode(IRQ_PIN, INPUT_PULLUP);

  /*
   * Allow the ... irq to be propagated to the IRQ pin
   * For test purposes propagate the IdleIrq and loAlert
   */
  regVal = 0xA0; //rx irq
  mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);

  bNewInt = false; //interrupt flag
  /*Activate the interrupt*/
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), readCard, FALLING);
//  do { //clear a spourious interrupt at start
//    ;
//  } while (!bNewInt);
  bNewInt = false;

  Serial.println(F("End setup"));
////////////////////////////////////
  
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
    144,      // Stack size
    NULL,     // NULL task parameter
    2,        // This task will run at First priority 1
    &LogIn );   // Do not use the task handle
  xTaskCreate(taskLEDTone, (const portCHAR *) "Task3", 100, NULL, 2, &LED);//Task3 :RGB&tone. 
  xTaskCreate(taskRFID, (const portCHAR *) "Task4", 144, NULL, 2, NULL);//Task4 : RFID  //Priority : two  Keypad take semaphore first.
  lcd.init();
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight 
   vTaskStartScheduler();
}

void loop() {                 
}

void taskKeypad(void *pvParameters) { //measure the light intensity of a photoresistor every 5 ticks (using vTaskDelay()) and store the value in a global variable Light.
  (void) pvParameters;
   unsigned int uxPriority;
  uxPriority = uxTaskPriorityGet(LogIn); 
  for( ;; ) // A Task shall never return or exit.
   {
//    Serial.println("keypad task");
//        if(xSemaphoreTake(gatekeeper, 100)){
          if(nowKeypad){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Input:");
              lcd.setCursor(0,1);
              lcd.print(answer_code);
              Serial.println("keypad got access");
              /* critical section *///(從拿到->到放開)
          }

            //read char.
        static int i = 0;
        key = keypad.readKey();
        
        if (mode == Enter_mode) {
            if (key == '#') {
              if(i>=4){ //To answer mode.                 
                   i = 0;//reset.
                   mode = answer_mode;
                   if(correct){
                      lcd.setCursor(0,0);
                      lcd.print("Correct!");
                   }
                   else{
                      lcd.setCursor(0,0);
                      lcd.print("Wrong!"); 
                   }
                  vTaskDelay(1000/portTICK_PERIOD_MS);//delay 3 sec 
              }
            }
            else{ //key is not '#'.
                if(isdigit(key)){
                    if(i==0){
                      nowKeypad = 1;
                    }
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
//            xSemaphoreGive(gatekeeper);  
//        }
            vTaskDelay(5);
   }
}

 int beep = 0;
 int UID_wrong = 0;
 byte *id;   // get UID of the card
 byte idSize;
//void DisplayTask(void *pvParameters) { //seven segment.
//  (void) pvParameters;
//  for( ;; ) // A Task shall never return or exit.
//   {            
//        lcd.setCursor(0, 0);  // setting cursor
//        if (mode == Enter_mode) { //read and store in answer_code.         
//          lcd.setCursor(0, 0);  // setting cursor
//          lcd.print("Input:");
//          lcd.setCursor(0, 1);
//          lcd.print(answer_code);          
////          vTaskPrioritySet(Display, 2);
////          vTaskPrioritySet(LogIn, 3);
//        }
//        else { //mode == answer_mode
//          lcd.clear();
//          if (correct)lcd.print("Correct!");
//          else lcd.print("Wrong!");
//          if(UID_wrong){
//            lcd.setCursor(6,0);
////            lcd.print(id,HEX);
//            for (byte i = 0; i < idSize; i++) {
//                  lcd.setCursor(6+i,0);
//                  lcd.print(id[i],HEX);
//                  }                  
//            }
//          }
//          beep = 0;
//          lcd.setCursor(0, 1);
//          lcd.print(answer_code);
//          vTaskSuspend(Display);
//          delay(2000);
//          lcd.clear();
//          for (int j = 0; j <= 7; j++) {
//            answer_code[j] = {' '};
//          }
//          mode = Enter_mode; //return.
//          correct = 1; //reset.
////          vTaskPrioritySet(Display, 2);
////          vTaskPrioritySet(LogIn,3);
////          }
//        vTaskDelay(5);
//   }     
//}
void taskLEDTone(void *pvParameters) { //seven segment.
  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   {            
        if(mode==answer_mode){ //mode == answer_mode
//           if(!beep){
//              beep = 1;
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
            mode = Enter_mode;
correct = 1;
            if(nowKeypad) nowKeypad = 0; //reset flag.
            for (int j = 0; j <= 8; j++) {
              answer_code[j] = {' '};
              }
        }           
        vTaskDelay(5);
   }     
}
void taskRFID(void *pvParameters) { //RFID : if card == Mycard : Correct, else Wrong.
  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   { 
         if(!nowKeypad){
//       if(xSemaphoreTake(gatekeeper, 100)){
         lcd.setCursor(0,0);
         lcd.clear();
         lcd.print("Input:");         
          Serial.println("RFID got access");
          /* critical section */

        if (bNewInt) { //new read interrupt
          Serial.print(F("Interrupt. "));
          mfrc522.PICC_ReadCardSerial(); //read the tag data
          // Show some details of the PICC (that is: the tag/card)
          Serial.print(F("Card UID:"));
          dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
          Serial.println();
      
//          clearInt(mfrc522);
//          mfrc522.PICC_HaltA();
    // check if a new card is valid
//    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
//         *id = mfrc522.uid.uidByte;   // get UID of the card
//         idSize = mfrc522.uid.size;   // get UID length
//        Serial.print("PICC type: ");      // show card type
////        // according to the SAK value (mfrc522.uid.sak) in response , identify the card type //SAK:select acknowledge
//
//        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
//        Serial.println(mfrc522.PICC_GetTypeName(piccType));
//        Serial.print("UID Size: ");       // show UID length
//        Serial.println(idSize);
//
//        for (byte i = 0; i < idSize; i++) {
//            Serial.print("id[");
//            Serial.print(i);
//            Serial.print("]: ");
//            Serial.println(id[i], HEX);       // show UID value in hexdecimal
//        }
//        Serial.println();
             beep = 0; //reset beep. (haven't beep)
             //Compare two array, array1's ptr, array 2's ptr, size_t.
            if (memcmp(myUID, mfrc522.uid.uidByte, mfrc522.uid.size) == 0) { //Same, <0 : First < Second, and vice versa.
//              Serial.println(myUID.name);  // 顯示標籤的名稱
              Serial.println("Correct!");  // 顯示標籤的名稱  
              correct = 1;  
              lcd.setCursor(0,0);
              lcd.print("Correct!");              
              UID_wrong = 0;
//              delay(1000);
              mode = answer_mode ;   
              vTaskDelay(1000/portTICK_PERIOD_MS);//delay 3 sec   
            }
            else{
              correct = 0;
              UID_wrong = 1;
              Serial.println("Wrong!");
              lcd.setCursor(0,0);
              lcd.print("Wrong!");
              for(byte j=0; j<mfrc522.uid.size; j++){
                lcd.setCursor(6+j*2,0);
                lcd.print(mfrc522.uid.uidByte[j],HEX);
              }
//              delay(1000);
              mode = answer_mode ; 
              vTaskDelay(1000/portTICK_PERIOD_MS);//delay 3 sec 
            }
            // halt the card
            mfrc522.PICC_HaltA();
            // Stop encryption on PCD, then you can read or write the sector and block many times
            mfrc522.PCD_StopCrypto1();
             bNewInt = false; 
        }//end bNewInt

        // The receiving block needs regular retriggering (tell the tag it should transmit??)
        // (mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg,mfrc522.PICC_CMD_REQA);)
        activateRec(mfrc522);
//        delay(1000);   //one second.  
//        xSemaphoreGive(gatekeeper); 
       }
        vTaskDelay(5);
  }
}   

/////////////// RFID interrupt fxns. ////////////////////////
/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
/**
 * MFRC522 interrupt serving routine
 */
void readCard() {
  bNewInt = true;
  nowKeypad = 0;
}

/*
 * The function sending to the MFRC522 the needed commands to activate the reception
 */
void activateRec(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
}

/*
 * The function to clear the pending interrupt bits after interrupt serving routine
 */
void clearInt(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
}
