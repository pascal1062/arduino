// inslude the SPI library:
#include <SPI.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x12

SPISettings MCPXXXX(2000000, MSBFIRST, SPI_MODE0);
SPISettings TLC5620(2000000, MSBFIRST, SPI_MODE1);

// set pin 10 as the slave select  
const int adcChipSelect = 10;
const int dacLoadPin[] = {9,8}; //LOAD pin. 2 DACs TLC5620

long previousMillis = 0;

int analogInput[8];
byte analogInputBytes[16]; //analog input 0@7 separate in two bytes MSB,LSB {MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB}
int idx = 0;
int aiIdx = 0;

byte analogOutput[8];
const byte dacAddress[] = {0x01,0x03,0x05,0x07}; //DAC addr and range bit. first bytes to send to TLC5620

void setup() {
  
  // set the ChipSelect as an output:
  pinMode(adcChipSelect, OUTPUT);
  pinMode(dacLoadPin[0], OUTPUT);
  pinMode(dacLoadPin[1], OUTPUT);

  // initialize SPI:
  SPI.begin();
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  Serial.begin(9600);
    
  digitalWrite(adcChipSelect, LOW);
  digitalWrite(adcChipSelect, HIGH);
  
  digitalWrite(dacLoadPin[0], HIGH);
  digitalWrite(dacLoadPin[1], HIGH);  
}

void loop() {
  
  for (int i = 0; i <= 7; i++) { 
    analogInput[i] = adcChannelRead(i);
    analogInTwoBytes(i);
  }

  analogOutputWrite();
  serialOut();
}

int adcChannelRead(byte readAddress) {
 
  byte dataMSB = 0;
  byte dataLSB = 0;
  byte JUNK = 0x00;
 
  SPI.beginTransaction(MCPXXXX);
  digitalWrite(adcChipSelect, LOW);
  SPI.transfer(0x01); // Start Bit                            
  dataMSB = SPI.transfer(((0x01 <<  3) | readAddress) << 4) & 0x03; // Send readAddress and receive MSB data, masked to two bits
  dataLSB = SPI.transfer(JUNK); // Push junk data and get LSB byte return
  digitalWrite(adcChipSelect, HIGH);
  SPI.endTransaction();
 
  return dataMSB << 8 | dataLSB;
}

void analogInTwoBytes(int id) { //Put 10 bits analog input in two separate bytes. For sending through I2C.
  //extract high and low from analog input readings. Stores in array after....
  int AnlgIn = analogInput[id];
  byte high = (AnlgIn >> 8) & 0xFF;
  byte low = AnlgIn & 0xFF; 
  
  if (id == 0) {
    aiIdx = 0;
  }   
  analogInputBytes[aiIdx] = high;
  analogInputBytes[aiIdx+1] = low;
  aiIdx += 2;
}

void dacChannelWrite(byte high, byte low, int loadPin) {
 
  SPI.beginTransaction(TLC5620);
  digitalWrite(loadPin, HIGH);
  SPI.transfer(high);
  SPI.transfer(low);
  digitalWrite(loadPin, LOW);
  digitalWrite(loadPin, LOW);
  digitalWrite(loadPin, LOW);
  digitalWrite(loadPin, HIGH);
  SPI.endTransaction(); 
}

void analogOutputWrite() {
  
  for (int a = 0; a <= 3; a++) {
    dacChannelWrite(dacAddress[a], analogOutput[a], dacLoadPin[0]); //write addr and value to DAC #1 TCL5620
  }
  for (int a = 0; a <= 3; a++) {
    dacChannelWrite(dacAddress[a], analogOutput[a+4], dacLoadPin[1]); //write addr and value to DAC #2 TCL5620
  }  
}

void receiveData(int bytes) {
  
  int operation = Wire.read();
  byte value;
  
  // Update analog Output value
  if (operation <= 7 && bytes > 1) {
    value = Wire.read();
    analogOutput[operation] = value;
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
  //maybe send a last byte telling python we're at the end of the array...and reset to 0 after that...
  //Serial.println("index to: "+idx);
  Wire.write((byte*) &analogInputBytes, sizeof(analogInputBytes));
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
    Serial.println();;    
  }   
}


