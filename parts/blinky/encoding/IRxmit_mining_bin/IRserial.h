/**

  Arduino Infrared Serial data I/O library:
    Send infrared using software modulation,
    receive using a TSOP382 IR demodulator chip.
 
 Typical usage:
 
 void setup() {
	IRxmit_setup();
	IRrecv_setup();
 }
 
 void loop() {
 	IRxmit_message<8>(0x37);
 	IRrecv_poll<8>(); // calls IRrecv_message
 }
 
 void IRrecv_message(int message) {
     // handle binary message data
 }
*/
#ifndef __IRSERIAL_H
#define __IRSERIAL_H

#include "notPinChangeInt.h"


// This is the user's function, called by IRrecv_poll when IR data arrives.
extern void IRrecv_message(int message);


// Pin connected to TSOP IR detector chip's output line:
//    must be an interrupt-on-change pin.
int IRrecv_pin=8;
int IRrecv_gnd=9;
int IRrecv_power=10;

/* Raw interrupt-driven bit receiver timing code: */
enum {IRrecv_change_mask=31}; // wrap around in change array (power of 2, minus 1)
enum {IRrecv_change_len=IRrecv_change_mask+1}; // length of change array (power of 2)
typedef unsigned short IRrecv_time_type;
IRrecv_time_type IRrecv_change_times[IRrecv_change_len];
volatile unsigned char IRrecv_change_write=0; // next time to write (updated by change interrupt)
unsigned char IRrecv_change_read=0; // next time to read (updated by IRrecv_poll)

/* INTERRUPT on change of IR sensor's output pin */
void IRrecv_change() {
  unsigned char c=IRrecv_change_write;
  IRrecv_change_times[c]=(IRrecv_time_type)micros();
  IRrecv_change_write=(c+1)&IRrecv_change_mask;
}

// This callback is called for each received message.
// extern void IRrecv_message(IRrecv_message_type message);

// Set bit timings: lower numbers are faster, but less reliable below 250 us
const int IRrecv_bit_time=256; // time per bit, in microseconds
const int IRrecv_bit_time_slop=110; // allow this much variation in time

int IRrecv_bit_error_count=0;
int IRrecv_bit_error_number=0;

/* Read data stored by interrupts to extract this many bits of serial data.
 If a message arrives, calls your function IRrecv_message(message);
*/
template <int message_bits>
void IRrecv_poll(void) {
  IRrecv_time_type now=(IRrecv_time_type)micros();
  const IRrecv_time_type message_time=(1+message_bits)*IRrecv_bit_time+IRrecv_bit_time_slop; // maximum message time

  unsigned char w=IRrecv_change_write; // atomic grab of write pointer
  unsigned char r=IRrecv_change_read; // our read pointer
  while (
       ((w-r)&IRrecv_change_mask)>=2 // we have at least two changes
    && (IRrecv_time_type)(now-IRrecv_change_times[r])>=message_time // time enough for a full message
  ) 
  { // try to decode the message:
    IRrecv_time_type lastbit=IRrecv_change_times[r]; // last bit start time
    r=(r+1)&IRrecv_change_mask;
    int value=1; // start bit (FIXME: doublecheck against actual value)
    int message=0;
    for (int databit=0;databit<=message_bits;databit++) {
      if ((((w-r)&IRrecv_change_mask)>0) // next time is still valid
        && (IRrecv_time_type)(IRrecv_change_times[r]-lastbit)<IRrecv_bit_time+IRrecv_bit_time_slop) 
      { // this bit has changed
        if ((IRrecv_time_type)(IRrecv_change_times[r]-lastbit)>IRrecv_bit_time-IRrecv_bit_time_slop) 
        {
          lastbit=IRrecv_change_times[r]; // update change time 
          value=!value; // flip the bit value
          r=(r+1)&IRrecv_change_mask;
        } else { // bad bit time--not within time slop
          IRrecv_bit_error_count++;
          IRrecv_bit_error_number=databit;
          r=(r+1)&IRrecv_change_mask;
          break; // outta here
        }
      } 
      else 
      { // bit is unchanged--assume nominal bit time
        lastbit+=IRrecv_bit_time;
      }
  
      
      if (databit==message_bits) 
      { // Message is now complete
        if (value!=0) { // did not end with stop bit
          IRrecv_bit_error_count++;
          IRrecv_bit_error_number=databit;
          break; // outta here
        }
        else {
          IRrecv_message(message);
        }
      }

      // Add this bit to our message
      message|=(value<<databit);
    }
  }
  IRrecv_change_read=r;
}


