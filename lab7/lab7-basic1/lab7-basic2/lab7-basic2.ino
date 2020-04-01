#include <stdio.h>
#include <stdlib.h>
//補debounce
/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
/*
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(6, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(6, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(6, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
*/
/*
  Debounce

  Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
  press), the output pin is toggled from LOW to HIGH or HIGH to LOW. There's a
  minimum delay between toggles to debounce the circuit (i.e. to ignore noise).

  The circuit:
  - LED attached from pin 13 to ground
  - pushbutton attached from pin 2 to +5V
  - 10 kilohm resistor attached from pin 2 to ground

  - Note: On most Arduino boards, there is already an LED on the board connected
    to pin 13, so you don't need any extra components for this example.

  created 21 Nov 2006
  by David A. Mellis
  modified 30 Aug 2011
  by Limor Fried
  modified 28 Dec 2012
  by Mike Walters
  modified 30 Aug 2016
  by Arturo Guadalupi

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Debounce
*/
#define Question 0
#define Answer 1
#define ON 0
#define OFF 1
int count = 0; //count the time btn is pressed in a second.
// constants won't change. They're used here to set pin numbers:
const int RedbuttonPin = 2;    // the number of the pushbutton pin
const int BluebuttonPin = 3;    // the number of the pushbutton pin
const int GreenbuttonPin = 4;    // the number of the pushbutton pin
const int RedLED = 5;      // the number of the LED pin
const int BlueLED = 6;      // the number of the LED pin
const int GreenLED = 7;      // the number of the LED pin

// Variables will change:
int RledState = HIGH;         // the current state of the output pin
int BledState = HIGH;
int GledState = HIGH;
int RbuttonState;             // the current reading from the input pin
int BbuttonState;             // the current reading from the input pin
int GbuttonState;             // the current reading from the input pin
int lastRBtnState = LOW;   // the previous reading from the input pin
int lastBBtnState = LOW;   // the previous reading from the input pin
int lastGBtnState = LOW;   // the previous reading from the input pin


int state = 0;
int event; //ON / OFF
int Wrong = 0;


int Rpress = 0;
int lastRpress = 0;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime[3]= {0,0,0}; //0-1-2 : R-G-B.
int buttonState[3] = {LOW,LOW,LOW};
int lastState[3] = {LOW,LOW,LOW};
int ledState[3]={LOW,LOW,LOW};
int lastButtonState[3]= {0,0,0};
//unsigned long RlastDebounceTime = 0;  // the last time the output pin was toggled
//unsigned long BlastDebounceTime = 0;  // the last time the output pin was toggled
//unsigned long GlastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int record_time;
//
void Light(int readbtn, int ledPin);
int debounce(int buttonPin, int ledPin);
//int Recording(int Btn, int LED, int*Blink, int index,int*OFF, int readbtn, int real_time);
//int Replaying(int LED, int*Blink, int*OFF,int index,int real_time);
int Q[5];//Question sequence.
int A[5];//Answer sequence.
int index; //index for Q and A.
int randled; //randon number for led.

