/**
  Blink LEDs, in timed order.
    pin 3 blinks with period of 3 milliseconds,
    pin 4 blinks with period of 4 milliseconds,
    ...
    pin 12 blinks with period of 12 milliseconds.

 The idea is you just plug the amplifier circuit into the appropriate pin.

 Pins 2 and 13 are slower blinking, for debugging.

 Dr. Orion Lawlor, lawlor@alaska.edu, 2014-03 (Public Domain)
*/

void setup(void) {
  for (int p=2;p<=13;p++) {
     pinMode(p,OUTPUT);
  }
}

void loop(void) {
  unsigned long t=micros()/1000;

  // The following just runs continually, although it only needs to run 
  //  once for each t value.
  // micros() does the silly leap millisecond thing every 43ms
  // delay(1) would be a bad idea here (the p loop takes too much time); 
  // a spinloop to wait until t>=old_t+1 might be OK.
  
  for (int p=2;p<=13;p++) {
     int period=p; // blink period, in milliseconds: pin p blinks every p milliseconds
     if (p==2) period=4000; // easy debug pin: 2 sec on, 2 sec off.
     if (p==13) period=1000; // blinking lights
     int on=period*4/8; // on-time
     int phase=t%period;
     if (phase<on) digitalWrite(p,HIGH);
     else digitalWrite(p,LOW);
  }
}


