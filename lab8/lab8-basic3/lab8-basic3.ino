#define TIMER1_COMPA_vect _VECTOR(11) /* Timer/Counter1 Compare Match A */
#define TIMER1_COMPB_vect _VECTOR(12) /* Timer/Counter1 Compare Match B */
#define STEPS 2048
#include <Stepper.h>

Stepper stepper(STEPS, 8, 10, 9, 11);

//8 state, every state is 1/8 * 2048 = 256

#define ON 0
#define OFF 1
#define Play 0
#define Blink 1

int detect=0;
int mode = 0;//initial : Play.
int moved =0;
int trueStep;
const int trigPin = 12, echoPin = 13;
long duration, distance;
long last_distance;
int dir;//0:1 -> left or right.
int randsteps; //0 - 2048.

int redPin = 3, greenPin = 5, bluePin = 6; //for RGB LED pins.

int real_time = 0;
int motor_state = 2;//initial: point to up.
int lastState = 2;

int correct = 1; //default:correct.

void setup() {
  Serial.begin(9600);
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(redPin, OUTPUT); //set RGB LED pins.
   pinMode(greenPin, OUTPUT);
   pinMode(bluePin, OUTPUT);  
   stepper.setSpeed(10); 
   
  noInterrupts(); // atomic access to timer reg.
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 1562;  // give0.1 sec at 16 MHz/1024
  interrupts(); // enable all interrupts
  
  real_time = 0;
   dir = random(0,2);
   randsteps = random(2,17);
   trueStep = 16*randsteps;
    
}


void loop(){

    if (TIFR1 & (1 << OCF1A)) { // wait for time up
      real_time ++;
      TIFR1 = (1<<OCF1A); 
      } 
  
//   digitalWrite(trigPin, LOW); // Clears the trigPin
//   delayMicroseconds(2);
//   /* Sets the trigPin on HIGH state for 10 ms */
//   digitalWrite(trigPin, HIGH);    delayMicroseconds(10);
//   digitalWrite(trigPin, LOW);
//   /* Reads Echo pin, returns sound travel time in ms */
//   duration = pulseIn(echoPin, HIGH);
//   /* Calculating the distance */
//   distance = duration*0.034/2; 
//   Serial.print("Now dist :");
//   Serial.print(distance);
//   Serial.print(" Last :");
//   Serial.println(last_distance);
    if(real_time == 5){
   if(moved == 0){
   if(dir == 0){
      stepper.step(trueStep);
      Serial.println("turn Right.");
   }
   else{
       stepper.step(-trueStep);
             Serial.println("turn Left.");
   }
    moved = 1;
   }
    }
    switch(mode){
    case Play:
      if(real_time==2){
                  digitalWrite(trigPin, LOW); // Clears the trigPin
         delayMicroseconds(2);
   /* Sets the trigPin on HIGH state for 10 ms */
   digitalWrite(trigPin, HIGH);    delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   /* Reads Echo pin, returns sound travel time in ms */
   duration = pulseIn(echoPin, HIGH);
   /* Calculating the distance */
   last_distance = duration*0.034/2; 
        
      }
      if(real_time == 18){//record final distance.
                  digitalWrite(trigPin, LOW); // Clears the trigPin
         delayMicroseconds(2);
   /* Sets the trigPin on HIGH state for 10 ms */
   digitalWrite(trigPin, HIGH);    delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   /* Reads Echo pin, returns sound travel time in ms */
   duration = pulseIn(echoPin, HIGH);
   /* Calculating the distance */
   distance = duration*0.034/2; 
      }
        if(real_time >= 20){
             Serial.print("Now dist :");
   Serial.print(distance);
   Serial.print(" Last :");
   Serial.println(last_distance);
//            Serial.println(real_time/10);
            mode = Blink;
            real_time = 0;
//            Serial.println("Enter Blink mode...");
            break;
        }
        if(dir == 0){ //turn right, we should move closer.
//            stepper.step(trueStep); 
            if(distance < last_distance){
               correct = 1;
//                Serial.println("Closer!");
            }
            else{
                correct = 0; //Wrong.
            }
        }
        else{ //turn left, we should move away.
//            stepper.step(-trueStep);
            if(distance > last_distance){
//              Serial.println("Far away!");
                correct = 1;
            }
            else{
                correct = 0; //Wrong.
            }            
        }
//        last_distance = distance; //Update.
    break;
    case Blink:
    if(real_time >=20){
        Serial.println("Moving back to initial position");
       if(dir == 0){
           stepper.step(-trueStep); //return to initial position.
        }
        else{
          stepper.step(trueStep);
        }
        detect = 0; //reset.
        real_time = 0;
        mode = Play;
        correct = 1; //reset.
        moved = 0; //reset.
        analogWrite(redPin, 0);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0); 
         dir = random(0,2);
         randsteps = random(2,17);
        trueStep = 16*randsteps;
        break;
    }
    else{
      if(correct){
//        Serial.println(correct);
         analogWrite(redPin, 0);
         analogWrite(greenPin, 255);
         analogWrite(bluePin, 0);      
      }
      else{
//                Serial.println(correct);
         analogWrite(redPin, 255);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0); 
      }
    }
    break;
  }
}
