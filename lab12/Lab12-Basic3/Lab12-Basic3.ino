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
byte blockData[16] = "dddddddddddddddd";   // max write data length is 16
int redPin = 6, greenPin = 3, bluePin =5; //for RGB LED pins.
char passcode[9] = {'8', '0', '8', '0',' ',' ',' ',' ',' '}; //4-8digit.
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
            lcd.setCursor(0,0);
         lcd.clear();
         lcd.print("Input:"); 
   vTaskStartScheduler();
}
char h;
void loop() {              
}

void taskKeypad(void *pvParameters) { //measure the light intensity of a photoresistor every 5 ticks (using vTaskDelay()) and store the value in a global variable Light.
  (void) pvParameters;
   unsigned int uxPriority;
  uxPriority = uxTaskPriorityGet(LogIn); 
  for( ;; ) // A Task shall never return or exit.
   {
//        if(xSemaphoreTake(gatekeeper, 10)){
          if(nowKeypad){
//              lcd.clear();
//              lcd.setCursor(0,0);
//              lcd.print("Input:");
              lcd.setCursor(0,1);
              lcd.print(answer_code);
//              Serial.println("keypad got access");
              /* critical section *///(從拿到->到放開)
//              xSemaphoreGive(gatekeeper);
          }

            //read char.
        static int i = 0;
        key = keypad.readKey();
        if(isdigit(key)){
          nowKeypad = 1;
           xSemaphoreTake(gatekeeper, 10);
        }
        if (mode == Enter_mode) {
            if (key == '#') {
              nowKeypad = 1; //To clear answer_code.
              if(i>=4){ //To answer mode.                 
                   i = 0;//reset.
                   mode = answer_mode;
                   for(int j=0; j<=8;j++){
                      if(answer_code[j] != passcode[j])correct =0;
                   }
                   if(correct){
                      lcd.setCursor(0,0);
                      lcd.print("Correct!");
                   }
                   else{
                      lcd.setCursor(0,0);
                      lcd.print("Wrong!"); 
                   }
                  lcd.setCursor(0,1);
                  lcd.print(answer_code);
                  vTaskDelay(1000/portTICK_PERIOD_MS);//delay 3 sec 
                  for(int i=0; i<=8; i++) answer_code[i] = ' ';
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Input:");                  
                  xSemaphoreGive(gatekeeper);
              }
            }
            else{ //key is not '#'.
                if(isdigit(key)){
                    if (i <= 8) {
//                      if (i == code_length- 1)Serial.println(key);
//                      else Serial.print(key);
                      answer_code[i] = key;
                      if (answer_code[i] != passcode[i]) {
                        Serial.print("Your input : ");
                        Serial.print(key);
                        Serial.print("passcode : ");
                        Serial.println(passcode[i]);
                        correct = 0; //Wrong.
                      }
                      i++; //move to next position.
                    }
                    else { //input more than 9 numbers.
                          Serial.println("Reset");
                          i = 0;//Reset everything.
                          correct = 1;
                          for (int j = 0; j <= 8; j++) {
                            answer_code[j] = {' '};
                          }
                    }
                }
            }
//            if(i==0){
//              xSemaphoreGive(gatekeeper);
//            }
        }
            vTaskDelay(5);
   }
}

 int beep = 0;
 int UID_wrong = 0;
 byte *id;   // get UID of the card
 byte idSize;

