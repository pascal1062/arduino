// inslude the SPI library:
#include <SPI.h>

SPISettings MCP3008(500000, MSBFIRST, SPI_MODE0);

// set pin 10 as the slave select  
const int chipSelect = 10;

int analogInput[8];
const byte adc_channel[] = {0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};

long previousMillis = 0;

void setup() {
  // set the chipSelect as an output:
  pinMode (chipSelect, OUTPUT);
  // initialize SPI:
  SPI.begin(); 
  Serial.begin(9600);
  digitalWrite(chipSelect, LOW);
  digitalWrite(chipSelect, HIGH);
}

void loop() {
  // go through the height channels 
  
  for (int i = 0; i <= 7; i++) { 
    analogInput[i] = adc_channel_read(adc_channel[i]);
  }
  serialOut();

}

int adc_channel_read(byte readAddress) {
 
  byte dataMSB = 0;
  byte dataLSB = 0;
  byte JUNK    = 0xFF;
 
  SPI.beginTransaction(MCP3008);
  digitalWrite(chipSelect, LOW);
  //delayMicroseconds(5);
  SPI.transfer(0x01);                               // Start Bit
  dataMSB = SPI.transfer(readAddress << 4) & 0x03;  // Send readAddress and receive MSB data, masked to two bits
  //dataMSB = SPI.transfer(0xC0) & 0x03;
  //dataMSB = SPI.transfer(((0x01 <<  3) | readAddress) << 4) & 0x03;
  dataLSB = SPI.transfer(JUNK);                     // Push junk data and get LSB byte return
  digitalWrite(chipSelect, HIGH);
  SPI.endTransaction();
 
  return (dataMSB << 8) | dataLSB;
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
