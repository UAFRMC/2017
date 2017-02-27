//analogTest.ino
//Test sensors on pins 34,36,50,52
//Made to test Bryant Klug's custom shield for Aurora Robotics
//Written for Aurora Robotics. (UAF RMC 2017)
//Arsh Chauhan
//02/26/2017
//Public Domain  




void setup() {
  
    pinMode(34,INPUT);
    pinMode(36,INPUT);
    pinMode(50,INPUT);
    pinMode(52,INPUT);

  Serial.begin(9600);

}

int values[4];

void loop() {

 values[0] = digitalRead(34);
 values[1] = digitalRead(36);
 values[2] = digitalRead(50);
 values[3] = digitalRead(52);
for( int i=0; i<4; i++)
 {
    Serial.print(values[i]);
 } 
 
 Serial.println();

 delay(50);
 }
