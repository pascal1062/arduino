// inslude the SPI library:
#include <SPI.h>

SPISettings MCPXXXX(2000000, MSBFIRST, SPI_MODE0);

// set pin 10 as the slave select  
const int adcChipSelect = 10;
const int dacChipSelect[] = {9,8,7,6}; //chip select pin. Four DAC MCP4902

long previousMillis = 0;

byte dacA[] = {0x00,0x00};
byte dacB[] = {0x00,0x00};
int analogInput[8];

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
 }
  
  dacChannelWrite(dacA[0], dacA[1], dacChipSelect[0]); //write to DACA from potentiometer. H and L bytes
  dacChannelWrite(dacB[0], dacB[1], dacChipSelect[0]); //write to DACB from flasher. H and L bytes
  
  serialOut();
  writeToDACA();
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

void dacChannelWrite(byte high, byte low, int cs) {
 
  SPI.beginTransaction(MCPXXXX);
  digitalWrite(cs, LOW);
  SPI.transfer(high);
  SPI.transfer(low);
  digitalWrite(cs, HIGH);
  SPI.endTransaction(); 
}

void serialOut() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > 1000) {
    // save the last time   
    previousMillis = currentMillis; 
  
    if (flasher) {
      dacB[0] = 0xBF;
      dacB[1] = 0xFF;
      flasher = false;
    } else {
      dacB[0] = 0xB0;
      dacB[1] = 0x00;
      flasher = true;     
    } 
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

void writeToDACA() {
  
  int potValue = map(analogInput[7], 0, 509, 0, 255);
  dacA[0] = (3 << 4) | (potValue >> 4);
  dacA[1] = (potValue & 0x0F)<< 4;    
}
