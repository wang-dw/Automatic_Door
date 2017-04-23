#include <Stepper.h>
#include <elapsedMillis.h>

#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64  //2048

Stepper small_stepper(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);

int  Steps2Take;

const int trigPin = 6;
const int echoPin = 7;

int lockPin = 12;
int pirPin = 13;
int pirValue;
int lockValue;
int doorValue;

int STATE;

const int idle = 0;
const int openDoor = 1;
const int stayOpen = 2;
const int closeDoor = 3;
const int lockDoor = 4;

void setup() {

  pinMode(pirPin, INPUT);
  pinMode(lockPin, INPUT);
  pinMode(echoPin, INPUT);
  STATE = idle;

  Serial.begin(9600);
}

void loop() {

  long duration, inches, cm;

  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  duration = pulseIn(echoPin, HIGH);

  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  if (duration < 23200) { //23200 = 4m
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
  }
  else {
    Serial.print("Out of Range");
  }

  pirValue = digitalRead(pirPin);
  lockValue = digitalRead(lockPin);
  doorValue = 0;

  doorState();

  delay(200);
}

void doorState() {

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
      if (pirValue == HIGH)
        STATE = openDoor;
      else
        STATE = idle;
      break;

    case lockDoor:
      if (lockValue == HIGH)
        STATE = lockDoor;
      else
        STATE = idle;
      break;
  }

  switch (STATE) { //STATE FUNCTIONS

    case idle:
      delay(200);
      break;

    case openDoor:
      Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION ;
      small_stepper.setSpeed(1000);
      small_stepper.step(Steps2Take);
      delay(1000);
      break;

    case stayOpen:
      delay(200);
      break;

    case closeDoor:
      Steps2Take  =  - STEPS_PER_OUTPUT_REVOLUTION;
      small_stepper.setSpeed(1000);
      small_stepper.step(Steps2Take);
      break;

    case lockDoor:
      delay(200);
      break;

  }

}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

