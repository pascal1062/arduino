#include <Wire.h>

#define SLAVE_ADDRESS 0x12
short ledValue = 0;
int idx = 0;
int analogInIdx = -1;
byte analogInput[] = {0,0,0,0,0,0,0,0}; //4 analog input readings 0 to 3. high and low bytes. {H1,L1,H2,L2,H3,L3,H4,L4}
byte highLowByteIDX[] = {0,2,4,6}; //start index to write to "analogInput[]" array...step by 2.

void setup() {
  pinMode(6, OUTPUT); //analog output
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
    
  Serial.println("Ready");
}

void loop() {
  analogInIdx++;
  if (analogInIdx > 3) { //analog input 0 to 3
    analogInIdx=0;
  }   
  analogInRead(analogInIdx);
     
  delay(10);
}

void receiveData(int bytes) {
  int operation = Wire.read();
  
  // Change the LED intensity...
  if (operation == 0x06 && bytes > 1) {
    ledValue = Wire.read();
    analogOutWrite(operation, ledValue);
  }
  
  // Reset analog readings...
  if (operation == 0xAA && bytes > 1) {
    idx = 0;
  }  

  // Consume any remainder bytes...
  while (Wire.available()) {
    Wire.read();
  }
}

void sendData() {
  Wire.write(analogInput[idx]);
  idx++;
  if (idx>7) {
    idx=0;
  }
  //maybe send a last byte telling python we're at the end of the array...and reset to 0 after that...
  //Serial.println("index to: "+idx);
}

void analogInRead(int AIpin) {
  //extract high and low from analog input readings. Stores in array to end with.
  int AnlgIn = analogRead(AIpin);
  byte high = (AnlgIn >> 8) & 0xFF;
  byte low = AnlgIn & 0xFF; 
 
  byte i = highLowByteIDX[AIpin]; 
  analogInput[i] = high;
  analogInput[i+1] = low;
}

void analogOutWrite(int AOpin, int pwm) {
  analogWrite(AOpin, pwm);
}


