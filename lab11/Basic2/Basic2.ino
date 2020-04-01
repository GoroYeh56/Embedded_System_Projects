#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <queue.h>
#include <math.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#define KEY_ROWS 4
#define KEY_COLS 4

byte minesweeper[8] = {B01110, B01110, B10100, B11111, B00101, B01110, B01010, B11011};
byte unchecked[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
byte exploded[8] = {B00100, B10101, B01110, B11111, B11111, B01110, B10101, B00100};
byte succeeded[8] = {B00100, B00110, B00111, B00110, B00100, B00100, B01110, B11111};

LiquidCrystal_I2C lcd(0x3F, 16, 2);
QueueHandle_t Global_Queue_Handle = 0;

TaskHandle_t TaskHandle_Map;

void controlTask(void *pvParameters);
void displayTask(void *pvParameters);
void mapTask(void *pvParameters);


int mineRemoverCnt = 10;


char inputStr[9];
int passLength = 4;
int firstInput = 0;
int correct = 0;
int wrong = 0;
int count = 0;
int finish = 0;
int i, j = 0, k;
int printMode = 0;
int globalMode = 0;
int printFinish = 0;
char keymap[KEY_ROWS][KEY_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Column pin 1~4
byte colPins[KEY_COLS] = {9, 8, 7, 6};
// Column pin 1~4
byte rowPins[KEY_ROWS] = {13, 12, 11, 10};
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);
char key;

void setup() {
  Serial.begin(9600);
  Global_Queue_Handle = xQueueCreate(5, sizeof(int));

  lcd.init();    // initialize LCD
  lcd.backlight();    // open LCD backlight
  lcd.createChar(0, minesweeper);
  lcd.createChar(1, unchecked);
  lcd.createChar(2, exploded);
  lcd.createChar(3, succeeded);
  lcd.setCursor(0, 0);
  lcd.write(0);

  

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 16; j++) {
      if (i == 0 && j == 0) {}
      else {
        lcd.setCursor(j, i);
        lcd.write(1);
      }
    }
  }

  xTaskCreate(controlTask, (const portCHAR *) "task_control", 128, NULL, 1, NULL);
  xTaskCreate(displayTask, (const portCHAR *) "task_display", 128, NULL, 1, NULL);
  xTaskCreate(mapTask, (const portCHAR *) "task_map", 128, NULL, 1, &TaskHandle_Map);

  vTaskStartScheduler();
}

void loop() {}

