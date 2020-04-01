int redPin = 9, greenPin = 10, bluePin = 11; //for RGB LED pins.
int button_int = 0;     // INT0 is on pin 2
int toggle_on = false;  // Button click switches LED
void setup() {
  attachInterrupt(button_int, handle_click, RISING);
  // Register handler
  Serial.begin(9600);
    pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
    pinMode(2, INPUT_PULLUP); //return LOW when down
}
void loop() {
  if (toggle_on) {
      analogWrite(redPin, 50); //設定成某種顏色 (三種搭配
    analogWrite(greenPin, 100);
    analogWrite(bluePin, 150);  

  } else {
      analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);  
  }
}
// button ISR
void handle_click() { // button debouncing, toggle LED
  static unsigned long last_int_time = 0;
  unsigned long int_time = millis(); // Read the clock

  if (int_time - last_int_time > 200 ) {  
    // Ignore when < 200 msec
    Serial.println("Pressed!");
    toggle_on = !toggle_on;  // switch LED
  }
//
  last_int_time = int_time;
}
