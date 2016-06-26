HardwareSerial &minePort=Serial1; //conveyor and arms
HardwareSerial &wheelPort=Serial2; //left and right wheels
HardwareSerial &armPort=Serial3; //left and right wheels

int conveyorSwitch=10;
int leftSwitch=11;
int rightSwitch=12;

int tickReport=0;

int tick=0;
int ptick=0;

const int leftAddr = 128;
const int rightAddr = 129;
const int conAddr = 128;
const int armAddr = 129;

void setup()
{
  Serial.begin(57600); // Control connection to PC
  Serial.write(0xA0); // Say I'm A-OK (needed so PC knows we're ready)
  
  wheelPort.begin(9600); // This is the baud rate you chose with the DIP switches.             
  //wheelPort.write(170);
  minePort.begin(9600); // This is the baud rate you chose with the DIP switches.
  armPort.begin(9600); // This is the baud rate you chose with the DIP switches.  
  //minePort.write(170);
 // Our ONE debug LED!
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  
  pinMode(conveyorSwitch, INPUT);
  pinMode(leftSwitch, INPUT);
  pinMode(rightSwitch, INPUT);
}

// Last-known motor power commands.  Sent from PC
// 1- turn backwards
// 64- do not turn
// 127- turn forwards
int leftPower=64;
int rightPower=64;
int leftArmPower=64;
int rightArmPower=64;
int conveyorPower=64;
int runPower=64;

int config;
int power;
int state=0;

void loop()
{
  while ((config=Serial.read())!=-1)
  {  // actual data available!
    ptick=tick;
    tick=digitalRead(conveyorSwitch);
    if(tick!=ptick) tickReport=1;  
    switch(state){
      default: case 0:
       if(config==0xA6) state++;
       break;
      case 1:
        leftPower=config;
        state++;
        break;
      case 2:
        rightPower=config;
        state++;
        break;
      case 3:
        leftArmPower=config;
        state++;
        break;
      case 4:
        rightArmPower=config;
        state++;
        break;
      case 5:
        conveyorPower=config;
        state++;
        break;
      case 6:
        runPower=config;
        state=0;
        sendReport();//Serial.write(digitalRead(rightSwitch));
        break;
    }
  }
  
  /*
  leftPort.write(128-leftPower); // rear left (backwards)
  leftPort.write(leftPower+128); // front left

  rightPort.write(rightPower); // front right
  rightPort.write(127-rightPower+128); // rear right (backwards)
  */
  //leftPower=5;
  //rightPower=5;
  sendLeft(leftPower);
  sendRight(rightPower);
  sendLeftArm(leftArmPower);
  sendRightArm(rightArmPower);
  extendConveyor(conveyorPower);
  runConveyor(runPower);
}

void sendReport(void) {
{
  int tick=tickReport;
  int left=digitalRead(leftSwitch);
  int right=digitalRead(rightSwitch);
  Serial.write(0xA0 | (tick<<2) | (left<<1) | (right<<0) );
  tickReport=0;
}        

void sendLeft(int pow){
  sendCommand(wheelPort, leftAddr, 6, 128-pow);
  sendCommand(wheelPort, leftAddr, 7, pow);
}

void sendRight(int pow){
  sendCommand(wheelPort, rightAddr, 6, 128-pow);
  sendCommand(wheelPort, rightAddr, 7, pow);
}

void runConveyor(int pow){
  sendCommand(minePort, conAddr, 6, pow);
}

void extendConveyor(int pow){
  sendCommand(minePort, conAddr, 7, pow);
}

void sendLeftArm(int pow){
  armPort.write(pow+128);
}

void sendRightArm(int pow){
  armPort.write(pow);
}

void sendCommand(HardwareSerial & port, int addr, int command, int val){
  port.write(addr);
  port.write(command);
  port.write(val);
  port.write((addr+command+val)&127);
}
