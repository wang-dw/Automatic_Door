#include <Stepper.h>

#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64  //2048

Stepper small_stepper(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);

int  Steps2Take;

int pirPin = 13;
int pirValue;

void setup() {

  pinMode(pirPin, INPUT);

  pirValue = digitalRead(pirPin);
  
}

void loop() {
  if(pirValue == HIGH){
  Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION ;
  small_stepper.setSpeed(1000);   
  small_stepper.step(Steps2Take);

}
  else{
  Steps2Take  =  - STEPS_PER_OUTPUT_REVOLUTION;
  small_stepper.setSpeed(1000);
  small_stepper.step(Steps2Take);

  }
}
