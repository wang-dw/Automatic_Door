/* Author: David Wang

   Automatic Door Using 28BYJ-48 Stepper motor, HC-SR04 Ultrasonic sensor, and HC-SR501 PIR
   motion sensor.
*/

#include <Stepper.h>

#define STEPS_PER_MOTOR_REVOLUTION 32
//#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64  //2048
#define STEPS_TO_END 15 //STEPS TO OPEN DOOR

Stepper stepper(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);

int  Steps2Take;
int startRev = 0; //INITIAL COUNTER FOR STEPS

const int trigPin = 6; //ULTRASONIC TRIGGER PIN
const int echoPin = 2; //ULTRASONIC ECHO PIN
const int lockPin = 4; //LOCK DOOR BUTTON
const int pirPin = 13; //PIR MOTION SENSOR PIN
const int buzzer = 3; //BUZZER PIN

int pirValue; //PIR MOTION SENSOR VALUE
int lockValue; //LOCK DOOR VALUE

int buttonState = 0; //LOCK DOOR STATE
int lastButtonState = 0;

int STATE; //STATE MACHINE VARIABLE

const int idle = 0; //STATES
const int openDoor = 1;
const int stayOpen = 2;
const int closeDoor = 3;
const int reOpen = 4;
const int lockDoor = 5;

volatile long distance; //ULTRASONIC DISTANCE + INTERRUPT VARIABLES
volatile long startTime;
volatile long endTime;

/////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(lockPin, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), risingEdge, RISING); //ATTACH INTERRUPT PINS TO ULTRASONIC ECHO PIN
  attachInterrupt(digitalPinToInterrupt(2), fallingEdge, FALLING);

  stepper.setSpeed(300); //STEPPER MOTOR SPEED

  lockValue = 0;
  
  STATE = idle; //INITIAL STATE

}

/////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  digitalWrite(trigPin, LOW); //SETTING ULTRASONIC TRIGGER TO DETECT DISTANCE DURATION
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  distance = pulseIn(echoPin, HIGH); //DISTANCE = DISTANCE DURATION

  pirValue = digitalRead(pirPin);
  buttonState = digitalRead(lockPin);

  if ( buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      lockValue = !lockValue;
    }
    else {
      //nothing
    }
    delay(50);
  }

  lastButtonState = buttonState;

  doorState(); //STATE MACHINE

  Serial.println(distance); //TESTING DISTANCE
}

/////////////////////////////////////////////////////////////////////////////////////////
void doorState() { //STATE MACHINE

  switch (STATE) { //CHANGE STATES

    case idle:
      if (lockValue == HIGH)
        STATE = lockDoor;
      else if (pirValue == HIGH)
        STATE = openDoor;
      else
        STATE = idle;
      break;

    case openDoor:
      if (pirValue == HIGH)
        STATE = stayOpen;
      else
        STATE = closeDoor;
      break;

    case stayOpen:
      if (pirValue == HIGH)
        STATE = stayOpen;
      else
        STATE = closeDoor;
      break;

    case closeDoor:
      if (distance > 550 && startRev < STEPS_TO_END) //500 original distance
        STATE = closeDoor;
      else if (startRev >= STEPS_TO_END)
        STATE = idle;
      else {
        STATE = reOpen;

        //BUZZ WHEN ULTRASONIC DETECTS SOMETHING UNDER 500
        tone(buzzer, 1000); //1 kHz SOUND SIGNAL
        delay(500); //PLAY FOR THIS LONG
        noTone(buzzer); //STOP SOUND
        //delay(100); //STOP BUZZER
      }
      break;

    case reOpen:
      if (pirValue == HIGH)
        STATE = stayOpen;
      else if (distance < 550)
        STATE = reOpen;
      else
        STATE = closeDoor;
      break;

    case lockDoor:
      if (lockValue == HIGH)
        STATE = lockDoor;
      else
        STATE = idle;
      break;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  switch (STATE) { //STATE FUNCTIONS

    case idle:
      startRev = 0;
      delay(200);
      break;

    case openDoor:
      Steps2Take  =  STEPS_PER_MOTOR_REVOLUTION * STEPS_TO_END ;
      stepper.step(Steps2Take);
      delay(1000);
      startRev = 0;
      break;

    case stayOpen:
      delay(200);
      break;

    case closeDoor:
      if (startRev == 0)
        Steps2Take = 0;
      else
        Steps2Take  = - STEPS_PER_MOTOR_REVOLUTION;
      stepper.step(Steps2Take);
      startRev++;
      break;

    case reOpen:
      Steps2Take  = STEPS_PER_MOTOR_REVOLUTION * startRev;
      stepper.step(Steps2Take);
      startRev = 0;
      delay(200);
      break;

    case lockDoor:
      delay(200);
      break;

  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void risingEdge() { //INTERRUPT
  endTime = 0;
  startTime = millis();
}

/////////////////////////////////////////////////////////////////////////////////////////
void fallingEdge() { //INTERRUPT
  endTime = millis();
  distance = endTime - startTime;
}

