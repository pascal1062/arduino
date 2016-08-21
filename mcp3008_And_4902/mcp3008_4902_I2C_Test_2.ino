// inslude the SPI library:
#include <SPI.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x12
SPISettings MCPXXXX(2000000, MSBFIRST, SPI_MODE0);

// set pin 10 as the slave select  
const int adcChipSelect = 10;
const int dacChipSelect[] = {9,8,7,6}; //chip select pin. Four DAC MCP4902

long previousMillis = 0;

int analogInput[8];
byte analogInputBytes[16]; //analog input 0@7 separate in two bytes MSB,LSB {MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB}
int idx = 0;
int aiIdx = 0;

byte analogOutput[8];
byte dacA[] = {0x00,0x00};
byte dacB[] = {0x00,0x00};

bool flasher = false;

void setup() {
  
  // set the ChipSelect as an output:
  pinMode(adcChipSelect, OUTPUT);
  pinMode(dacChipSelect[0], OUTPUT);
  pinMode(dacChipSelect[1], OUTPUT);
  pinMode(dacChipSelect[2], OUTPUT);
  pinMode(dacChipSelect[3], OUTPUT);
  
  // initialize SPI:
  SPI.begin();
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  Serial.begin(9600);
    
  digitalWrite(adcChipSelect, LOW);
  digitalWrite(adcChipSelect, HIGH);
  
  for (int a = 0; a <= 3; a++) {
    digitalWrite(dacChipSelect[a], LOW); 
    digitalWrite(dacChipSelect[a], HIGH); 
  }
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

void dacChannelWrite(byte high, byte low, int cs) {
 
  SPI.beginTransaction(MCPXXXX);
  digitalWrite(cs, LOW);
  SPI.transfer(high);
  SPI.transfer(low);
  digitalWrite(cs, HIGH);
  SPI.endTransaction(); 
}

void analogOutputWrite() {
  
  int cspin = 0;
  for (int a = 0; a <= 6; a+=2) {
    dacA[0] = (3 << 4) | (analogOutput[a] >> 4); //MSB to DAC-A
    dacA[1] = (analogOutput[a] & 0x0F) << 4; //LSB to DAC-A
    
    dacB[0] = (11 << 4) | (analogOutput[a+1] >> 4); //MSB to DAC-B
    dacB[1] = (analogOutput[a+1] & 0x0F) << 4; //LSB to DAC-B
    
    dacChannelWrite(dacA[0], dacA[1], dacChipSelect[cspin]); //write to DAC-A MSB and LSB
    dacChannelWrite(dacB[0], dacB[1], dacChipSelect[cspin]); //write to DAC-B MSB and LSB
    cspin++;
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
  //Wire.write(analogInputBytes[idx]);
  //idx++;
  //if (idx>15) {
  //  idx=0;
  //}
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