void controlTask(void *pvParameters) {
  (void) pvParameters;
  int dir_send_row = 0; // 0 1
  int dir_send_col = 0; // 0~15
  int dir_send_mode = 0; // 0:nothing 1:3steps 2:mine
  int dir_send_state = 0; 
  int mine_i = 0;
  int mine_j = 0;
  int restart = 0;
  int last_state = 0;
  
  const TickType_t xDelay = 50;
  
  if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){
    Serial.print("mine_i");
    Serial.println(mine_i);
    Serial.print("mine_j");
    Serial.println(mine_j);
  }

  for (;;) {
    key = myKeypad.getKey();
    if (key == '2') { // up
      if (dir_send_row == 1) { // 2nd row
        dir_send_row -= 1;
        if (dir_send_row != mine_i || dir_send_col != mine_j) {
          if(last_state == 1) {
            dir_send_mode = 2;
            dir_send_state = 4;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_state, 1000);
            dir_send_mode = 0;    
            dir_send_row = 0;
            dir_send_col = 0;
            dir_send_state = 0;
            last_state = 0;
            mineRemoverCnt = 10;
            
            vTaskDelay( xDelay );
            if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){

            }      
          }
          else {
            dir_send_mode = 3;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
          }
          
        }
        else {
          dir_send_mode = 3;
          last_state = 1;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);       
        }

      }
    }
    else if (key == '4') { // left
      if (dir_send_col > 0) {
        dir_send_col -= 1;
        if (dir_send_row != mine_i || dir_send_col != mine_j) {
          if(last_state == 1) {
            dir_send_mode = 2;
            dir_send_state = 3;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_state, 1000);
            dir_send_mode = 0;    
            dir_send_row = 0;
            dir_send_col = 0;
            dir_send_state = 0;
            last_state = 0;
            mineRemoverCnt = 10;
            
            vTaskDelay( xDelay );
            if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){

            }      
          }
          else {
            dir_send_mode = 3;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
          }
        }
        else {
          dir_send_mode = 3;
          last_state = 1;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);            
        }

      }
    }
    else if (key == '6') { // right
      if (dir_send_col < 15) {
        dir_send_col += 1;
        if (dir_send_row != mine_i || dir_send_col != mine_j) {
          if(last_state == 1) {
            dir_send_mode = 2;
            dir_send_state = 1;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_state, 1000);
            dir_send_mode = 0;    
            dir_send_row = 0;
            dir_send_col = 0;
            dir_send_state = 0;
            last_state = 0;
            mineRemoverCnt = 10;
            
            vTaskDelay( xDelay );
            if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){

            }      
          }
          else {
            dir_send_mode = 3;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
          }
        }
        else {
          dir_send_mode = 3;
          last_state = 1;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);     
        }

      }
    }
    else if (key == '8') {
      if (dir_send_row == 0) {
        dir_send_row += 1;
        if (dir_send_row != mine_i || dir_send_col != mine_j) {
          if(last_state == 1) {
            dir_send_mode = 2;
            dir_send_state = 2;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_state, 1000);
            dir_send_mode = 0;    
            dir_send_row = 0;
            dir_send_col = 0;
            dir_send_state = 0;
            last_state = 0;
            mineRemoverCnt = 10;
            
            vTaskDelay( xDelay );
            if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){
              Serial.print("mine_i");
              Serial.println(mine_i);
              Serial.print("mine_j");
              Serial.println(mine_j);
            }      
          }
          else {
            dir_send_mode = 3;
            xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
            xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
          }
        }
        else {
          dir_send_mode = 3;
          last_state = 1;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);    
        }   
      }
    }
    else if (key == '5') {
      mineRemoverCnt--;
      
      Serial.print("cnter:");
      Serial.println(mineRemoverCnt);
      
      if (dir_send_row == mine_i && dir_send_col == mine_j) {
        dir_send_mode = 2;
        dir_send_state = 0;
        xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
        xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
        xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
        xQueueSend(Global_Queue_Handle, &dir_send_state, 1000);
        dir_send_mode = 0;    
        dir_send_row = 0;
        dir_send_col = 0;
        dir_send_state = 0;
        last_state = 0;
        mineRemoverCnt = 10;
        
        vTaskDelay( xDelay );
        if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){

        }
      }
      else if ((abs(dir_send_row - mine_i) + abs(dir_send_col - mine_j) <= 3)) {
        if(mineRemoverCnt>0) {
          dir_send_mode = 1;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
        }
        else {
          dir_send_mode = 2;
          dir_send_state = 5;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_state, 1000);
          dir_send_mode = 0;    
          dir_send_row = 0;
          dir_send_col = 0;
          dir_send_state = 0;
          last_state = 0;
          mineRemoverCnt = 10;
          
          vTaskDelay( xDelay );
          if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){

          }      
        }
        
      }
      else {
        if(mineRemoverCnt>0) {
          dir_send_mode = 0;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);         
        }
        else {
          dir_send_mode = 2;
          dir_send_state = 5;
          xQueueSend(Global_Queue_Handle, &dir_send_row, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_col, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_mode, 1000);
          xQueueSend(Global_Queue_Handle, &dir_send_state, 1000);
          dir_send_mode = 0;    
          dir_send_row = 0;
          dir_send_col = 0;
          dir_send_state = 0;
          last_state = 0;
          mineRemoverCnt = 10;
          
          vTaskDelay( xDelay );
          if(xQueueReceive(Global_Queue_Handle, &mine_i, 1000) && xQueueReceive(Global_Queue_Handle, &mine_j, 1000)){

          }      
        }
      }
    }
  }
}

