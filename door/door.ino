#include <Stepper.h>

#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64  //2048

Stepper small_stepper(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);

int  Steps2Take;

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
  STATE = idle;
}

void loop() {

  pirValue = digitalRead(pirPin);
  lockValue = digitalRead(lockPin);
  doorValue = 0;

  doorState();

  //delay(1000);
}

void doorState()
{
  switch (STATE) //CHANGE STATES
  {
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

  switch (STATE) //STATE FUNCTIONS
  {
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

