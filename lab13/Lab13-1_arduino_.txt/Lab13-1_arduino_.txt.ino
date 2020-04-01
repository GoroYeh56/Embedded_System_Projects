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
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  mySerial.begin(9600);
}
char input;
char test;
int i=0;
int j=0;
char Data[20];
char data[20];
// the loop function runs over and over again forever
void loop() {

  if(mySerial.available()>0){
    input = mySerial.read(); //Send "HELLO\r\n"
                             //Send string(cnt)
                             //Send rxData.(input from user.)     
     Data[i++] = input;
    if(input == '\n'){ //Stored to \r.
      for(int m=0; m<i; m++){
          Serial.print(Data[m]);
      }
      memset(Data,0,20);
      i = 0; //reset index.
    }
  }
  //Arduino send to MSP
  if(Serial.available()>0){
      test = Serial.read();//read from user.
      data[j++] = test;
      if(test== '\n'){//Get string from msp.
        for(int m=0; m<j; m++){
            mySerial.print(data[m]);
        }        
//        mySerial.println(data);
        memset(data,0,20);
        j = 0;        
      }
  }
}
