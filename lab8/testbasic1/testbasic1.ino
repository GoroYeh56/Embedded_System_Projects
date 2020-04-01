#define TIMER1_COMPA_vect _VECTOR(11) /* Timer/Counter1 Compare Match A */
#define TIMER1_COMPB_vect _VECTOR(12) /* Timer/Counter1 Compare Match B */
#define ON 0
#define OFF 1

int intensity;//red LED ON intensity time, OFF (5-intensity) times.
int redPin = 11, greenPin = 10, bluePin = 9; //for RGB LED pins.
int count;
//int event = OFF;
int event[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int real_time;
int button = 2;
int xAxis = A0,     yAxis = A1;
int xValue, yValue;
int lastXval, lastYval;
int lastR,lastG,lastB;
int button_int = 0;     // INT0 is on pin 2
void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT); //set RGB LED pins.
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
      analogWrite(redPin, 126);
    analogWrite(greenPin, 126);
    analogWrite(bluePin, 126); 
  
  cli(); // stop interrupts, atomic access to reg.
  // initialize timer1 
  TCCR1A = 0;    TCCR1B = 0;    TCNT1 = 0;
  TCCR1B |= (1 << WGM12); // turn on CTC
  TCCR1B |= (1<<CS12) | (1<<CS10); // 1024 prescaler
  OCR1A = 7812;  // target for counting
  TIMSK1 |= (1<<OCIE1A); // enable timer compare int.
  sei(); // enable all interrupts


  count = 0;
  real_time = 0;
  intensity = 0;
 
//    analogWrite(redPin, 126);
//    analogWrite(greenPin, 126);
//    analogWrite(bluePin, 126);  
//    
}

//void handle_click() { // button debouncing, press btn -> RGB LED flashes in 2 Hz driven by another timer interrupt
//  static unsigned long last_int_time = 0;
//  unsigned long int_time = millis(); // Read the clock
////  int  isPress = digitalRead(button);//有沒有壓btn?
//  static int click_time = 0;
//    if (int_time - last_int_time > 200 ) {  
//    // Ignore when < 200 msec
//    click_time++;
//  }
//    if(click_time == 1){
////    TIMSK1 |= (1<<OCIE1B); //enable TimerB interrupt.
////    TIMSK1 &= !(1<<OCIE1A);
//    Serial.println("Now B mode");
//    }
//    else{
////      TIMSK1 |= (1<<OCIE1A); //enable TimerA interrupt.
////      TIMSK1 &= !(1<<OCIE1B);
//      Serial.println("Now A mode");
//      click_time = 0;//reset.      
//    }
//  last_int_time = int_time;
//}

ISR(TIMER1_COMPA_vect) { // Timer1 ISR

  Serial.println("Hi");

}
void loop(){
}
