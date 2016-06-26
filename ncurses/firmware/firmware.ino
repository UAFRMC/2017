

// All sabertooth commands leave via this port
HardwareSerial &masterPort=Serial1;//TX1

// These MUST match the sabertooth DIP address pins!
const int leftAddr=128;
const int rightAddr=128;
const int frontAddr=129;
const int mineAddr=130;

const int motor1=7; // command motor 1
const int motor2=6; // command motor 2 (note: backwards of documentation!)

int batteryVoltage=0;
int mineVoltage=0;


int arduinoStatus=0xA0;  // A-OK!


void setup()
{
  Serial.begin(57600); // Control connection to PC
  
  masterPort.begin(9600); // <- mabye try 38400 for faster updates?
  delay(2000); // Sabertooth docs: "allow a two-second delay between applying power and..."
  masterPort.write(0xAA); // "first character must be 170 (AA in hex)"
  delay(10); // <- needed?

 
  
  Serial.write(arduinoStatus); // Say I'm A-OK (needed so PC knows we're ready)
 
 // Our ONE debug LED!
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

// Last-known motor power commands.  Sent from PC
// 1- turn backwards
// 64- do not turn
// 127- turn forwards
int leftPower=64; // drive wheel power
int rightPower=64;

int frontPower=64; // raise/lower front wheels

int minePower=64; // mining conveyor run
int dumpPower=64; // raise mining head and dump storage


void sendMotor(int addr, int command, int val){
  masterPort.write(addr);
  masterPort.write(command);
  masterPort.write(val);
  masterPort.write((addr+command+val)&127);
}

int blink=0;

void loop()
{
  int config,power;
  batteryVoltage = analogRead(A10); // Enter pin before compiling
  mineVoltage = analogRead(A12);// Enter pin before compiling
  
  config = Serial.read(); // used for error checking 
  while (config!=-1)
  {  // actual data available!
    
    //what if Serial.read() gets a power reading of 0xA3 and mistakes it for config?
    {if(config==0xA5) //drive
    {
      leftPower=Serial.read();
      rightPower=Serial.read();
      frontPower=Serial.read();
      minePower=Serial.read();
      dumpPower=Serial.read();
    
      Serial.write(arduinoStatus); // let PC know our status
      Serial.write(batteryVoltage); 
      Serial.write(mineVoltage);
      digitalWrite(13,!blink); //if good input received, blink!
      blink=!blink;
      

    }
  }
 if(config==-1)
  {
     leftPower=rightPower=frontPower=minePower=0;
      Serial.write(arduinoStatus); // let PC know our status
      Serial.write(batteryVoltage);
      Serial.write(mineVoltage);
      digitalWrite(13,!blink); //if good input received, blink!
      blink=!blink;
  } 
       
}


  
  sendMotor(leftAddr,motor1,leftPower);
  sendMotor(rightAddr,motor2,rightPower);
  
  sendMotor(frontAddr,motor1,frontPower);
  
  sendMotor(mineAddr,motor1,minePower);
  sendMotor(mineAddr,motor2,dumpPower);
}
