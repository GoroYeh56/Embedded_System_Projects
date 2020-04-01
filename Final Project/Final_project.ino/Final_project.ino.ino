 /* PID balance code with ping pong ball and distance sensor sharp 2y0a21
 *  by ELECTRONOOBS: https://www.youtube.com/channel/UCjiVhIvGmRZixSzupD0sS9Q
 *  Tutorial: http://electronoobs.com/eng_arduino_tut100.php
 *  Code: http://electronoobs.com/eng_arduino_tut100_code1.php
 *  Scheamtic: http://electronoobs.com/eng_arduino_tut100_sch1.php 
 *  3D parts: http://electronoobs.com/eng_arduino_tut100_stl1.php   
 */
#include <Wire.h>
#include <Servo.h>
#include<Arduino_FreeRTOS.h>
#include<LiquidCrystal_I2C.h>
#define Auto 0
#define Manual 1
int redPin = 3, greenPin = 5, bluePin = 6; //for RGB LED pins.
int button = 2;
int button_int = 0;     // INT0 is on pin 2
int poten = A1,     yAxis = A2;
int potentiometerVal;
int lastXval, lastYval;
int buzzer = 9;
LiquidCrystal_I2C lcd(0x3F,16,2);
//////////For MSP430/////////
#include <stdio.h>
#include <stdlib.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11);

///////////////////////Inputs/outputs///////////////////////
int Analog_in = A0;
Servo myservo;  // create servo object to control a servo, later attatched to D9
///////////////////////////////////////////////////////

//arduino input 9 bit use digital 2~10.
//input A0 distance, use A1 A2 A3 A4 A5 11 12 send to FPGA

////////////////////////Variables///////////////////////
int Read = 0;
int distance =0;
float elapsedTime, time, timePrev;        //Variables for time control
int distance_previous_error, distance_error;
int period = 50;  //Refresh rate period of the loop is 1ms
int mode = 0; //Initial Auto mode.
///////////////////////////////////////////////////////


///////////////////PID constants///////////////////////
float kp=5; //Mine was 8
float ki=0.2; //Mine was 0.2
float kd=2500; //Mine was 3100
float distance_setpoint = 25;           //Should be the distance from sensor to the middle of the bar in mm
float PID_p, PID_i, PID_d, PID_total;
///////////////////////////////////////////////////////



void setup() {
  //analogReference(EXTERNAL);
  Serial.begin(9600);  
  mySerial.begin(9600);  
    // Now set up three tasks to run independently
    while (!Serial) {
    ; // wait for serial port to connect.
      }
        pinMode(buzzer, OUTPUT);  
        noTone(buzzer);
        pinMode(redPin, OUTPUT); //set RGB LED pins.
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);  
       analogWrite(redPin, 125); //0-255
       analogWrite(greenPin, 125);
       analogWrite(bluePin, 125); 
    pinMode(button, INPUT_PULLUP); //return LOW when down
    attachInterrupt(button_int, handle_click, FALLING);
      
        lcd.init();              
//  xTaskCreate( //Task1 : Login.
//    controlTask,   // Pointer to function for thse task
//    (const portCHAR *) "Task1", // Name for the task
//    128,      // Stack size
//    NULL,     // NULL task parameter
//    1,        // This task will run at First priority 1
//    NULL );   // Do not use the task handle
//  xTaskCreate(DisplayTask, (const portCHAR *) "Task2", 128, NULL, 1, NULL);//Task3 : Display.  
  lcd.begin(16, 2);   // initialize LCD //size of LCD.
  lcd.backlight();    // open LCD backlight
  lcd.setCursor(0,0);
  lcd.print("Auto mode: ");  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(0); //Put the servco at angle 125, so the balance is in the middle
  pinMode(Analog_in,INPUT);  
  time = millis();
}

int last_mode = Auto;

