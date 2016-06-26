/**
  Receive serial-formatted IR pulses, while scanning a stepper motor left and right.

  Designed for Arduino Nano, reporting data up to an Arduino Mega
  over the serial port.

  Dr. Orion Lawlor, lawlor@alaska.edu, 2016-02-02
  This file is Public Domain.
*/
#include "IRserial.h"
#include "AccelStepper.h"
#include "serial_packet.h"
#include "robot.h"

bool serial_debug_ASCII=true; // human readable reports

bool serial_binary_reports=true; // binary A-packet reports
A_packet_formatter<HardwareSerial> serial_binary(Serial);

typedef unsigned short milli_t;



enum {xmit_nout=3};
int xmit_pattern[xmit_nout]={0xA,0xB,0xC};
long xmit_tot_steps[xmit_nout];
int xmit_cnt[xmit_nout];
int xmit_cnt_last[xmit_nout];

void xmit_clear(void) {
  for (int p=0;p<xmit_nout;p++) {
    xmit_tot_steps[p]=0;
    xmit_cnt[p]=0;
    xmit_cnt_last[p]=0;
  }
}

void xmit_check(void) {
  for (int p=0;p<xmit_nout;p++) 
  {
    int nlast=xmit_cnt[p]-xmit_cnt_last[p];
    if (xmit_cnt[p]>0) 
    { // we once saw it
      if (nlast==0) { // but we're done seeing it
        // Send it off
        long cen_angle=xmit_tot_steps[p]/xmit_cnt[p];
        report_detection(p,cen_angle,xmit_cnt[p]);

        // Reset for next detection
        xmit_tot_steps[p]=0;
        xmit_cnt[p]=0;
      }
    }

    // Copy over counts so we know how many happen at each check
    xmit_cnt_last[p]=xmit_cnt[p];
  }
}

void report_detection(int xmit,int angle,int how_many)
{
	if (serial_debug_ASCII) {
		const char *blinkdesc[4]={"A","B","C","K"};
		Serial.print("blinky	");
   if (xmit<4) {
		Serial.print(blinkdesc[xmit]);
   } else Serial.print(xmit);
		Serial.print("	");
		Serial.print(angle);
		Serial.print("	");
		Serial.print(how_many);
    Serial.print("  ");
    Serial.print(millis());
		Serial.println("	");
	}

	if (serial_binary_reports && how_many>1) {
		// Send binary data report up to mega:
		robot_blinky_update b;
		b.angle=angle;
		b.many=how_many;
		b.side=0; // <- overwritten by mega, same firmware for both sides
		b.millitime=0; // <- timestamped by mega
		b.xmit=xmit;
		serial_binary.write_packet(0xB,sizeof(b),&b);
	}
}



int backlash;
long cur_stepper_step=-1;
void IRrecv_message(int msg_rx)
{
  digitalWrite(13,HIGH); // blink on incoming data
//  Serial.print("Angle ");
//  Serial.print(cur_stepper_step*360/4096.0);
  int xmit=-1;
  for (int p=0;p<xmit_nout;p++) {
    if (xmit_pattern[p]==msg_rx) xmit=p;
  }
  if (xmit>=0) { // saw an emitter
    int angle=cur_stepper_step+backlash;
    report_detection(xmit,angle,1); // report immediately
    xmit_tot_steps[xmit]+=angle;
    xmit_cnt[xmit]++;
  }
}


class IRrecv_stepper : public AccelStepper {
public:
  milli_t last_check_milli;
  IRrecv_stepper() 
    :AccelStepper(AccelStepper::HALF4WIRE, 6,8,7,9) // was: 8,10,9,11);
  {
    last_check_milli=(milli_t)millis();
  }
  virtual void step(long cur_step) {
    AccelStepper::step(cur_step); // move hardware

    if (cur_step<0) return; // startup only
    
    cur_stepper_step=cur_step; // record shaft position
    IRrecv_poll<8>(); // check for IR messages so far
    //Serial.print("s");
    
    milli_t cur=(milli_t) millis();
    if ((cur-last_check_milli)>=12)
    {
      xmit_check();
      last_check_milli=cur;
    }
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
  if (serial_debug_ASCII) Serial.println("Reading IR pulses");
  pinMode(13,OUTPUT);

  IRrecv_setup();
  IRsend_setup();  
  
  // Fast enough to use (at 5V from Arduino USB)
  //stepper.setMaxSpeed(1500.0);  stepper.setAcceleration(4000.0);
  //stepper.setMaxSpeed(1000.0);  stepper.setAcceleration(2000.0);
  stepper.setMaxSpeed(500.0);  stepper.setAcceleration(2000.0);


  // Seek back until stepper is at its hard stop:
  if (serial_debug_ASCII) Serial.println("Running stepper back to hard stop:");
  stepper.runToNewPosition(-step_max_normal);
  stepper.setCurrentPosition(0);
}



int test_good=0;
int test_bad=0;
int test_count=-1;

float up_avg=0.0, dn_avg=0.0;

void loop() {  
    backlash=20;
    step_target+=step_dir*5000;
    if (step_target>=step_max_normal) {
        step_target=step_max_normal;
        step_dir=-1;
        backlash=-backlash;
    }
    if (step_target<=step_min_normal) {
      step_target=step_min_normal;
      step_dir=1;
    }

    xmit_clear();
    
    stepper.runToNewPosition(step_target);
    digitalWrite(13,LOW);

    report_detection(3,0,0); // report seek back to start
}


