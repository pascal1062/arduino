// inslude the SPI library:
#include <SPI.h>


// set pin 10 as the slave select for the digital pot:
const int chipSelect = 10;

int analogInput[8];
long previousMillis = 0;
byte b1=0;
byte b2=0;
byte b3=0;
byte b4=0;

void setup() {
  // set the chipSelect as an output:
  pinMode (chipSelect, OUTPUT);
  // initialize SPI:
  SPI.begin(); 
  Serial.begin(9600);
  digitalWrite(chipSelect, HIGH);
}

void loop() {
  // go through the six channels of the digital pot:
  //for (int i = 0; i <= 7; i++) { 
  //  analogInput[i] = mcp3008_read(chipSelect,i);
  //}
  //serialOut();
  mcp3008_read(chipSelect,1);
  serialTest(b1, b2, b3, b4);
}

int mcp3008_read(int chip_select, int channel) {
  digitalWrite(chip_select, LOW);
  //delayMicroseconds(4);
  
  b1 = SPI.transfer(((channel << 2) + 0b1100000));
  b2 = SPI.transfer(0x00);
  b3 = SPI.transfer(0x00);
  b4 = SPI.transfer(0x00);
  //int value = SPI.transfer(0) << 2;
  //value += (SPI.transfer(0) & 0b11000000) >> 6;

  digitalWrite(chip_select, HIGH);
  //return value;
  return 0;
}

//a utiliser plus tard
int mcp3008_average(int chip_select, int channel, int range) {
  int sample_i;
  long sample_sum = 0;
  for (sample_i = 0; sample_i < 10; sample_i++) {
    sample_sum += mcp3008_read(chip_select, channel);
  }
  return (sample_sum / 10) * range / 1023;
}

void serialOut() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > 1000) {
    // save the last time   
    previousMillis = currentMillis;   

    Serial.print(analogInput[0]);
    Serial.print(";");
    Serial.print(analogInput[1]);
    Serial.print(";");
    Serial.print(analogInput[2]);
    Serial.print(";");
    Serial.print(analogInput[3]);
    Serial.print(";");
    Serial.print(analogInput[4]);
    Serial.print(";");
    Serial.print(analogInput[5]);
    Serial.print(";");
    Serial.print(analogInput[6]);
    Serial.print(";");
    Serial.print(analogInput[7]);
    Serial.println();
  }   
}

void serialTest(byte hb, byte lb, byte a, byte b) {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > 1000) {
    // save the last time   
    previousMillis = currentMillis;   

    Serial.print(hb);
    Serial.print(";");
    Serial.print(lb);
    Serial.print(";");
    Serial.print(a);
    Serial.print(";");  
    Serial.print(b);
    Serial.print(";");    
    Serial.println();
  }  
}  