void setup() {
  Serial.begin(9600);
  pinMode(RedbuttonPin, INPUT);
  pinMode(BluebuttonPin, INPUT);
  pinMode(GreenbuttonPin, INPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);
//
//  // set initial LED state
//  digitalWrite(RedLED, RledState);
//  digitalWrite(BlueLED, BledState);
//  digitalWrite(GreenLED, GledState);
  
  // initialize timer1 
  noInterrupts(); // atomic access to timer reg.
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 15624;  // give 0.1 sec at 16 MHz/1024 *1 -1.
  interrupts(); // enable all interrupts

  state = Question;
  index = 0;
  event = OFF;
//  Serial.println(record_time);
  randomSeed(analogRead(A0));
}
int press_time;
int release_time;
int pressingtime;
int pressingtime2;
int real_time = 0;
int Endtime;
int Rreading;
int Greading;
int Breading;
//int index;//store the time when LED should blink.
//int off_index;
void loop() {
     // read the state of the switch into a local variable:
//  Rreading = debounce(RedbuttonPin,RedLED);
//  Greading = debounce(GreenbuttonPin,GreenLED);
//  Breading = debounce(BluebuttonPin, BlueLED);
    Rreading = digitalRead(RedbuttonPin);
        Greading = digitalRead(GreenbuttonPin);
            Breading = digitalRead(BluebuttonPin);
    if (TIFR1 & (1 << OCF1A)) { // wait for time up
        real_time++;
//    after one second.

  switch(state){
    case Question:
      Serial.print(index);
      Serial.print(": ");
      if(event == ON){
          digitalWrite(RedLED, LOW);
          digitalWrite(GreenLED, LOW);
          digitalWrite(BlueLED, LOW);   
          event = OFF;
      }
      else{ // now should turn on.     
      //if index == 5 , means finish recording Question. Change to Answer mode.
      if(index==5){
          state = Answer;
          real_time = 0;//reset time.
          index = 0; //reset index.
          Wrong = 0;
          break;   
      }
      randled = random(5,8);
      if(randled == 5){//blink Red LED and keep Q[index] = 2.
        digitalWrite(RedLED, HIGH);
        Serial.println(" Red");
        Q[index++] = 2;
      }
      else if(randled == 6){
        digitalWrite(BlueLED, HIGH);
        Serial.println(" Blue");        
        Q[index++] = 3;
      }
      else if(randled == 7){
        digitalWrite(GreenLED,HIGH);
        Serial.println(" Green");
        Q[index++] = 4;
      }
      event = ON;
      }
      
//     if(Rpress == lastRpress +1){ 
//        count++;
//        if(count==1){
//          time0 = millis(); //record first press time.
//      }
//      if(count>=2){
//          time1 = millis();//start record time;
//              if(time1 - time0 > 1000){
//                  time0 = time1 = 0;//reset time.
//                  count = 0; 
//                }
//               else{
//                state = Record;
//                digitalWrite(RedLED, LOW);
//                digitalWrite(GreenLED, LOW);
//                digitalWrite(BlueLED, LOW);
//                Serial.println("ALL OFF");
//                real_time = 0; //start count time.
//                //Why我在這邊有歸零，Endtime還會等於付的?
//                count = 0;
//                break;
//               }
//      }
//    }
//    lastRpress = Rpress; 
//  // check to see if you just pressed the button
//  // (i.e. the input went from LOW to HIGH), and you've waited long enough
//  // since the last press to ignore any noise:
//
//  // If the switch changed, due to noise or pressing:
//  if (Rreading != lastRBtnState) {
//    // reset the debouncing timer
//    RlastDebounceTime = millis();
//  }
//  //Green
//    if (Greading != lastGBtnState) {
//    // reset the debouncing timer
//    GlastDebounceTime = millis();
//  }
//  //Blue
//    if (Breading != lastBBtnState) {
//    // reset the debouncing timer
//    BlastDebounceTime = millis();
//  }
//  //Red.
//  if ((millis() - RlastDebounceTime) > debounceDelay) {
//    // whatever the reading is at, it's been there for longer than the debounce
//    // delay, so take it as the actual current state:
//
//    // if the button state has changed:
//    if (Rreading != RbuttonState) {
//      RbuttonState = Rreading;
//
//      // only toggle the LED if the new button state is HIGH
//      if (RbuttonState == HIGH) {
//        RledState = !RledState;
//        lastRpress = Rpress;
//        Rpress++;
//      }
//    }
//  }
//    //Green.
//  if ((millis() - GlastDebounceTime) > debounceDelay) {
//    // whatever the reading is at, it's been there for longer than the debounce
//    // delay, so take it as the actual current state:
//
//    // if the button state has changed:
//    if (Greading != GbuttonState) {
//      GbuttonState = Greading;
//
//      // only toggle the LED if the new button state is HIGH
//      if (GbuttonState == HIGH) {
//        GledState = !GledState;
//      }
//    }
//  }
//    //Blue.
//  if ((millis() - BlastDebounceTime) > debounceDelay) {
//    // whatever the reading is at, it's been there for longer than the debounce
//    // delay, so take it as the actual current state:
//
//    // if the button state has changed:
//    if (Breading != BbuttonState) {
//      BbuttonState = Breading;
//
//      // only toggle the LED if the new button state is HIGH
//      if (BbuttonState == HIGH) {
//        BledState = !BledState;
//      }
//    }
//  }
//
//  // set the LED:
//  digitalWrite(RedLED, RledState);
//  digitalWrite(GreenLED, GledState);
//  digitalWrite(BlueLED, BledState);
////   save the reading. Next time through the loop, it'll be the lastButtonState:
//  lastRBtnState = Rreading;
//  lastGBtnState = Greading;
//  lastBBtnState = Breading;
  break;
  
  case Answer:
        Light(Rreading, RedLED);
        Light(Breading, BlueLED);
        Light(Greading, GreenLED);
        if(index == 5){
            for(int i=0; i<=4; i++){
               if(Q[i] != A[i]){ //Mismatch.
                Serial.println("Wrong");
                state = Question;
                index = 0;
                Wrong = 1;
                break;            
                }
            }
            if(Wrong == 0){
              Serial.println("Right");
              index = 0;
              state = Question;
            }
            else{
              break;
            }
        }
        if(real_time >= 8){
            Serial.println("Time's UP");
            index = 0;
            state = Question;
        }
        if(Rreading ==HIGH){ //user press Red btn.
            A[index++] = 2;
            Serial.println("Press: Red.");
            real_time = 0; //reset time.
        }
        else if(Breading == HIGH){
            A[index++] = 3;
            Serial.println("Press: Blue.");
            real_time = 0; //reset time.
        }
        else if(Greading == HIGH){
            A[index++] = 4;
            Serial.println("Press: Green.");
            real_time = 0; //reset time.
        }
        //else : wait.
  break;
  }//end switch.
      TIFR1 = (1<<OCF1A); } // clear overflow flag
}

//Blink Btn.
void Light(int buttonState, int ledPin){
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
}
}

int debounce(int buttonPin, int ledPin) {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[ledPin]) {
    // reset the debouncing timer
    lastDebounceTime[ledPin] = millis();
  }

  if ((millis() - lastDebounceTime[ledPin]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState[ledPin] = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState[ledPin] = !ledState[ledPin];
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState[ledPin]);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState[ledPin] = reading;

  return reading;
}