void loop() {
   Check_Btn_from_MSP430();
  if(mode == Auto){
       noTone(buzzer);
        if(last_mode==Manual){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Auto mode: "); 
          last_mode = Auto;         
        }
        lcd.setCursor(0,1);
        lcd.print(distance);
        lcd.setCursor(3,1);
        lcd.print("cm");  
          if (millis() > time+period)
          {  
            time = millis();    
            distance = get_dist(); 
              //Modify minor distance error.  
            if(distance>=33 && distance <36){distance = distance-3;}
            if(distance>=36 && distance <40){distance = distance-3;}
            else if(distance>=40){distance = distance-10;}
            if(distance <= 15){
              analogWrite(redPin,0);
              analogWrite(bluePin,0);  
              analogWrite(greenPin,0);              
            }
            else if(distance >= 45){
              analogWrite(redPin,255);
              analogWrite(bluePin,0);  
              analogWrite(greenPin,0);
            }
        
            else{
              analogWrite(redPin,distance*5);
              analogWrite(bluePin,distance*5);  
              analogWrite(greenPin,distance*5);
            }
            distance_error = distance_setpoint - distance;   
            PID_p = kp * distance_error;
            float dist_difference = distance_error - distance_previous_error;     
            PID_d = kd*((dist_difference)/period);
              
            if(-20 < distance_error && distance_error < 20)
            {
              PID_i = PID_i + (ki * distance_error);
            }
            else
            {
              PID_i = 0;
            }
          
            PID_total = PID_p + PID_i + PID_d;  
            Serial.print("PID befor mappeds: ");
            Serial.print(PID_total);
            Serial.print(" ");  
            
            PID_total = map(PID_total, -150,150,0,150);
            
            if(PID_total < 20){PID_total = 20;}
            if(PID_total >150) {PID_total = 150; } 
          
            myservo.write(PID_total);  
            Serial.print("PID: ");
            Serial.print(PID_total);
            Serial.print(" ");
            Serial.print("Distance: ");
            Serial.println(distance);    
            distance_previous_error = distance_error;
          }
          
  }
  else {//Manual mode, Use Joysticks.
        if(last_mode==Auto){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Manual mode: "); 
          last_mode = Manual;         
        }
        lcd.setCursor(0,1);
        lcd.print(distance);
        lcd.setCursor(3,1);
        lcd.print("cm");
          
            distance = get_dist(); 
              //Modify minor distance error.  
            if(distance>=33 && distance <36){distance = distance-3;}
            if(distance>=36 && distance <40){distance = distance-3;}
            else if(distance>=40){distance = distance-10;}
            //RGB
            if(distance <= 15){
              analogWrite(redPin,0);
              analogWrite(bluePin,0);  
              analogWrite(greenPin,0);              
            }
            else if(distance >= 45){
              analogWrite(redPin,255);
              analogWrite(bluePin,0);  
              analogWrite(greenPin,0);
            }
            else{
              analogWrite(redPin,distance*5);
              analogWrite(bluePin,distance*5);  
              analogWrite(greenPin,distance*5);
            }         
        //Read Potentiometer and write motor.
        potentiometerVal = analogRead(poten);
        Serial.print("Potentiometer value: ");
        Serial.println(potentiometerVal);      
        //Write servo. from 0-1023 to 0-180
        PID_total = map(potentiometerVal, 0,1023, 20, 160);
        myservo.write(PID_total);
        Serial.println("PID_total");
        tone(buzzer, potentiometerVal);
        delay(10);
  }
}
char input;
char test;
int i=0;
int j=0;
char Data[20];
char data[20];
// the loop function runs over and over again forever
void Check_Btn_from_MSP430() {
  static int click_time = 0;
  if(mySerial.available()>0){
    input = mySerial.read(); //Send "HELLO\r\n"
                             //Send string(cnt)
                             //Send rxData.(input from user.)     
     Data[i++] = input;
    if(input == '\n'){ //Stored to \r.
      for(int m=0; m<i; m++){
          Serial.print(Data[m]);
      }
            //if we get pressed signal.
            if(strncmp(Data, "HELLO\r\n",5)==0){
              if(mode == 0)mode = 1;
              else mode = 0;
//                click_time++;              
                //check click time.
//                if(click_time == 1){
//                mode = 1; //Manual mode with joysticks.
//                Serial.println("Now Manual mode");//Flash at 2Hz
//                }
//                else{
//                  mode = 0; //Automatic Balance mode.
//                  Serial.println("Now Auto mode");
//                  click_time = 0;//reset.      
//                }      
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
        memset(data,0,20);
        j = 0;        
      }
  }  
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
     if(mode == 0)mode = 1;
     else mode = 0;
//    if(click_time == 1){
//    mode = 1; //Manual mode with joysticks.
//    Serial.println("Now Manual mode");//Flash at 2Hz
//    }
//    else{
//      mode = 0; //Automatic Balance mode.
//      Serial.println("Now Auto mode");
//      click_time = 0;//reset.      
//    }
  last_int_time = int_time;
}


 
 /*
 * Sharp IR (infrared) distance measurement module for Arduino
 * Measures the distance in cm.

 * Watch the video https://youtu.be/GL8dkw1NbMc

 *  * 

Original library: https://github.com/guillaume-rico/SharpIR

Updated by by Ahmad Nejrabi for Robojax.com
on Feb 03, 2018 at 07:34 in Ajax, Ontario, Canada
 * Permission granted to share this code given that this
 * note is kept with the code.
 * Disclaimer: this code is "AS IS" and for educational purpose only.
 * 
 /*
/*




 */
  // Sharp IR code for Robojax.com
#include <SharpIR.h>

#define IR A0 // define signal pin
#define model 1080 // used 1080 because model GP2Y0A21YK0F is used
// Sharp IR code for Robojax.com
// ir: the pin where your sensor is attached
// model: an int that determines your sensor:  1080 for GP2Y0A21Y
//                                            20150 for GP2Y0A02Y
//                                            430 for GP2Y0A41SK   
/*
2 to 15 cm GP2Y0A51SK0F  use 1080
4 to 30 cm GP2Y0A41SK0F / GP2Y0AF30 series  use 430
10 to 80 cm GP2Y0A21YK0F  use 1080
10 to 150 cm GP2Y0A60SZLF use 10150
20 to 150 cm GP2Y0A02YK0F use 20150
100 to 550 cm GP2Y0A710K0F  use 100550

 */

SharpIR SharpIR(IR, model);
int dis;
int get_dist(){
   dis=SharpIR.distance();  // this returns the distance to the object you're measuring
   return dis;
     
}
