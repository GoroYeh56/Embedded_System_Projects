#include <Servo.h>
Servo myservo;

int potentioPin = A1;
int val;
void setup() {
  // put your setup code here, to run once:
  myservo.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(potentioPin);
  val = map(val, 0, 1023, 0, 180);
  myservo.write(val);
  delay(15);
}
