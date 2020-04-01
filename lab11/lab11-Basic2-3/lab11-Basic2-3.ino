#include<Arduino_FreeRTOS.h>
#include<Key.h>
#include <Wire.h>
#include<Keypad.h>
#include<LiquidCrystal_I2C.h>
#define KEY_ROWS 4
#define KEY_COLS 4
#include <queue.h>
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


QueueHandle_t Global_Queue_Handle = 0; //Global Handler
QueueHandle_t Map_Queue_Handle = 0; //Global Handler
TaskHandle_t Map;
LiquidCrystal_I2C lcd(0x3F,16,2);

void setup() {
    // Now set up three tasks to run independently
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect.
      }
        lcd.init();                lcd.backlight();
//  lcd.createChar(0, err_dragon);
  
  // Create a queue of 3 int
  Global_Queue_Handle = xQueueCreate(3, sizeof(int));
  Map_Queue_Handle = xQueueCreate(2,sizeof(int));
  xTaskCreate( //Task1 : Login.
    controlTask,   // Pointer to function for thse task
    (const portCHAR *) "Task1", // Name for the task
    100,      // Stack size
    NULL,     // NULL task parameter
    1,        // This task will run at First priority 1
    NULL );   // Do not use the task handle
  xTaskCreate(mapTask, (const portCHAR *) "Task3", 42, NULL, 1, &Map);  
  xTaskCreate(DisplayTask, (const portCHAR *) "Task2", 144, NULL, 1, NULL);//Task3 : Display.  
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight

   vTaskStartScheduler();
}

void loop() {
  // put your main code here, to run repeatedly:
}
//int change_position = 0;

void controlTask(void *pvParameters) { //seven segment.
  (void) pvParameters;
  int current_x=0;
  int current_y=0;
  int change_position=0;
  for( ;; ) // A Task shall never return or exit.
   {
//        Serial.println("sent value:"); 
//        Serial.print(current_x);
//        Serial.print(" ");
//        Serial.println(current_y);
        if(!xQueueSend(Global_Queue_Handle, &current_x, 1000))
          Serial.println("Failed to send current_x to queue");  
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec  
        if(!xQueueSend(Global_Queue_Handle, &current_y, 1000))
          Serial.println("Failed to send current_Y to queue");    
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec 
        if(!xQueueSend(Global_Queue_Handle, &change_position, 1000))
          Serial.println("Failed to send to queue");    
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec        
    
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
//        lcd.setCursor(current_x, current_y);  // setting cursor
        vTaskDelay(5);
   }     
}
void mapTask(void *pvParameters) { //Generate mine position.
  (void) pvParameters;
  int mineX, mineY;
  for( ;; ) // A Task shall never return or exit.
   { 
        mineX = random(0,16);
        mineY = random(0,2);
        Serial.print(mineX);
        Serial.print(", ");
        Serial.println(mineY);
//        if(!xQueueSend(Global_Queue_Handle, &mineX, 1000))
//          Serial.println("Failed to send mineX to queue");    
//        if(!xQueueSend(Global_Queue_Handle, &mineY, 1000))
//          Serial.println("Failed to send mineY to queue");         
        vTaskDelay(5);
        vTaskSuspend(Map);
   }     
}


byte minesweeper[8] = {B01110,B01110,B10100,B11111,B00101,B01110,B01010,B11011};
byte unchecked[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte exploded[8] = {B00100,B10101,B01110,B11111,B11111,B01110,B10101,B00100};
byte succeeded[8] = {B00100,B00110,B00111,B00110,B00100,B00100,B01110,B11111};
void DisplayTask(void *pvParameters) { //seven segment.
  (void) pvParameters;
  int col, row;
  int last_x, last_y;
  int change_position=0;
  int i,j;
  int mineX,mineY;
  int Getmine_place = 0;
  lcd.createChar(0, minesweeper);
  lcd.createChar(1, unchecked);
  lcd.createChar(2, exploded);
  lcd.createChar(3, succeeded);  
        for(i=0; i<=1; i++){
          for(j=0; j<=15; j++){
              lcd.setCursor(j,i);
              lcd.write(1);
          }
        }  
  for( ;; ) // A Task shall never return or exit.
   {

//        //Get mine positoin.
//        if(!Getmine_place){
//        if(xQueueReceive(Global_Queue_Handle,&mineX,1000)){
//          Serial.print("Get mineX ");
//         }
//        else 
//         Serial.println("Failed to receive mineX from queue");  
//
//        if(xQueueReceive(Global_Queue_Handle,&mineY,1000)){
//          Serial.print("Get mineY ");
//        }
//        else 
//         Serial.println("Failed to receive mineY from queue");  
//          Getmine_place = 1;
//        } 
//    
        if(xQueueReceive(Global_Queue_Handle,&col,1000)){
          Serial.println("receive value:");
          Serial.println(col);   }
        else 
         Serial.println("Failed to receive from queue");  

        if(xQueueReceive(Global_Queue_Handle,&row,1000)){
          Serial.println("receive value:");
          Serial.println(row);   }
        else 
         Serial.println("Failed to receive from queue");

        if(xQueueReceive(Global_Queue_Handle,&change_position,1000)){
          Serial.println("Change position?:");
          Serial.println(change_position);   }
        else 
         Serial.println("Failed to receive from queue");      
                
        if(change_position){
//          lcd.clear();
        
          lcd.setCursor(last_x, last_y);
//          lcd.setCursor(col,row);  // setting cursor
          lcd.write(1);
          change_position = 0;
        }
          lcd.setCursor(col,row);  // setting cursor
          lcd.write(0);    
        last_x = col;
        last_y =row;
        vTaskDelay(5);
   }     
}
