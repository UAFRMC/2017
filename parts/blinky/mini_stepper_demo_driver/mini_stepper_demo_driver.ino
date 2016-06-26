/*
 Use acceleration to drive a 28BYJ geared stepper motor
    Pins 8-11 are straight through to driver board.
*/

#include "AccelStepper.h"

int REAL_SLOW=0;

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::HALF4WIRE, 8,10,9,11);

void setup()
{  
   if (REAL_SLOW) {
  //Slow enough to watch:
     stepper.setMaxSpeed(150.0);  stepper.setAcceleration(1.0);
   } else {
   // Fast enough to use (at 5V from Arduino USB)
     stepper.setMaxSpeed(1000.0);  stepper.setAcceleration(2000.0);
  // (at 8V or 12V supply)
    // stepper.setMaxSpeed(3000.0);  stepper.setAcceleration(100000.0);
   }

    Serial.begin(57600);
    Serial.println("Started up!");
}

void toPos(int p)
{
  Serial.print("Seeking to "); Serial.println(p);
  stepper.runToNewPosition(p);
}

void loop()
{
  toPos(0);
  if (REAL_SLOW) {
    toPos(200);
  }
  else {
    toPos(2037);
  }
//    toPos(2048);
}

