#include<Arduino_FreeRTOS.h>
#include<Key.h>
#include <Wire.h>
#include<Keypad.h>
#include<LiquidCrystal_I2C.h>
#define KEY_ROWS 4
#define KEY_COLS 4
#include <Queue.h>
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


QueueHandle_t Miner_control_Queue_Handle = 0; //Global Handler
TaskHandle_t Map;

LiquidCrystal_I2C lcd(0x3F,16,2);
byte minesweeper[8] = {B01110,B01110,B10100,B11111,B00101,B01110,B01010,B11011};
byte unchecked[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte exploded[8] = {B00100,B10101,B01110,B11111,B11111,B01110,B10101,B00100};
byte succeeded[8] = {B00100,B00110,B00111,B00110,B00100,B00100,B01110,B11111};
void setup() {
    // Now set up three tasks to run independently
  Serial.begin(9600);
//    while (!Serial) {
//    ; // wait for serial port to connect.
//      }
        lcd.init();                lcd.backlight();
      lcd.createChar(0,minesweeper);
      lcd.createChar(1,unchecked);
      lcd.createChar(2,exploded);
      lcd.createChar(3,succeeded);
//  // Create a queue of 5 int
  Miner_control_Queue_Handle = xQueueCreate(5, sizeof(int));
  xTaskCreate( //Task1 : Login.
    controlTask,   // Pointer to function for thse task
    (const portCHAR *) "Task1", // Name for the task
    128,      // Stack size
    NULL,     // NULL task parameter
    1,        // This task will run at First priority 1
    NULL );   // Do not use the task handle
  xTaskCreate(DisplayTask, (const portCHAR *) "Task2", 128, NULL, 1, NULL);//Task2 : Display.
  xTaskCreate(mapTask, (const portCHAR *) "Task3", 128, NULL, 1, NULL);
   lcd.begin(16, 2);   // initialize LCD //size of LCD.
   vTaskStartScheduler();
}
void loop() {
  // put your main code here, to run repeatedly:
}

void mapTask(void *pvParameters) { //seven segment.
  (void) pvParameters;
  Serial.println("mapTask!");
  for( ;; ) // A Task shall never return or exit.
   {
        Serial.println("mapTask!");
        vTaskDelay(5);
   }     
}


void controlTask(void *pvParameters) { 
  (void) pvParameters;
  int last_x = 0;
  int last_y = 0;
  int current_x=0;
  int current_y=0;
  int change_position=0;
  int remover_num =10;
  for( ;; ) // A Task shall never return or exit.
   {
        Serial.println("sent value:"); 
        Serial.print(current_x);
        Serial.print(" ");
        Serial.println(current_y);
        last_x = current_x;
        last_y = current_y;
        if(!xQueueSend(Miner_control_Queue_Handle, &last_x, 1000))
          Serial.println("Failed to send to queue");  
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec  
        if(!xQueueSend(Miner_control_Queue_Handle, &last_y, 1000))
          Serial.println("Failed to send to queue");            
 
    //For direction control.
        key = myKeypad.getKey(); 
        if(key == '2'){//UP
          if(current_y ==0){
           
          }
          else{
            current_y -=1;
            change_position = 1;
          }
        }
        else if(key == '8'){
          if(current_y ==1){
           
          }
          else{
            current_y +=1;
            change_position = 1;
          } 
        }
        else if(key == '4'){
          if(current_x ==0){
           
          }
          else{
            current_x -=1;
            change_position = 1;
          }           
        }
        else if(key == '6'){ //Right.
          if(current_x ==15){
           
          }
          else{
            current_x +=1;
            change_position = 1;
          }              
        }
       //mine remover.      
        else if(key == '5'){ 
          if(remover_num >0){
              remover_num--;
          }
          else{
            //Game lose should start a new game.
            Serial.println("No remover left.");
//            vTaskPrioritySet(Map, 2);
          }              
        }        
        if(!xQueueSend(Miner_control_Queue_Handle, &current_x, 1000))
          Serial.println("Failed to send to queue");  
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec  
        if(!xQueueSend(Miner_control_Queue_Handle, &current_y, 1000))
          Serial.println("Failed to send to queue");    

        if(!xQueueSend(Miner_control_Queue_Handle, &change_position, 1000))
          Serial.println("Failed to send to queue");    
         
        vTaskDelay(5);
   }     
}

void DisplayTask(void *pvParameters) { //seven segment.
  (void) pvParameters;
  int col, row;
  int last_x,last_y;
  int change_position=0;
         for(int i=0; i<=1; i++){
          for(int j=0; j<=15; j++){
             if(j==col && i==row){
             }
             else{
               lcd.setCursor(j,i);
               lcd.write(1); //draw uncheck.
             }
          }
        }
  for( ;; ) // A Task shall never return or exit.
   {
        if(xQueueReceive(Miner_control_Queue_Handle,&last_x,1000)){
          Serial.println("receive value:");
          Serial.println(last_x);   }
        else 
         Serial.println("Failed to receive from queue");  

        if(xQueueReceive(Miner_control_Queue_Handle,&last_y,1000)){
          Serial.println("receive value:");
          Serial.println(last_y);   }
        else 
         Serial.println("Failed to receive from queue");    
        if(xQueueReceive(Miner_control_Queue_Handle,&col,1000)){
          Serial.println("receive value:");
          Serial.println(col);   }
        else 
         Serial.println("Failed to receive from queue");  

        if(xQueueReceive(Miner_control_Queue_Handle,&row,1000)){
          Serial.println("receive value:");
          Serial.println(row);   }
        else 
         Serial.println("Failed to receive from queue");

        if(xQueueReceive(Miner_control_Queue_Handle,&change_position,1000)){
          Serial.println("Change position?:");
          Serial.println(change_position);   }
        else 
         Serial.println("Failed to receive from queue");      
          
        if(change_position){
          lcd.setCursor(last_x,last_y);
          lcd.write(byte(1));
          lcd.setCursor(col,row);  // setting cursor
          lcd.write(byte(0));
          change_position = 0;
        }
        else{
          lcd.setCursor(col,row);  // setting cursor
          lcd.write(byte(0));    
        }
        vTaskDelay(5);
   }     
}
