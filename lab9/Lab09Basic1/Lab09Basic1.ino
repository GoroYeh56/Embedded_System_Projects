#include<stdio.h>

//Two photoresistors-> Two parking space.
//if value < 500 : means has car.
int Light[2]= {0,0}; //photoresistor at A2,A3. 
int isPark[2] = {0,0};
int space = 0;
              //a,b, c, d, e, f, g.  dp
int pins[8] = {2, 3, 4, 5, 6, 7, 13, 12};//pins to 7-seg.
boolean data[3][8] = { 
// define the pins to light the 3 number: 0, 1, 2
  {true, true, true, true, true, true, false, false}, // 0
  {false, true, true, false, false, false, false, false}, // 1
  {true, true, false,true, true, false, true, false} // 2
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A2, INPUT);
}
//seven-segment display to show the number of empty parking spaces. 
void loop() {
  space = 2;
  // put your main code here, to run repeatedly:
    Light[0]= analogRead(A2);//read the value of photoresistor.
    Light[1]= analogRead(A3);//read the value of photoresistor.
       Serial.print(Light[0]);
        Serial.print(" Light[1]: ");
       Serial.println(Light[1]);
//    for(int j = 0; j<=1; j++){
//      if(isPark[j] == 0){
//        if(space < 2){         
//          space++;
//        }
//      }
//      else{
//        if(space>0)space--;
//      }
//    }
    if(Light[0] < 700 ){ //There's a car.
      isPark[0] = 1;
      space--;

    }
    else{
      isPark[0] = 0;
    }
    if(Light[1] < 700) {//There's a car.
      isPark[1] = 1;
      space--;
    }
    else{
      isPark[1] = 0;
    }

// to display the number 0, 1, or 2
    for(int i=0; i<=7; i++){
        digitalWrite(pins[i], data[space][i] == true ? HIGH : LOW);}     
    Serial.println(space);
}