void taskLEDTone(void *pvParameters) { //seven segment.
  (void) pvParameters;
  for( ;; ) // A Task shall never return or exit.
   {            
        if(mode==answer_mode){ //mode == answer_mode
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
            if(nowKeypad) nowKeypad = 0; 
        }
        vTaskDelay(5);
   }     
}
void taskRFID(void *pvParameters) { //RFID : if card == Mycard : Correct, else Wrong.
  (void) pvParameters;
  int found = 0;
  char h;
  for( ;; ) // A Task shall never return or exit.
   { 
        if (bNewInt) { //new read interrupt
          Serial.print(F("Interrupt. "));
          mfrc522.PICC_ReadCardSerial(); //read the tag data
          // Show some details of the PICC (that is: the tag/card)
          Serial.print(F("Card UID:"));
          dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
          Serial.println();
      
             beep = 0; //reset beep. (haven't beep)
             //Compare two array, array1's ptr, array 2's ptr, size_t.
            if (memcmp(myUID, mfrc522.uid.uidByte, mfrc522.uid.size) == 0) { //Same, <0 : First < Second, and vice versa.
//              Serial.println(myUID.name);  // 顯示標籤的名稱
              Serial.println("Correct!");  // 顯示標籤的名稱  
              correct = 1;        
              UID_wrong = 0;
              mode = answer_mode ;   
            }
            else{ //if incorrect, find out flag.
               for(int sector=0; sector<=15; sector++){
                    for(int block = 0; block <=2; block++){
                          readBlock( sector, block, buffer);
//                          h = buffer[0];
                          if( buffer[0] == 0x64){
                              found = 1;
                              printRFIDdata();
                              break;
                          }
//                          if(found)break;
                    }
                        if(found)break;
               }
              correct = 0;
              UID_wrong = 1;
              Serial.println("Wrong!");
            }
               mode = answer_mode ; 
            // halt the card
            mfrc522.PICC_HaltA();//Proximity Coupling device.
            // Stop encryption on PCD, then you can read or write the sector and block many times
            mfrc522.PCD_StopCrypto1();
             bNewInt = false;  
               if(xSemaphoreTake(gatekeeper, 10)){
                  if(correct){
                      lcd.setCursor(0,0);
                      lcd.print("Correct!");                     
                  }
                  else{
                      //First row.
                      lcd.setCursor(0,0);
                      lcd.print("Wrong!");
                      for(byte j=0; j<mfrc522.uid.size; j++){
                        lcd.setCursor(6+j*2,0);
                        lcd.print(mfrc522.uid.uidByte[j],HEX);
                      }                    
                    if(found){
                      Serial.println("Found");
                       for(byte i=0; i<=15; i++){
                          lcd.setCursor(0+i,1);
//                          h = (char)buffer[i];
                          lcd.print((char)buffer[i]);
//                          Serial.write(buffer[i]);
////                         lcd.write(buffer[i]);
                       }
                       printRFIDdata();
                       found = 0;//reset.   
                     }
                    else{
                        lcd.setCursor(0,1);
                        lcd.print("Not found flag.");         
                    }
                 }
                  vTaskDelay(1000/portTICK_PERIOD_MS);//delay 3 sec     
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Input:");                             
                   xSemaphoreGive(gatekeeper);
               }                          
        }//end bNewInt
                          

        // The receiving block needs regular retriggering (tell the tag it should transmit??)
        activateRec(mfrc522);  
        vTaskDelay(5);
  }
}   
/////////////// RFID Read fxns. ////////////////////////
void readBlock(byte _sector, byte _block, byte _blockData[])  {
    if (_sector < 0 || _sector > 15 || _block < 0 || _block > 2) {
        // check the validation of sector and block
        Serial.println(F("Wrong sector or block number."));
        return;
    }

    byte blockNum = _sector * 4 + _block;  // get the real block number（0~63）
    byte trailerBlock = _sector * 4 + 3;   // get the trailer block

    // key validation
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &KEY, &(mfrc522.uid));
    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    byte buffersize = 18;
    // read the data from the block and save the result to _blockData
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockNum, _blockData, &buffersize);

    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    Serial.println(F("Data was read."));
}

void printRFIDdata() {
    Serial.print(F("Read block: "));
    for (byte i = 0 ; i < 16 ; i++) {
        Serial.write(buffer[i]);
    }
    Serial.println();
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

void writeBlock(byte _sector, byte _block, byte _blockData[]) {
    if (_sector < 0 || _sector > 15 || _block < 0 || _block > 3) {
        // check the validation of sector and block
        Serial.println(F("Wrong sector or block number."));
        return;
    }

    if (_sector == 0 && _block == 0) {
        // first block is read-only
        Serial.println(F("First block is read-only."));
        return;
    }

    byte blockNum = _sector * 4 + _block;  // get the real block number（0~63）
    byte trailerBlock = _sector * 4 + 3;   // get the trailer block

    // key validation
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &KEY, &(mfrc522.uid));
    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // write the data to the block
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockNum, _blockData, 16);
    // if fail
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    Serial.println(F("Data was written."));
}
