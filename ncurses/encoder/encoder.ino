/* Original Code: http://playground.arduino.cc/Main/RotaryEncoders#Intro  (Public Domain)
   Arsh Chauhan's refactoring to blink LED's to show direction of motion
   LED on pin 8: clockwise
   LED on pin 9: counter-clockwise
 */ 


 #define encoder0PinA 2

#define encoder0PinB 3

volatile unsigned int encoder0Pos = 0;
volatile unsigned int encoderPrevious = 0;
int greenLed = 8;
int redLed = 9;
void setup() {

  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT); 

// encoder pin on interrupt 0 (pin 2)

  attachInterrupt(0, doEncoderA, CHANGE);

// encoder pin on interrupt 1 (pin 3)

  attachInterrupt(1, doEncoderB, CHANGE);  
  
  pinMode(greenLed,OUTPUT); // blinks if moving forward
  pinMode(redLed, OUTPUT); // blinks of moving in reverse

  Serial.begin (9600); // only used for debugging (original author)
}

void loop(){ 
  
  if(encoder0Pos > encoderPrevious)
  {
    digitalWrite(greenLed, LOW);
    digitalWrite(greenLed, HIGH);
    delay(1000);
    digitalWrite(greenLed, LOW);
  }
  if(encoder0Pos < encoderPrevious)
  {
    digitalWrite(redLed, LOW);
    digitalWrite(redLed, HIGH);
    delay(1000);
    digitalWrite(redLed, LOW);
  }
  // no need for a loop  
}


void doEncoderA(){

  // look for a low-to-high on channel A
  if (digitalRead(encoder0PinA) == HIGH) { 

    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == LOW) {  
      encoderPrevious = encoder0Pos;
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoderPrevious = encoder0Pos;
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  else   // must be a high-to-low edge on channel A                                       
  { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinB) == HIGH) {  
      encoderPrevious = encoder0Pos; 
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoderPrevious = encoder0Pos;
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
  Serial.println (encoder0Pos, DEC);          
  // use for debugging - remember to comment out

}

void doEncoderB(){

  // look for a low-to-high on channel B
  if (digitalRead(encoder0PinB) == HIGH) {   

   // check channel A to see which way encoder is turning
    if (digitalRead(encoder0PinA) == HIGH) { 
      encoderPrevious = encoder0Pos; 
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoderPrevious = encoder0Pos;
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  // Look for a high-to-low on channel B

  else { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinA) == LOW) { 
      encoderPrevious = encoder0Pos;  
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoderPrevious = encoder0Pos;
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }

} 
