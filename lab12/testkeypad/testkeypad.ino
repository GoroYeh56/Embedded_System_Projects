#include "AnalogMatrixKeypad.h"
// include the file in your lab12 folder
// └── lab12
//      ├── lab12.ino
//      ├── AnalogMatrixKeypad.h
//      └── AnalogMatrixKeypad.cpp
AnalogMatrixKeypad keypad(A0);
// init keypad and keypad input is A0
void setup(){
    Serial.begin(9600);
}
void loop(){
    char key = keypad.readKey();
    // read the keypad value
    if (key != KEY_NOT_PRESSED) {
        // if keypad is pressed, print the key
        Serial.println(key);
    }
}
