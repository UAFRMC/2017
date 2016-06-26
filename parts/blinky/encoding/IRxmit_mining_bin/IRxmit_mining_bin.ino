/**
  Emit serial data as IR pulses.

  Dr. Orion Lawlor, lawlor@alaska.edu, 2016-02-02
  This file is Public Domain.
*/
#include "IRserial.h"

int pin_common=3;
enum {pin_nout=3};
int pin_out[pin_nout]={6,4,5}; // Left, Middle, Right
int pin_pattern[pin_nout]={0xA,0xB,0xC};

void setup()
{
	Serial.begin(57600);
	Serial.println("IRXmit_mining_bin transmitter, 2016-05-07");
	pinMode(13,OUTPUT);
  pinMode(pin_common,OUTPUT); // N channel, active high
  for (int p=0;p<pin_nout;p++) {
    pinMode(pin_out[p],OUTPUT); 
    digitalWrite(pin_out[p],HIGH); // P channel active low
    Serial.println(pin_pattern[p],HEX);
  }

	IRsend_setup();  
}



void loop() {
  digitalWrite(13,HIGH);
  for (int p=0;p<pin_nout;p++) {
    digitalWrite(pin_out[p],LOW); // on (if pin 3 is on too)
	  IRsend_message<4>(pin_pattern[p]);
    digitalWrite(pin_out[p],HIGH); // off again
  }
  digitalWrite(13,LOW);
	//delay(1); // delay between each signal burst
}

