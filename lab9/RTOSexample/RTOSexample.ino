#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskRGB( void *pvParameters );
void Joystick( void *pvParameters );
//void Photoresistor( void *pvParameters );
int xAxis, yAxis;
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskRGB
    ,  (const portCHAR *)"Blink"   // A name just for humans
    ,  128  // Stack size
    ,  NULL
    ,  2  // priority
    ,  NULL );

  xTaskCreate(
    Joystick
    ,  (const portCHAR *) "AnalogRead"
    ,  128 // This stack size can be checked & adjusted by reading Highwater
    ,  NULL
    ,  1  // priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskRGB(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital pin 13 as an output.
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
        analogWrite(5, 125);
    analogWrite(3,(xAxis/4));
    analogWrite(6, (xAxis/4)/*(Light/512)*(yAxis/4)*/);
    vTaskDelay(1);
//   analogWrite(5, 0);
//    analogWrite(3,0);
//    analogWrite(6, 0);
//        vTaskDelay(200);
//    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
//    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
//    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
//    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void Joystick(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize serial communication at 9600 bits per second:
//  Serial.begin(9600);

  for (;;)
  {
    // read the input on analog pin 0:
    xAxis = analogRead(A0);
//    yAxis = analogRead(A1);
    // print out the value you read:
    Serial.println(xAxis);
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
