/**
  Emit serial data as IR pulses.

  Dr. Orion Lawlor, lawlor@alaska.edu, 2016-02-02
  This file is Public Domain.
*/
#include "IRserial.h"


void setup()
{
	Serial.begin(57600);
	Serial.println("Sending IR pulses");
	pinMode(13,OUTPUT);

	IRsend_setup();  
}



// Actual sent message:
int message=0x5A;
void loop() {
  digitalWrite(13,HIGH);
	IRsend_message<8>(message);
  digitalWrite(13,LOW);
  if ((message&0xf)==0) {
    Serial.println(message);
  }
	
	delay(1); // delay between each signal burst
	
	message=(message+1)&0xff;
}

