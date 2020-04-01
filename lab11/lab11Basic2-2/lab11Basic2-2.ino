#include<Arduino_FreeRTOS.h>
#include<Key.h>
#include <Wire.h>
#include<Keypad.h>
#include<LiquidCrystal_I2C.h>
#define KEY_ROWS 4
#define KEY_COLS 4
#include <Queue.h>
#include <math.h>
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
QueueHandle_t Succeed_Queue_Handle = 0;
 TaskHandle_t Map;

LiquidCrystal_I2C lcd(0x3F,16,2);
byte minesweeper[8] = {B01110,B01110,B10100,B11111,B00101,B01110,B01010,B11011};
byte unchecked[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte exploded[8] = {B00100,B10101,B01110,B11111,B11111,B01110,B10101,B00100};
byte succeeded[8] = {B00100,B00110,B00111,B00110,B00100,B00100,B01110,B11111};
void setup() {
    // Now set up three tasks to run independently
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect.
      }
        lcd.init();                lcd.backlight();
  lcd.createChar(0, minesweeper);
  lcd.createChar(1, unchecked);
  lcd.createChar(2, exploded);
  lcd.createChar(3, succeeded);
  // Create a queue of 3 int
  Global_Queue_Handle = xQueueCreate(8, sizeof(int));
  Map_Queue_Handle = xQueueCreate(2, sizeof(int)); //send the mine_x and mine_y to Display.
  Succeed_Queue_Handle = xQueueCreate(1, sizeof(int));
  xTaskCreate( //Task1 : Login.
    controlTask,   // Pointer to function for thse task
    (const portCHAR *) "Task1", // Name for the task /* 100 114 64*/
    96,      // Stack size
    NULL,     // NULL task parameter
    1,        // This task will run at First priority 1
    NULL );   // Do not use the task handle
  xTaskCreate(DisplayTask, (const portCHAR *) "Task2",64, NULL, 1, NULL);//Task2 : Display.
//  xTaskCreate(mapTask, (const portCHAR *) "Task3", 114, NULL, 1, &Map);//Task3 : Create initial map.    
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight
   vTaskStartScheduler();
}
int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int mineX = 5;
int mineY = 0;
void loop() {
//  Serial.println(freeRam());
  // put your main code here, to run repeatedly:
}

