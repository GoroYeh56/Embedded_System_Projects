#include <stdio.h>
#include <stdlib.h>
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
#define Initial 0
#define Record 1
#define Replay 2
#define pressing 3
#define released 4

int count = 0; //count the time btn is pressed in a second.
int event=4; //pressing / released.
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

int RBlink[20];
int ROFF[20];
int BBlink[20];
int BOFF[20];
int GBlink[20];
int GOFF[20];
int time0 = 0;
int time1  = 0;
int state = 0;
int Rpress = 0;
int lastRpress = 0;
int RONindex=0;
int ROFFindex=0;
int BONindex=0;
int BOFFindex=0;
int GONindex=0;
int GOFFindex=0;
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long RlastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long BlastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long GlastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int record_time;

int Recording(int Btn, int LED, int*Blink, int index,int*OFF, int readbtn, int real_time);
int Replaying(int LED, int*Blink, int*OFF,int index,int real_time);

void setup() {
  Serial.begin(9600);
  pinMode(RedbuttonPin, INPUT);
  pinMode(BluebuttonPin, INPUT);
  pinMode(GreenbuttonPin, INPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);

  // set initial LED state
  digitalWrite(RedLED, RledState);
  digitalWrite(BlueLED, BledState);
  digitalWrite(GreenLED, GledState);
  
  // initialize timer1 
  noInterrupts(); // atomic access to timer reg.
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 1562;  // give 0.1 sec at 16 MHz/1024 *0.1 -1.
  interrupts(); // enable all interrupts

  state = Initial;
  record_time = 0;
  Serial.println(record_time);
    
}

 
int press_time;
int release_time;
int pressingtime;
int pressingtime2;
int real_time = 0;
int Endtime;
//int index;//store the time when LED should blink.
//int off_index;
void loop() {
     // read the state of the switch into a local variable:
    int Rreading = digitalRead(RedbuttonPin);
    int Greading = digitalRead(GreenbuttonPin);
    int Breading = digitalRead(BluebuttonPin);

    if (TIFR1 & (1 << OCF1A)) { // wait for time up
        real_time++;
//                Serial.println(real_time);

  switch(state){
    case Initial:
//    Serial.println("Initial");
     if(Rpress == lastRpress +1){ 
        count++;
        if(count==1){
          time0 = millis(); //record first press time.
      }
      if(count>=2){
          time1 = millis();//start record time;
              if(time1 - time0 > 1000){
                  time0 = time1 = 0;//reset time.
                  count = 0; 
                }
               else{
                state = Record;
                digitalWrite(RedLED, LOW);
                digitalWrite(GreenLED, LOW);
                digitalWrite(BlueLED, LOW);
                Serial.println("ALL OFF");
                real_time = 0; //start count time.
                //Why我在這邊有歸零，Endtime還會等於付的?
                count = 0;
                break;
               }
      }
    }
    lastRpress = Rpress; 
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (Rreading != lastRBtnState) {
    // reset the debouncing timer
    RlastDebounceTime = millis();
  }
  //Green
    if (Greading != lastGBtnState) {
    // reset the debouncing timer
    GlastDebounceTime = millis();
  }
  //Blue
    if (Breading != lastBBtnState) {
    // reset the debouncing timer
    BlastDebounceTime = millis();
  }
  //Red.
  if ((millis() - RlastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (Rreading != RbuttonState) {
      RbuttonState = Rreading;

      // only toggle the LED if the new button state is HIGH
      if (RbuttonState == HIGH) {
        RledState = !RledState;
        lastRpress = Rpress;
        Rpress++;
      }
    }
  }
    //Green.
  if ((millis() - GlastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (Greading != GbuttonState) {
      GbuttonState = Greading;

      // only toggle the LED if the new button state is HIGH
      if (GbuttonState == HIGH) {
        GledState = !GledState;
      }
    }
  }
    //Blue.
  if ((millis() - BlastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (Breading != BbuttonState) {
      BbuttonState = Breading;

      // only toggle the LED if the new button state is HIGH
      if (BbuttonState == HIGH) {
        BledState = !BledState;
      }
    }
  }

  // set the LED:
//  digitalWrite(RedLED, RledState);
//  digitalWrite(GreenLED, GledState);
//  digitalWrite(BlueLED, BledState);
//   save the reading. Next time through the loop, it'll be the lastButtonState:
  lastRBtnState = Rreading;
  lastGBtnState = Greading;
  lastBBtnState = Breading;
  break;
  
  case Record:
        RONindex = Recording(RedbuttonPin, RedLED, RBlink, RONindex, ROFF, Rreading, real_time,lastRBtnState);
        BONindex = Recording(BluebuttonPin, BlueLED, BBlink,BONindex, BOFF,Breading, real_time,lastBBtnState);
        GONindex = Recording(GreenbuttonPin, GreenLED, GBlink,GONindex, GOFF,Greading, real_time,lastGBtnState);
        if(Rreading==HIGH)lastRBtnState = 3;//pressing.
        else lastRBtnState = 4;
        if(Breading==HIGH)lastBBtnState = 3;//pressing.
        else lastBBtnState = 4;
        if(Greading==HIGH)lastGBtnState = 3;//pressing.
        else lastGBtnState = 4;                       
        if(Rreading == HIGH && Breading == HIGH && Greading == HIGH){
          Serial.println("All pressed!");
          digitalWrite(RedLED,LOW);
          digitalWrite(BlueLED,LOW);
          digitalWrite(GreenLED,LOW);
          RONindex = 0;
          BONindex = 0;
          GONindex = 0;
          Endtime = real_time;//record end time.
          state = Replay;
          real_time = 0;//reset.
      }
  break;
  case Replay:
//      Serial.print("Timestamp(ms):");
//      Serial.println(record_time);
       RONindex =  Replaying(RedLED, RBlink, ROFF,RONindex,real_time);
       GONindex =  Replaying(GreenLED, GBlink, GOFF,GONindex,real_time);
       BONindex =  Replaying(BlueLED, BBlink, BOFF,BONindex,real_time);
//      Serial.println(index);
//      Endtime = abs(Endtime);//為甚麼endtime會有負的?
//      if(record_time<=Endtime){
//          if(record_time == RBlink[index]){
//              digitalWrite(RedLED,HIGH);
//              Serial.print("ON index:");
//              Serial.println(index);
//            Serial.print("Blink Timestamp(ms):");
//            Serial.println(record_time);              
//              RBlink[index] = 0; //reset Blink array.
//              index++;
//           }
//           else if(record_time == ROFF[off_index]){
//              digitalWrite(RedLED,LOW);
//              Serial.print("OFF index:");
//              Serial.println(off_index);
//              ROFF[off_index] = 0; //reset Blink array.
//              off_index++;
//           }
//           record_time++;
//      }
//      else{
//        index = 0;
//        off_index = 0;
//        record_time = 0;
////        Endtime = 0; 這行沒註解的話回到Record state會出問題 why?
//        state = Record;
//        event = released;
//      }
  break;
  }//end switch.
      TIFR1 = (1<<OCF1A); } // clear overflow flag
}

//
int Recording(int Btn, int LED, int*Blink, int index,int*OFF, int readbtn, int real_time, int event){ //event == lastBTNstate
        if(event == pressing){
            if(readbtn==HIGH){
              digitalWrite(LED, HIGH);
            }
            else if(readbtn == LOW){ //press->released: keep this point.
               digitalWrite(LED, LOW);
               event = released;
              Serial.print("We record OFF index:");
              Serial.println(index);
               OFF[index++] = real_time;
           }
          }
      else{ //released
         if(readbtn == HIGH){ //release to press:remember this point.
             digitalWrite(LED, HIGH);
//             pressingtime = millis();//reset pressing time.
              Serial.print("We record ON index:");
              Serial.println(index);
              Blink[index] = real_time; //should blink.
              event = pressing;  
         }
         else if(readbtn == LOW){//still released. do nothing.
                digitalWrite(LED, LOW); 
         }
      }
      return index;    
}

int Replaying(int LED, int*Blink, int*OFF,int index, int real_time){
        if(real_time<=Endtime){
          if(real_time == Blink[index]){
              digitalWrite(LED,HIGH);
              Serial.print("ON index:");
              Serial.println(index);
////            Serial.print("Blink Timestamp(ms):");
//              Serial.println(real_time);              
              Blink[index] = 0; //reset Blink array.
           }
           else if(real_time == OFF[index]){
              digitalWrite(LED,LOW);
              Serial.print("OFF index:");
              Serial.println(index);
              OFF[index++] = 0; //reset Blink array.
              //move index to next.
           }
      }
      else{
        index = 0;
        real_time = 0;
//      Endtime = 0; 這行沒註解的話回到Record state會出問題 why?
        state = Initial;
        Serial.println("Back to Initial state!");
        event = released;
      }
      return index;
}