// For debugging, dump last times stored by interrupt function:
void IRrecv_dump(void) {
  /*
  Serial.print("   Current time (us):");
  IRrecv_time_type now=micros();
  Serial.print(now);
  */
  int w=IRrecv_change_write;
  /*
  Serial.println();
  Serial.print("   Last 10 change times (us):");
  for (int i=0;i<10;i++) {
    Serial.print(now-IRrecv_change_times[(-i+IRrecv_change_len)&IRrecv_change_mask]);
    Serial.print(" ");
  }
  */
  Serial.println();
  Serial.print("  Last 10 dels (us):");
  for (int i=1;i<=10;i++) {
    Serial.print(IRrecv_change_times[(w-i+IRrecv_change_len)&IRrecv_change_mask]-IRrecv_change_times[(w-i-1+IRrecv_change_len)&IRrecv_change_mask]);
    Serial.print(" ");
  }
  Serial.println();
}




// Call at startup to start receiving IR messages
void IRrecv_setup() {
  // Add soft power, so you can plug the sensor in directly:
  pinMode(IRrecv_pin,INPUT_PULLUP);
  pinMode(IRrecv_gnd,OUTPUT); digitalWrite(IRrecv_gnd,LOW);
  pinMode(IRrecv_power,OUTPUT); digitalWrite(IRrecv_power,HIGH);

  attachPinChangeInterrupt(IRrecv_pin,IRrecv_change,CHANGE);
}


/***************** IRsend part of library ********************/
#include "notIRremote.h"


#if defined(__AVR_ATmega2560__) // Mega
int IRgnd=8; // Soft ground pin for IR LED
int IRsend_pin=9; // IR drive pin

#elif defined(__AVR_ATmega328P__) // uno/nano
int IRgnd=2; // Soft ground pin for IR LED
int IRsend_pin=3; // IR drive pin

#else
# error "IRserial.h can't figure out timer pin: see IRremoteInt for what timer & pin your board uses"

#endif

// This is the class that actually sends IR pulses
IRsend irsend;

// Call at startup to enable transmission of IR messages
void IRsend_setup(int kHz=38,int duty=256/3) {	
  irsend.enableIROut(kHz,duty); // run at this kHz and duty cycle

  pinMode(IRgnd,OUTPUT); digitalWrite(IRgnd,LOW); // soft ground
  pinMode(IRsend_pin,OUTPUT); digitalWrite(IRsend_pin,LOW); // LED output pin
}


// Send this byte of data as IR blinks
template <int message_bits>
void IRsend_message(int v) {
 #if 0
  // Library-based irsend version
  irsend.mark(IRrecv_bit_time); // start bit
  for (int bit=0;bit<=message_bits;bit++) {
    if (v&1) { irsend.mark(IRrecv_bit_time); } 
    else { irsend.space(IRrecv_bit_time); }
    v=v>>1; // down to next bit
  }
  irsend.space(IRrecv_bit_time); // stop bit
 
 #else
  // manual timer version (goal is less skew, not much different though!)
  int send=1; // start bit is always high
  IRrecv_time_type next_time=micros();
  for (int bit=0;bit<1+message_bits+1;bit++) {
    if (send) { TIMER_ENABLE_PWM; } else { TIMER_DISABLE_PWM; }
    send=v&1;
    v=v>>1; // down to next bit

    // wait until next bit time:
    next_time+=IRrecv_bit_time;
    while ((int)(next_time-micros())>0) {} // busywait
  }
  { TIMER_DISABLE_PWM; }  // turn off at output (stop bit should have done it, but be sure!)

 #endif
}


#endif

