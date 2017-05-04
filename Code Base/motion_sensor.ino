//int ledPin = 13;  // LED on Pin 13 of Arduino
int pirPin = 9; // Input for HC-S501

int pirValue; // Place to store read PIR Value


void setup() {
  
  //pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
 
  //digitalWrite(ledPin, LOW);
  
  Serial.begin(9600);
  
}

void loop() {
  pirValue = digitalRead(pirPin);
  //digitalWrite(ledPin, pirValue);
  if(pirValue)
  {
    Serial.println("Motion detected");
  }
  else
  {
    Serial.println("No Motion detected");
  }

delay(100);

}
