#include <Arduino_FreeRTOS.h>

int Light=0; //photoresistor at A2.
int xPin = A0, yPin = A1;
int xAxis = 0, yAxis = 0; 
int rpin = 9, gpin =10, bpin = 11; //for RGB LED light.

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);

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
    3,        // This task will run at First priority 1
    NULL );   // Do not use the task handle
  xTaskCreate(vTask2, (const portCHAR *) "Task2", 128, NULL, 2, NULL);//Task2 : joystick.
  xTaskCreate(vTask3, (const portCHAR *) "Task3", 128, NULL, 1, NULL);//Task3 : set RGB LED.
  
  /* Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started */
}
void loop() {  
   // Empty! Things are done in Tasks.
}

void vTask1(void *pvParameters) { //measure the light intensity of a photoresistor every 5 ticks (using vTaskDelay()) and store the value in a global variable Light.
  (void) pvParameters;
  pinMode(A2, INPUT);
  for( ;; ) // A Task shall never return or exit.
   {
    Light = analogRead(A2);
                Serial.println("1");
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
            Serial.println("2");
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
    analogWrite(gpin, 125);
    analogWrite(rpin, (Light/512)*(xAxis/4));
    analogWrite(bpin, (Light/512)*(yAxis/4));
    vTaskDelay(20);
    // do whatever the task is to do 
   }
}
