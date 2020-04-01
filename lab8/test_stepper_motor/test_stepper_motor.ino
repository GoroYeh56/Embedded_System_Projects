#include <Stepper.h>

// for 28BYJ-48
#define STEPS 2048
//Create一個stepper的Object(類似ESC object)
// create an instance of stepper class, specifying #
// of steps of the motor and the pins attached to
Stepper stepper(STEPS, 8, 10, 9, 11);

// the previous reading from the analog input
int previous = 0;

void setup() { //(一分鐘30轉：2秒轉一圈)
  Serial.begin(9600);
  stepper.setSpeed(10); // set the speed to 30 RPMs
}
void loop() {
  // get the sensor value from pin A0 接一個感測器再a0
//  int val = analogRead(A0);
////
//  // move a number of steps equal to the change in the
  // sensor reading
  stepper.step(-20);
//  Serial.print(val);
//  Serial.print(" : previous:");
//  Serial.println(previous);

  // remember the previous value of the sensor
//  previous = val;
}
