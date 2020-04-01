#include <Arduino_FreeRTOS.h>

#define StandBy 0
#define Play 1


int score = 0;
int randpin; //random RGB pin.
int rand_interval; // 1-3 sec.
int rand_scale;
int Light=0; //photoresistor at A2.
int xPin = A0, yPin = A1;
int xAxis = 0, yAxis = 0; 
const int rpin = 9, gpin = 10, bpin = 11; //for RGB LED light.
int state = 0;//initial state: StandBy.
int start_time = 0;
int end_time = 0;
int correct = 0; //default: incorrect to avoid getting score if the player is not responding.
int button_int = 0;     // INT0 is on pin 2
int button = 2; //SW at joystick.

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters); //for Game Control.

void setup() {
  // Now set up two tasks to run independently
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect.
      }
  xTaskCreate( //Task1 : Photoresistor.
    vTask1,   // Pointer to function for thse task
    (const portCHAR *) "Task1", // Name for the task
    128,      // Stack size
    NULL,     // NULL task parameter
    2,        // This task will run at First priority 1
    NULL );   // Do not use the task handle
  xTaskCreate(vTask2, (const portCHAR *) "Task2", 128, NULL, 1, NULL);//Task2 : joystick.
  xTaskCreate(vTask3, (const portCHAR *) "Task3", 128, NULL, 3, NULL);//Task3 : control Game flow.
  //for btn interrupt.
    pinMode(button, INPUT_PULLUP); //return LOW when down
    attachInterrupt(button_int, handle_click, FALLING);
  
  /* Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started */
}
void loop() {  
   // Empty! Things are done in Tasks.
}

//interrupt with btn debounce.
void handle_click() { // button debouncing, press btn -> RGB LED flashes in 2 Hz driven by another timer interrupt
  static unsigned long last_int_time = 0;
  unsigned long int_time = millis(); // Read the clock
//  int  isPress = digitalRead(button);//有沒有壓btn?
  static int click_time = 0;
    if (int_time - last_int_time > 200 ) {  
    // Ignore when < 200 msec
    click_time++;
  }
    if(click_time == 1){
    state = Play;
    Serial.println("Now Play mode");//Flash at 2Hz
    }
    else{
      state = StandBy;
      Serial.println("Stand By...");
      click_time = 0;//reset.      
    }
  last_int_time = int_time;
}


void vTask1(void *pvParameters) { //measure the light intensity of a photoresistor every 5 ticks (using vTaskDelay()) and store the value in a global variable Light.
  (void) pvParameters;
  pinMode(A2, INPUT);
  for( ;; ) // A Task shall never return or exit.
   {
      Light = analogRead(A2);
//      Serial.println("1");
      vTaskDelay(5);
   }
}
void vTask2(void *pvParameters) {//position of a joystick every 5 ticks and store the values in global variables xAxis and yAxis respectively. 
  (void) pvParameters;
//  pinMode(A0, INPUT);
//  pinMode(A1, INPUT);
  for( ;; ) // A Task shall never return or exit.
   {
      xAxis = analogRead(A0);
      yAxis = analogRead(A1);
//      Serial.println("2");
      vTaskDelay(5);
   }
}
/*read values in Light, xAxis, and yAxis every 20 ticks to set the color of a RGB LED: 
Green = 125
Red = (Light / 512) * (xAxis/4)
Blue = (Light / 512) * (yAxis/4)
*/
void vTask3(void *pvParameters) { 
  (void) pvParameters;
  pinMode(rpin, OUTPUT);
  pinMode(gpin, OUTPUT);
  pinMode(bpin, OUTPUT);
  for( ;; ) // A Task shall never return or exit.
   {
    switch(state){
      case StandBy:
          analogWrite(gpin, 100);
          analogWrite(rpin, 100);
          analogWrite(bpin, 100);
      break;
      case Play:
          analogWrite(gpin, 0);
          analogWrite(rpin, 0);
          analogWrite(bpin, 0);
          randpin = random(9,12);
          rand_scale = random(128,256);
          analogWrite(randpin, (Light/512)*rand_scale );
          rand_interval = random(1,4);
          start_time = millis();
          xAxis = analogRead(A0);
          yAxis = analogRead(A1);
          vTaskDelay(rand_interval*50);
          analogWrite(gpin, 0);
          analogWrite(rpin, 0);
          analogWrite(bpin, 0);
          //Detect user's motion.
//          start_time = millis();
          xAxis = analogRead(A0);
          yAxis = analogRead(A1);
            switch(randpin){
              case rpin: //Red  move to left(value below than 400 in left direction)
                if(xAxis <= 400) correct= 1;
                else correct = 0;
              break; 
              case gpin://Green move to far right(value upper than 700 in right direction)
                if(xAxis >=700) correct= 1;
                else correct = 0;               
              break;
              case bpin:// Blue  move to full up(value upper than 700 in up direction)
                if(yAxis <=300) correct= 1;
                else correct = 0;  
              break;
            }
          end_time = millis();
          if(end_time - start_time < 5000){ //within one second.
              if(correct)score++;
              else score--;
              Serial.println(score);
          }
//          vTaskDelay(50);
      break;
    }
    vTaskDelay(20);
    // do whatever the task is to do 
   }
}
