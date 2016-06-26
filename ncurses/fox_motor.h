
/**
LunAlaska motor control header:
	Call motors_setup once at startup.
	Write motor power commands to the "Power" global variables.
	Call motors_send to communicate those values to the motors
	(at least several times per second; at most 100 times per second)

Dr. Orion Lawlor's refactoring of Noah Betzen's code.
2013-05-16 (Public Domain)
Arsh Chauhan's addition of arduino_reconnect() function.***Untested
2014-01-27 (Public Domain)
*/

static int timeUI = 0 ;
static int dataTime = 0;
int batteryVoltage=0;
int mineVoltage=0;
void arduino_reconnect();

float limit(float cur,float lo,float hi) //keep value in between lo and hi
{
	if (cur<lo) return lo;
	if (cur>hi) return hi;
	return cur;
}

// Slowly change value to approach target at rate proportional to frac
void smooth(float &value,float target,float frac) {
	value=value*(1.0-frac)+target*frac;
}

//NOTE: Won't work on Mac, everything relating to serial.h needs to be commented out to build

#include "serial.h"

void motors_setup()
{
	printf("Connecting to Arduino...\n");
	while((Serial.begin(57600) == -1)) // serial port to Arduino
	{
		printf("Attempting to connect\n");
		usleep(100*1000); //100ms delay
	}
	
	int r=0;
	while (-1==(r=Serial.read()))
    { // wait for Arduino to start up
		// keep waiting
	}
	printf("Arduino connected: %02x.\n",r);
	timeUI=dataTime=0;
}

/* These are the current power levels for each motor. */
    float leftPower=0;
    float rightPower=0;
    float frontPower=0;
    float minePower=0;
    float dumpPower=0;

    int arduinoStatus=0;
    enum {nPowers=5};
/* Send the current power levels (above) 
   off to the motors. */


void motors_send() {
	
	unsigned char powers[nPowers];	

        powers[0] = (64+limit(leftPower*63,-63,+63));
        powers[1] = (64+limit(-rightPower*63,-63,+63));
        powers[2] = (64+limit(frontPower*63,-63,+63));
		powers[3] = (64+limit(minePower*63,-63,+63));
        powers[4] = (64+limit(dumpPower*63,-63,+63));
        
        Serial.write(0xA5);
        for(int i=0; i<nPowers; i++){
		Serial.write(powers[i]);
	}
	arduinoStatus = -1;
	arduinoStatus = Serial.read();
	batteryVoltage=Serial.read();
	mineVoltage=Serial.read();
	
	
	if(arduinoStatus != -1)
		
	   {	
		dataTime = timeUI;
	    }	
	else
	//if(arduinoStatus ==-1)
	{
		if( (timeUI - dataTime) > 10)
		{
					
			printw("Connection Lost \n");
			refresh();
			arduino_reconnect();
		}
	}
	
}
/* Arsh Chauhan's refactoring of Noah Betzen's Code.
   Software Hack to deal with weird serial disconnection issue when 
   linear actuator reaches end of travel 
*/ 

void arduino_reconnect()
{
	
	motors_setup();
}
