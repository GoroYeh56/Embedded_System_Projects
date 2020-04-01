#define TIMER1_COMPA_vect _VECTOR(11) /* Timer/Counter1 Compare Match A */
#define TIMER1_COMPB_vect _VECTOR(12) /* Timer/Counter1 Compare Match B */
#define STEPS 2048
#include <Stepper.h>

Stepper stepper(STEPS, 8, 10, 9, 11);



#define ON 0
#define OFF 1
#define A 0
#define B 1
int mode = A;
int intensity;//red LED ON intensity time, OFF (5-intensity) times.
int redPin = 3, greenPin = 5, bluePin = 6; //for RGB LED pins.
int count;
int event = OFF;
//int event[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int real_time;
int button = 2;
int xAxis = A0,     yAxis = A1;
int xValue, yValue;
int lastXval, lastYval;
int lastR,lastG,lastB;
int button_int = 0;     // INT0 is on pin 2
void flash_red(int pin, int intensity);
void change_intensity() ;
int Rintensity;
int Gintensity;
int Bintensity;


void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);//intrinsic LED.
  pinMode(redPin, OUTPUT); //set RGB LED pins.
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
   stepper.setSpeed(10);
  Rintensity = 126;
  Gintensity = 126;
  Bintensity = 126;
   analogWrite(redPin, Rintensity);
   analogWrite(greenPin, Gintensity);
   analogWrite(bluePin, Bintensity); 
   
    lastR = 126;
    lastG = 126;
    lastB = 126;
    pinMode(button, INPUT_PULLUP); //return LOW when down
    attachInterrupt(button_int, handle_click, FALLING);

  
  cli(); // stop interrupts, atomic access to reg.
  // initialize timer1 
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 7812;  // target for counting
//  TIMSK1 |= (1<<OCIE1A); // enable timer compare int.
  sei(); // enable all interrupts


  count = 0;
  real_time = 0;
  intensity = 0;
    
}

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
    mode = B;
    TIMSK1 |= (1<<OCIE1A); //enable TimerA interrupt.
    Serial.println("Now B mode");//Flash at 2Hz
    }
    else{
      mode = A;
      TIMSK1 &= !(1<<OCIE1A);
      Serial.println("Now A mode");
      click_time = 0;//reset.      
    }
  last_int_time = int_time;
}


//void flash(int pin, int intensity){
//    if(event[pin] == OFF){//should turn on.
//      analogWrite(pin, 255 );
////      Serial.println("Now turn ON!");
//      event[pin] = ON;
//    }
//    else{//already ON. wait for time equal to intensity.
//       analogWrite(pin, 0 ); //turn OFF.
//       event[pin] = OFF;
////       Serial.println("Now turn OFF!");
//    }
//}
ISR(TIMER1_COMPA_vect) { // Timer1 ISR
//when enter, 1 sec, go to  flash function.
//  Serial.println("Hi");
  if(event == ON){
      analogWrite(redPin, Rintensity);
      analogWrite(greenPin, Gintensity);
      analogWrite(bluePin, Bintensity); 
    event = OFF;
  }
  else{
//      Serial.println("Hi");
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0); 
    event = ON;
  }
//  xValue = analogRead(xAxis);
//  yValue = analogRead(yAxis);
//  Serial.print("X value: ");
//  Serial.println(xValue);
//    Serial.print("last value: ");
//  Serial.println(lastXval);
//  Serial.print("Y value: ");
//  Serial.println(yValue);

//  if(xValue != lastXval){
//    analogWrite(greenPin,lastG+(xValue - lastXval));
//    lastG = lastG+(xValue - lastXval);
//    lastXval = xValue; //update last value of X.
////    Serial.println("Green changed.");
//  }
//  if(yValue != lastYval){
//    analogWrite(redPin,lastR+(yValue - lastYval));
//    lastR = lastR+(yValue - lastYval);
//        lastYval = yValue;
////            Serial.println("Red changed.");
//  }
}
//ISR(TIMER1_COMPB_vect) { // TimerB ISR
////when enter, 0.01sec, go to  flash function.
//  if(event[11] == ON){
//      analogWrite(redPin, 0); //設定成某種顏色 (三種搭配
//      analogWrite(greenPin, 0);
//      analogWrite(bluePin, 0);  
//      event[11] = OFF;
//
//  }
//  else{
//      analogWrite(redPin, 50); //設定成某種顏色 (三種搭配
//      analogWrite(greenPin, 100);
//      analogWrite(bluePin, 150); 
//      event[11] = ON;
//  }
//  real_time ++;
//  if(real_time == 15){//should change intensity.
//      //change intensity.
////      change_intensity() ;
//      real_time = 0;
//  }
//}

