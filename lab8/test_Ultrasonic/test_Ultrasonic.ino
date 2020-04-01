const int trigPin = 12, echoPin = 13;
long duration, distance;
void setup() {
  Serial.begin(9600);
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
}
void loop() {
   digitalWrite(trigPin, LOW); // Clears the trigPin
   delayMicroseconds(2);
   /* Sets the trigPin on HIGH state for 10 ms */
   digitalWrite(trigPin, HIGH);    delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   /* Reads Echo pin, returns sound travel time in ms */
   duration = pulseIn(echoPin, HIGH);
   /* Calculating the distance */
   distance = duration*0.034/2; 
   Serial.println(distance);
}