void displayTask(void *pvParameters) {
  (void) pvParameters;
  const TickType_t xDelay = 50;
  int dir_recv_row = 0; //  0~15 16~31
  int dir_recv_col = 0;
  int isZero[7];
  int isOne[7];
  int dir_recv_mode = 0;
  int dir_recv_state = 0;
  int zeroTmp = 0;
  int oneTmp = 0;
  int flag = 0;
  int flag2 = 0;
  int restart = 0;
  int settingMine = 0;


  for (i = 0; i < 7; i++) {
    isZero[i] = -1;
    isOne[i] = -1;
  }

  for (;;) {
    if (settingMine == 0) {
      if (xQueueReceive(Global_Queue_Handle, &dir_recv_row, 1000) && xQueueReceive(Global_Queue_Handle, &dir_recv_col, 1000) && xQueueReceive(Global_Queue_Handle, &dir_recv_mode, 1000)) {
        lcd.clear();
        /*
        Serial.print("row:");
        Serial.print(dir_recv_row);
        Serial.print("   col:");
        Serial.print(dir_recv_col);
        Serial.print("   mode:");
        Serial.println(dir_recv_mode);
        */
        
        if (dir_recv_mode == 0) { // step 0
          isZero[zeroTmp] = dir_recv_row * 16 + dir_recv_col;
          zeroTmp++;
        }
        
        else if (dir_recv_mode == 1) { // step 1
          isOne[oneTmp] = dir_recv_row * 16 + dir_recv_col;
          oneTmp++;
        }
        
        else if (dir_recv_mode == 2) { // succeed
          xQueueReceive(Global_Queue_Handle, &dir_recv_state, 1000);
          restart = 1;
          lcd.clear();
          if(dir_recv_state == 0) {
            lcd.setCursor(dir_recv_col, dir_recv_row);
            lcd.write(3);
          }
          else if(dir_recv_state == 1) {
            lcd.setCursor(dir_recv_col-1, dir_recv_row);
            lcd.write(2);
          }
          else if(dir_recv_state == 2) {
            lcd.setCursor(dir_recv_col, dir_recv_row-1);
            lcd.write(2);
          }
          else if(dir_recv_state == 3) {
            lcd.setCursor(dir_recv_col+1, dir_recv_row);
            lcd.write(2);
          }
          else if(dir_recv_state == 4) {
            lcd.setCursor(dir_recv_col, dir_recv_row+1);
            lcd.write(2);
          }
          else if(dir_recv_state == 5) {
            
          }
          vTaskDelay( xDelay );
          
          for (i = 0; i < 7; i++) {
            isZero[i] = -1;
            isOne[i] = -1;
          }
          flag = 0;
          flag2 = 0;
          zeroTmp = 0;
          oneTmp = 0;
          dir_recv_state = 0;
          dir_recv_mode = 0;
          dir_recv_row = 0;
          dir_recv_col = 0;
          restart = 0;
          settingMine = 1;
          vTaskResume(TaskHandle_Map);
        }
        
        

        if(restart==0) {
          for (i = 0; i < 2; i++) {
            for (j = 0; j < 16; j++) {
              lcd.setCursor(j, i);
              flag = 0;
              for (k = 0; k < 6; k++) {
                if (isZero[k] == i * 16 + j) {
                  flag = 1;
                  if (i == dir_recv_row && j == dir_recv_col && dir_recv_mode == 0) {
                    flag2 = 1;
                    lcd.print("0");
                  }
                  else if(i == dir_recv_row && j == dir_recv_col) {
                    lcd.write(0);
                  }
                  else {
                    lcd.print("0");
                  }
    
                }
                else if (isOne[k] == i * 16 + j) {
                  flag = 1;
                  if (i == dir_recv_row && j == dir_recv_col && dir_recv_mode == 1) {
                    flag2 = 1;
                    lcd.print("1");
                  }
                  else if(i == dir_recv_row && j == dir_recv_col) {
                    lcd.write(0);
                  }
                  else {
                    lcd.print("1");
                  }
    
                }
              }
              if (i == dir_recv_row && j == dir_recv_col && flag == 0) {
                lcd.write(0);
              }
              else if (flag == 0) {
                lcd.write(1);
              }
            }
          }
          if(flag2==1) {
            vTaskDelay( xDelay );
            lcd.setCursor(dir_recv_col, dir_recv_row);
            lcd.write(0);
            flag2 = 0;
          }
        }
        settingMine = 0;
      }  
    }
  }
}

void mapTask(void *pvParameters) {
  (void) pvParameters;
  int mine_i;
  int mine_j;
  for (;;) {
    randomSeed(analogRead(0));
    mine_i = random(0, 2);
    mine_j = random(0, 16);

    
    Serial.print("mine_i");
    Serial.println(mine_i);
    Serial.print("mine_j");
    Serial.println(mine_j);
    

    xQueueSend(Global_Queue_Handle, &mine_i, 1000);
    xQueueSend(Global_Queue_Handle, &mine_j, 1000);
    
    vTaskSuspend(TaskHandle_Map);
  }
}
