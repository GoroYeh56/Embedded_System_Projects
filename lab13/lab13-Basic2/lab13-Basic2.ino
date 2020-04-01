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
#define TIMER1_COMPA_vect _VECTOR(11) /* Timer/Counter1 Compare Match A */
#include <stdio.h>
#include <stdlib.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11);
int redPin = 3, greenPin = 5, bluePin = 6; //for RGB LED pins.
int randomLEDpin;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(redPin, OUTPUT); //set RGB LED pins.
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
  
  cli(); // stop interrupts, atomic access to reg.
  // initialize timer1 
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 15624;  // target for counting 1sec.
 // 16 MHz/1024 *0.1 -1.
  TIMSK1 |= (1<<OCIE1A); // enable timer compare int.
  sei(); // enable all interrupts
//  interrupts(); // enable all interrupts

   analogWrite(redPin, 0);
   analogWrite(greenPin, 0);
   analogWrite(bluePin, 0);     
}
 int red=0;
 int event = 0;
 int Blue_is_ON = 0;
 int correct = 0;
ISR(TIMER1_COMPA_vect) { // Timer1 ISR
//when enter, 1 sec, go to  flash function.
  if(event==0){ //OFF, should ON.
      randomLEDpin = random(0,3);
      switch(randomLEDpin){
        Serial.println("Turn on.");
        case 0: //Red.
           analogWrite(redPin, 255);
           analogWrite(greenPin, 0);
           analogWrite(bluePin, 0);    
        break;
        case 1:
           analogWrite(redPin, 0);
           analogWrite(greenPin, 255);
           analogWrite(bluePin, 0); 
        break;
        case 2: //Blue
           analogWrite(redPin, 0);
           analogWrite(greenPin, 0);
           analogWrite(bluePin, 255);   
           Blue_is_ON = 1;  
//           Serial.print("Should_press_btn\r\n"); //You should press MSP430 btn.
        break;
      }
      event = 1; //now ON.
  }
  else{ //now ON, should OFF.
       //Turn OFF.
       if(Blue_is_ON && !correct){
          red = 1;
       }
       Blue_is_ON = 0;
       analogWrite(redPin, 0);
       analogWrite(greenPin, 0);
       analogWrite(bluePin, 0); 
       event = 0;  
  }
}


char input;
char test;
int i=0;
int j=0;
char Data[20];
char data[20];
// the loop function runs over and over again forever
void loop() {
//  Serial.println(event);

 
  if(mySerial.available()>0){
    input = mySerial.read(); //Send "HELLO\r\n"
                             //Send string(cnt)
                             //Send rxData.(input from user.)     
     Data[i++] = input;
    if(input == '\n'){ //Stored to \r.
      for(int m=0; m<i; m++){
          Serial.print(Data[m]);
      }
        if(Blue_is_ON){
//            Serial.println("Did you press btn?");
            //if we get pressed signal.
            if(strncmp(Data, "HELLO\r\n",5)==0){
                correct = 1;
                mySerial.print("green on\r\n");
                delay(1000);
                mySerial.print("off\r\n");
                correct = 0;
            }
            else{
                // red on 1 sec.
                mySerial.print("red on\r\n");
                delay(1000);
                mySerial.print("off\r\n");                
            }
        }      
      memset(Data,0,20);
      i = 0; //reset index.
    }
  }

  if(red){
    red = 0;
    correct = 0;
                mySerial.print("red on\r\n");
                delay(1000);
                mySerial.print("off\r\n");      
  }
  //Arduino send to MSP
  if(Serial.available()>0){
      test = Serial.read();//read from user.
      data[j++] = test;
      if(test== '\n'){//Get string from msp.
        for(int m=0; m<j; m++){
            mySerial.print(data[m]);
        }        
        memset(data,0,20);
        j = 0;        
      }
  }  
}
