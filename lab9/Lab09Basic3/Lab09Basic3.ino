#include <Arduino_FreeRTOS.h>
#define STEPS 2048
#include <Stepper.h>

Stepper stepper(STEPS, 8, 10, 9, 11);
const int trigPin = 12, echoPin = 13;
int Light[2]= {0,0}; //photoresistor at A2,A3. 
int isPark0 = 0;
int isPark1 = 0;
int space = 2;
int distance;
int duration;

int xPin = A0, yPin = A1;
int xAxis = 0, yAxis = 0; 
int rpin = 9, gpin =10, bpin = 11; //for RGB LED light.
int motor_state=3;
int pins[8] = {2, 3, 4, 5, 6, 7, 14, 15};//pins to 7-seg. //14 15 : A0 A1
boolean data[3][8] = { 
// define the pins to light the 3 number: 0, 1, 2
  {true, true, true, true, true, true, false, false}, // 0
  {false, true, true, false, false, false, false, false}, // 1
  {true, true, false,true, true, false, true, false} // 2
};  

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void Ultrasonic(void *pvParameters);
void StepperMotor(void *pvParameters);
void setup() {
  // Now set up two tasks to run independently
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect.
      }
  xTaskCreate( //Task1 : Photoresistor Light[1].
    vTask1,   // Pointer to function for thse task
    (const portCHAR *) "Task1", // Name for the task
    128,      // Stack size
    NULL,     // NULL task parameter
    1,        // This task will run at First priority 1
    NULL );   // Do not use the task handle
  xTaskCreate(vTask2, (const portCHAR *) "Task2", 128, NULL, 1, NULL);//Task2 : Light[1]
  xTaskCreate(vTask3, (const portCHAR *) "Task3", 128, NULL, 1, NULL);//Task3 : set sevenSegment.
  xTaskCreate(Ultrasonic, (const portCHAR *) "Task4", 128, NULL, 1, NULL);//Detect 15 cm.
  xTaskCreate(StepperMotor, (const portCHAR *) "Task5", 128, NULL, 1, NULL);//stepper motor.
  
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
    Light[0] = analogRead(A2);
    if(Light[0] < 700)isPark0 = 1;
    else isPark0 = 0;
//     Serial.print(Light[0]);
//     Serial.print(" ");
    vTaskDelay(5);
   }
}
void vTask2(void *pvParameters) {//position of a joystick every 5 ticks and store the values in global variables xAxis and yAxis respectively. 
  (void) pvParameters;
pinMode(A3, INPUT);
  for( ;; ) // A Task shall never return or exit.
   {
     Light[1] = analogRead(A3);
     if(Light[1] < 700)isPark1 = 1;
    else isPark1 = 0;
//     Serial.println(Light[1]);
      vTaskDelay(5);
   }
}

void vTask3(void *pvParameters) { //seven segment.
  (void) pvParameters;
  
  for( ;; ) // A Task shall never return or exit.
   {
        space = 2;
        if(isPark0 == 1)space--;
        if(isPark1 == 1)space--;
//          Serial.println(space);
        for(int i=0; i<=7; i++){
          digitalWrite(pins[i], data[space][i] == true ? HIGH : LOW);
        }     
    vTaskDelay(5);
    // do whatever the task is to do 
   }
}

void Ultrasonic(void *pvParameters) { //detect whether there is a car.
  (void) pvParameters;
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
  for( ;; ) // A Task shall never return or exit.
   {
         digitalWrite(trigPin, LOW); // Clears the trigPin
         delayMicroseconds(2);
         /* Sets the trigPin on HIGH state for 10 ms */
         digitalWrite(trigPin, HIGH);    delayMicroseconds(10);
         digitalWrite(trigPin, LOW);
         /* Reads Echo pin, returns sound travel time in ms */
         duration = pulseIn(echoPin, HIGH);
         /* Calculating the distance */
         distance = duration*0.034/2; 
//            Serial.print(" Distance: ");
//   Serial.println(distance);
     }     
    vTaskDelay(5);
    // do whatever the task is to do 
 }


void StepperMotor(void *pvParameters) { //Stepper.steps. if distance <= 15cm, turn.
  (void) pvParameters;
//  int motor_state;
  int lastState = 0;
  stepper.setSpeed(10);
  for( ;; ) // A Task shall never return or exit.
   {    
        //initial state : 4 (up). Positive: turn Right. Negative: turn Left.
//        motor_state = 4;          
        if(distance <= 15){
             Serial.print("Space: ");
             Serial.print(space);
             Serial.print(" Distance: ");
             Serial.print(distance);
              if(space ==2){ //point Left :1
                  motor_state = 1;
                   Serial.print(" Should turn Left: ");
                  Serial.println(motor_state);
              }
              else if(space == 1){//point to right:6.
                  if(isPark0 == 1){
                    motor_state = 5;
                    Serial.print(" Should turn Right: ");
                    Serial.println(motor_state);
                  }
                  else{
                    motor_state = 1;
                    Serial.print(" Should turn Left: ");
                    Serial.println(motor_state);                    
                    
                  }
              }
              else{//no empty space. point to eight.
                motor_state = 7;
                  Serial.print(" Should turn Other: ");
                  Serial.println(motor_state);
              }
              if(motor_state != lastState){
              stepper.step(256 * (motor_state - lastState));
              }
        }
        else{//the car is away.
            stepper.step(256*(3-motor_state)); //should turn back to initial state.
            motor_state = 3;//reset motor state.
             Serial.print(" Should turn Back to Initial: ");
              Serial.println(motor_state);
              
        }
        lastState = motor_state;
    vTaskDelay(5);
    // do whatever the task is to do 
   }
}
