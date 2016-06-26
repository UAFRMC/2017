/* Infrared blinker receiver demo:
     Takes modulated infrared input via A0, 
     does signal processing, writes result to serial port.

Dr. 
*/

void setup() {
  Serial.begin(57600);
  pinMode(12,OUTPUT);
}

bool phase=false;
void loop() {
   
   int lo=1025, hi=-1; // simple range output
   
   unsigned int phase4=0, phase5=0;
   long sum4_real=0, sum4_imag=0; // unscaled integers
   long sum5_real=0, sum5_imag=0; // scaled to fixed point math
 #define scl 32 /// scale factor for sum5 fixed point 
 
   long last=micros();
   for (int reps=0;reps<10;reps++) { // take 100 samples (multiple of both 4 and 5)
   
     if ((reps%5)==0) { // blink LED
       phase=!phase;
       digitalWrite(12,phase?HIGH:LOW);
     }
     
     // Wait 1 millisecond between samples (skewless version)
     while (micros()<last+1000) {}
     last+=1000; 
     
     int v=analogRead(A0);
     if (lo>v) lo=v;
     if (hi<v) hi=v;

  // Add to period-4 helix     
     phase4++; if (phase4>=4) phase4=0;
     switch (phase4) {
       case 0: sum4_real+=v; break;
       case 1: sum4_imag+=v; break;
       case 2: sum4_real-=v; break;
       case 3: sum4_imag-=v; break;
     }
     
  // Add to period-5 helix, with fixed-point scale factors
     phase5++; if (phase5>=5) phase5=0;
     switch (phase5) {
       case 0: sum5_real+=int(scl)*v; break;
       case 1: sum5_real+=int(0.309*scl)*v; sum5_imag+=int(0.951*scl)*v; break; // 72 deg
       case 2: sum5_real+=int(-0.809*scl)*v; sum5_imag+=int(0.587*scl)*v; break; // 144 deg
       case 3: sum5_real+=int(-0.809*scl)*v; sum5_imag+=int(-0.587*scl)*v; break;
       case 4: sum5_real+=int(0.309*scl)*v; sum5_imag+=int(-0.95*scl)*v; break;
     }
   }
   Serial.print(lo);
   Serial.print("  min  ");
   Serial.print(hi);
   Serial.print("  max  ");
   
   long mag4=sum4_real*sum4_real+sum4_imag*sum4_imag;
   Serial.print(sqrt(mag4));
   Serial.print(" amp4  ");
   
   long mag5=sum5_real*sum5_real+sum5_imag*sum5_imag;
   Serial.print(sqrt(mag5)*(1.0/scl));
   Serial.println(" A0");
}