void controlTask(void *pvParameters) { //seven segment.
  (void) pvParameters;
  int current_x=0;
  int current_y=0;
  int last_x = 0;
  int last_y = 0;
  int change_position=0;
  int removers = 10;
  int use_remover = 0;
  int gameover  = 0;
  int succeed = 0;
  int should_Boom = 0;
  int been_there=0;
  for( ;; ) // A Task shall never return or exit.
   {
//        Serial.println("sent value:"); 
//        Serial.print(current_x);
//        Serial.print(" ");
//        Serial.println(current_y);
        if(!xQueueSend(Global_Queue_Handle, &current_x, 1000))
          Serial.println("Failed to send current x to queue");  
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec  
        if(!xQueueSend(Global_Queue_Handle, &current_y, 1000))
          Serial.println("Failed to send current y to queue");    
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec
        if(!xQueueSend(Global_Queue_Handle, &last_x, 1000))
          Serial.println("Failed to send last x to queue");  
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec  
        if(!xQueueSend(Global_Queue_Handle, &last_y, 1000))
          Serial.println("Failed to send last y to queue");    
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec 
        if(!xQueueSend(Global_Queue_Handle, &change_position, 1000))
          Serial.println("Failed to send change_position to queue");    
//        vTaskDelay(500/portTICK_PERIOD_MS);//delay 3 sec     
        if(!xQueueSend(Global_Queue_Handle, &use_remover, 1000))
          Serial.println("Failed to send use_remover to queue");   
        if(!xQueueSend(Global_Queue_Handle, &gameover, 1000))
          Serial.println("Failed to send gameover to queue");     
        if(!xQueueSend(Global_Queue_Handle, &should_Boom, 1000))
          Serial.println("Failed to send shouldBoom to queue");     
          last_x = current_x;
          last_y = current_y;      
        gameover = 0;       
        should_Boom = 0;
        use_remover = 0; //reset.
        change_position = 0;//reset.
        key = myKeypad.getKey(); 
        if(key == '2'){//UP
          if(current_y ==0){
           
          }
          else{
            last_y = current_y;
            current_y -=1;
            change_position = 1;
          }
        }
        else if(key == '8'){
          if(current_y ==1){
           
          }
          else{
            last_y = current_y;
            current_y +=1;
            change_position = 1;
          } 
        }
        else if(key == '4'){
          if(current_x ==0){
           
          }
          else{
            last_x = current_x;
            current_x -=1;
            change_position = 1;
          }           
        }
        else if(key == '6'){ //Right.
          if(current_x ==15){
           
          }
          else{
            last_x = current_x;
            current_x +=1;
            change_position = 1;
          }              
        }    
        else if(key == '5'){ //Use a remover.
          if(removers>0){
              Serial.println(F("We send a 5 signal"));
              use_remover = 1;
              removers--;
          }
          else{
            gameover = 1;
            removers = 10; //refill.
            current_x = current_y = 0;
            Serial.println(F("Game over! No more remover."));
          }              
        }
        if(current_x == mineX && current_y== mineY){
          been_there = 1;
        }
        else{
          if(been_there){
              been_there = 0;//reset.
              gameover = 1;
              removers = 10;
              current_x = current_y = 0;
              last_x = last_y = 0;
              should_Boom = 1;
          }
        }
        
        if(xQueueReceive(Succeed_Queue_Handle,&succeed,1000)){ 
        }
        else 
         Serial.println(F("Failed to receive succeed from queue")); 
        //if succeed.
         if(succeed==1){
                removers = 10; //refill.
                current_x = current_y = 0;
                should_Boom = 0;
                last_x = last_y = 0;
         }
                     
        vTaskDelay(5);
   }     
}
void mapTask(void *pvParameters){
  (void)pvParameters;
  int mine_x;
  int mine_y;
  for(;;){
      mine_x = random(0,16);
      mine_y = random(0,2);
      Serial.print(mine_x);
      Serial.print(" ");
      Serial.println(mine_y);
     if(!xQueueSend(Map_Queue_Handle, &mine_x, 500))
          Serial.println("Failed to send to queue");  
     if(!xQueueSend(Map_Queue_Handle, &mine_y, 500))
          Serial.println("Failed to send to queue");  
//      Serial.println(F(mine_x));
//      Serial.println(F(mine_y));      
//      vTaskSuspend(Map);
      vTaskDelay(5);
  }
}
void DisplayTask(void *pvParameters) { //LCD Display.
  (void) pvParameters;
//  int mymap[2][16]; //keep the col, row of 'open position'
  int isZero[6] = {-1,-1,-1,-1,-1,-1};
  int isOne[6]= {-1,-1,-1,-1,-1,-1};
  int col, row;
  int mine_x, mine_y;
  int last_x=0;
  int last_y=0;
  int change_position=0;
  int use_remover=0;
  int gameover = 0; //if 1, start a new game.
  int succeed = 0;
  int should_Boom = 0;
  lcd.setCursor(0,0);
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
  int match = 0;
  int get_mine_pos = 0;
  for( ;; ) // A Task shall never return or exit.
   {  
        static int i_one = 0; //index for isOne.
        static int i_zero = 0;
        if(xQueueReceive(Global_Queue_Handle,&col,1000)){
//          Serial.println(F("receive value:"));
//          Serial.println(col);   
        }
        else 
         Serial.println("Failed to receive from queue");  

        if(xQueueReceive(Global_Queue_Handle,&row,1000)){
//          Serial.println(F("receive value:"));
//          Serial.println(row);   
        }
        else 
         Serial.println("Failed to receive from queue");

        if(xQueueReceive(Global_Queue_Handle,&last_x,1000)){
//          Serial.println(F("receive value:"));
//          Serial.println(col);   
        }
        else 
         Serial.println("Failed to receive from queue");  

        if(xQueueReceive(Global_Queue_Handle,&last_y,1000)){
//          Serial.println(F("receive value:"));
//          Serial.println(row);   
        }
        else 
         Serial.println("Failed to receive from queue");         

        if(xQueueReceive(Global_Queue_Handle,&change_position,1000)){
//          Serial.println("Change position?:");
//          Serial.println(change_position);   
        }
        else 
         Serial.println("Failed to receive from queue");      

        if(xQueueReceive(Global_Queue_Handle,&use_remover,1000)){
//          Serial.println(F("Use a remover here."));
//          Serial.println(use_remover);   
        }
        else 
         Serial.println("Failed to receive from queue"); 
        if(xQueueReceive(Global_Queue_Handle,&gameover,1000)){
//          Serial.println("receive value:");
//          Serial.println(col);   
        }
        else 
         Serial.println("Failed to receive from queue");   
        if(xQueueReceive(Global_Queue_Handle,&should_Boom,1000)){
//          Serial.println("receive value:");
//          Serial.println(col);   
        }
        else 
//         Serial.println("Failed to receive from queue");  
         Serial.print("Mine position: ");
//         Serial.print(mineX);
//         Serial.print(",");
//         Serial.print(mineY);         
//         Serial.print(" My place: ");
//         Serial.print(col);
//         Serial.print(",");
//         Serial.println(row);               
//         Serial.print(" Shoud Boom?: ");
//         Serial.print(should_Boom);
//         Serial.print(" Game over?: ");
//         Serial.println(gameover);
//      if(!get_mine_pos){
//       //Get mine position.
//       if(xQueueReceive(Map_Queue_Handle,&mine_x,1000)){
//          Serial.println("Get mine position");
//                      get_mine_pos = 1;
////          Serial.println(use_remover);   
//       }
//        else 
//         Serial.println("Failed to receive from- queue");    
//        if(xQueueReceive(Map_Queue_Handle,&mine_y,1000)){
////          Serial.println("Use a remover here.");
////          Serial.println(use_remover);   
//       }
//        else 
//         Serial.println("Failed to receive from queue");
//      }
      
        if(gameover == 1 || succeed == 1){
            if(should_Boom == 1){
                 lcd.setCursor(mineX,mineY);
                 Serial.println("Boom!");
                 lcd.write(2); 
                 vTaskDelay(1000/portTICK_PERIOD_MS);
            }
            succeed = 0;
            
            lcd.clear();
            for(int i=0; i<=1; i++){
                    for(int j=0; j<=15; j++){
                           lcd.setCursor(j,i);
                           lcd.write(1); //draw uncheck.
                    }
            }
            for(int k=0; k<=5; k++){
                isOne[k] = -1;
                isZero[k] = -1;
            }
            i_one = i_zero = 0;
            get_mine_pos = 0;
            vTaskResume(Map);
        }
        else{
            if(use_remover == 1){
                 //First determine.
                lcd.setCursor(col,row);
                if(col== mineX && row == mineY){
                    lcd.clear();
                    lcd.setCursor(col,row);                    
                    lcd.write(3);
                    succeed = 1;
                }
                else{
                    if(abs(mineX - col) < 3 ){ //distance smaller than 3.
                        isOne[i_one] = (row)*16 + col; //this position should be '1' next time.
                        Serial.println(isOne[i_one]);
                        i_one++;
                        lcd.print("1");
                    }
                    else{
                        isZero[i_zero++] = (row)*16 + col; //this position should be '0' next time.
                        lcd.print("0");
                    }
                }
               vTaskDelay(1000/portTICK_PERIOD_MS);
    //            //Then redraw.
//              Serial.println("Use a remover");         
            }
                      
            if(change_position){
                      lcd.setCursor(last_x,last_y);
                      for(int i=0; i<=5; i++){
                          if( ((last_y)*16 + last_x) == isOne[i]){
//                          Serial.println((last_y)*16 + last_x);  
                              match = 1;                          
                              lcd.print("1");
                          }
                          else if(((last_y)*16 + last_x) == isZero[i] ){
//                          Serial.println((last_y)*16 + last_x);                            
                              lcd.print("0");
                               match = 1;
                          }
//                          else{
//                              lcd.write(byte(1));
//                          }
//                          Serial.println((last_y)*16 + last_x);
//         Serial.print("last position: ");
//         Serial.print(last_x);
//         Serial.print(",");
//         Serial.print(last_y);         
//         Serial.print("Array: ");
//         Serial.print(isOne[i]);
//         Serial.print(",");
//         Serial.println(isZero[i]);               
                      }                     
//                      last_x = col;
//                      last_y = row;
              if(!match)lcd.write(1);
              else match = 0;
             }
            else{
              lcd.setCursor(col,row);  // setting cursor
              lcd.write(0);    
            }
        }
        if(!xQueueSend(Succeed_Queue_Handle, &succeed, 1000)) //Send to control task.
           Serial.println("Failed to send to queue"); 
        vTaskDelay(5);
   }     
}
