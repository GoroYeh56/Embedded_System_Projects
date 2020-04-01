int button = 2;
int xAxis = A0,     yAxis = A1;
void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP); //return LOW when down
}
void loop() {
  int xVal = analogRead(xAxis);//讀x軸座標
  int yVal = analogRead(yAxis);//讀y
  int isPress = digitalRead(button);//有沒有壓btn?
  Serial.print("X="); Serial.println(xVal);
  Serial.print("Y="); Serial.println(yVal);
  if(isPress == 0) Serial.println("pressed.");
  else Serial.println("Button is not pressed.");
  delay(1000);
}