//void change_intensity() { 
//  static int count_intensity = 0;
//  count_intensity++;
//  if (count_intensity <= 5) {  
//    intensity += 1;
//  }
//  else{
//    intensity = 0;//reset.
//    count_intensity = 0;
//  }
//  Serial.print("Change intensity to: ");
//  Serial.println(intensity);
//}
void loop() {
  stepper.step(256);
    xValue = analogRead(xAxis);
    yValue = analogRead(yAxis);
    Serial.print("X value: ");
    Serial.println(xValue);
    Serial.print("Y value: ");
    Serial.println(yValue);
//    map(value, fromLow, fromHigh, toLow, toHigh)
//    map(value, fromLow, fromHigh, toLow, toHigh)
//    map(value, fromLow, fromHigh, toLow, toHigh)
  Gintensity = xValue/4;
  Rintensity = yValue/4;
  if(abs(xValue-yValue) <=50){
    Bintensity = (xValue + yValue)/8;
  }
    if(abs(xValue)+abs(yValue) >= 1023-200){
    Bintensity = (yValue - xValue + 1024 )/8;
  }
//  Bintensity = sqrt(Gintensity*Gintensity + Rintensity*Rintensity);
  int R = sqrt(xValue*xValue + yValue*yValue);
//  if(xValue >= 1023/2){//Green or Blue.
//        if(yValue > 512 + 512/2 && yValue <= 1023- 512/2){// Blue increase.
//            Bintensity = (255-126)* R/(1023-512);
//        }
//        else if(yValue >= 512-20){//Green intensity increase. .
//            Gintensity = (255-126)*(1+ xValue/(1023-512));
//        }
//  }
//  if(xValue < 1023/2){//Green intensity decrease.
//        if(yValue > 512 + 512/2 - 30  && yValue <= 1023- 512/2 + 30){// Blue increase.
//          Bintensity = (255-126)* R/(1023-512);
//        }
//        else if(yValue >= 512-20){//Green intensity decrease. .
//            Gintensity = /*(255-126)* (1-*/ xValue/(1023-512);
//        }
//  }
//  if(yValue > 1023/2){//Red intensity increase.
//      if(xValue > 512 + 512/2 - 30  && xValue <= 1023- 512/2 + 30){ // Blue decrease.
//        Bintensity = 126* R/(1023-512);
//      }
//      else if (xValue >= 512-30){ //Red intensity increase.
//        Rintensity = (255-126)* (1+ yValue/(1023-512));
//        
//      }
//  if(yValue < 1023/2){//Red intensity decrease.
//      if(xValue > 512 + 512/2 - 30  && xValue <= 1023- 512/2 + 30){ // Blue decrease.
//        Bintensity = 126* R/(1023-512);
//      }
//      else if (xValue >= 512-30){ //Red intensity decrease
//         Rintensity = yValue/(1023-512);
//        
//      }
//  }
  if(mode == A){
     analogWrite(redPin, Rintensity);
   analogWrite(greenPin, Gintensity);
   analogWrite(bluePin, Bintensity); 
  }

//  if(xValue > lastXval+100){
//    analogWrite(greenPin,lastG+(xValue - lastXval));
//    lastG = lastG+(xValue - lastXval);
//    lastXval = xValue; //update last value of X.
//    Serial.println("Green changed.");
//  }
//  if(yValue > lastYval+50){
//    analogWrite(redPin,lastR+(yValue - lastYval));
//    lastR = lastR+(yValue - lastYval);
//        lastYval = yValue;
////            Serial.println("Red changed.");
//  }

  }
