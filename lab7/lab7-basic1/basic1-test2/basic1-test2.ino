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

int Blink[1000];
int time0 = 0;
int time1  = 0;
int state = 0;
int Rpress = 0;
int lastRpress = 0;
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long RlastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long BlastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long GlastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

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

  state = Replay;
    
}

 
int press_time;
int release_time;
int record_time;
int pressingtime = 0;
int index;//store the time when LED should blink.
int readbtn;
void loop() {

      Serial.println(state);
//      Serial.println(event);
  switch(state){
    case Initial:
//    Serial.println("Initial");
//      // read the state of the switch into a local variable:
//    int Rreading = digitalRead(RedbuttonPin);
//    int Greading = digitalRead(GreenbuttonPin);
//    int Breading = digitalRead(BluebuttonPin);
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
                record_time = 0; //start count time.
                index = 0;
                break;
               }
      }
    }
    lastRpress = Rpress; 
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
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
//  // save the reading. Next time through the loop, it'll be the lastButtonState:
//  lastRBtnState = Rreading;
//  lastGBtnState = Greading;
//  lastBBtnState = Breading;
  break;
  
  case Record:
      Serial.println("Recording...");
//      readbtn = 1;
////      int readbtn = digitalRead(RedbuttonPin);
////      record_time++; //every time record time ++.
//      Serial.println("Recording...");
//      digitalWrite(RedLED, HIGH);
//      if(event == pressing){
//          pressingtime++;
//          if(pressingtime>=3000){
//            index--;
//            state = Replay;
//            break;
//          }
//          if(readbtn==HIGH){
//            digitalWrite(RedLED, HIGH);
//            Blink[index++] = record_time;  
//            Serial.println("Press!");
//          }
//          else{ //released.
//               digitalWrite(RedLED, LOW);
//               event = released;
//          }
//      }
//      else{ //released
//          pressingtime = 0; //reset pressing time.
//         if(readbtn == HIGH){
//             digitalWrite(RedLED, HIGH);
//             Blink[index++] = record_time; //should blink.
//             event = pressing;  
//         }
//         else{//still released. do nothing.
//                digitalWrite(RedLED, LOW); 
//         }
//      }
//      press_time = PressTime(GreenbuttonPin, GreenLED);
//      release_time = (RedbuttonPin, RedLED);
  break;
  case Replay:
      Serial.println(index);
//      if(index >=0){
//          Serial.println(index);
//          if(record_time == Blink[index]){
//              digitalWrite(RedLED,HIGH);
//              Serial.println(Blink[index]);
//              Blink[index] = 0; //reset Blink array.
//              index--;
//           }
//           else{
//              digitalWrite(RedLED,LOW);
//           }
//           record_time--;
//      }
//      else{
//        state = Initial;
//      }
  break;
  }//end switch.
}
