/**
  Receive serial-formatted IR pulses, while scanning a stepper motor left and right.

  Dr. Orion Lawlor, lawlor@alaska.edu, 2016-02-02
  This file is Public Domain.
*/
#include "IRserial.h"
#include "AccelStepper.h"

long cur_stepper_step=-1;
long tot_stepper=0;
long cnt_stepper=0;
void IRrecv_message(int msg_rx)
{
  digitalWrite(13,HIGH); // blink on incoming data
//  Serial.print("Angle ");
//  Serial.print(cur_stepper_step*360/4096.0);
  Serial.print("  step ");
  Serial.print(cur_stepper_step);
  Serial.print(" byte ");
  Serial.println(msg_rx,HEX);
  
  tot_stepper+=cur_stepper_step; cnt_stepper++;
}


class IRrecv_stepper : public AccelStepper {
public:
  IRrecv_stepper() 
    :AccelStepper(AccelStepper::HALF4WIRE, 6,8,7,9) // was: 8,10,9,11);
  {}
  virtual void step(long cur_step) {
    AccelStepper::step(cur_step); // move hardware
    cur_stepper_step=cur_step; // record shaft position
    IRrecv_poll<8>(); // check for IR messages so far
    //Serial.print("s");
  }
};

// Define our stepper
IRrecv_stepper stepper; 

// Stepper's current position target
int step_target=0;
int step_dir=1;
int step_max_normal=3800; // hits endstop other way
int step_min_normal=100; // avoid endstop area

void setup()
{
  Serial.begin(57600);
  Serial.println("Reading IR pulses");
  pinMode(13,OUTPUT);

  IRrecv_setup();
  IRsend_setup();  
  
  // Fast enough to use (at 5V from Arduino USB)
  stepper.setMaxSpeed(1000.0);  stepper.setAcceleration(2000.0);

  // Seek back until stepper is at its hard stop:
  Serial.println("Running stepper back to hard stop:");
  stepper.runToNewPosition(-step_max_normal);
  stepper.setCurrentPosition(0);
}



int test_good=0;
int test_bad=0;
int test_count=-1;

float up_avg=0.0, dn_avg=0.0;

void loop() {  
    step_target+=step_dir*5000;
    if (step_target>=step_max_normal) {
        step_target=step_max_normal;
        step_dir=-1;
    }
    if (step_target<=step_min_normal) {
      step_target=step_min_normal;
      step_dir=1;
    }
    if (Serial.available()) { // allow manual motion
      int i=Serial.parseInt();
      if (i!=0) step_target=i;
    }
    
    if (true) { // test_count++>=0) {
      Serial.println();
      Serial.print("Stepping to ");
      Serial.print(step_target);
      Serial.print(": ");
      test_count=test_good=test_bad=0;
    }
    stepper.runToNewPosition(step_target);
    digitalWrite(13,LOW);

// Update average detected positions (assumes only one emitter)
   float avg=tot_stepper*1.0/cnt_stepper;
    tot_stepper=cnt_stepper=0;
   if (step_target<step_max_normal/2) dn_avg=avg;
   else {
     up_avg=avg;
     Serial.print("Average detection: ");
     Serial.print(up_avg);
     Serial.print(" ");
     Serial.print(dn_avg);
     Serial.println();
   }


    delay(5); // delay between each manual step
}

