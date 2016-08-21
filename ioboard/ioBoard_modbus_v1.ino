
// inslude libraries:
#include <SPI.h>
#include <SimpleModbusSlave.h>

SPISettings MCP3008(500000, MSBFIRST, SPI_MODE0);
SPISettings TLC5620(500000, MSBFIRST, SPI_MODE1);

// set pin 10 as the slave select  
const int adcChipSelect = 8;
const int dacLoadPin[] = {10,9}; //LOAD pin. 2 DACs TLC5620

long previousMillis = 0;

int analogInput[8];
byte analogInputBytes[16]; //analog input 0@7 separate in two bytes MSB,LSB {MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB,MSB,LSB}
int idx = 0;
int aiIdx = 0;

byte analogOutput[8];
const byte dacAddress[] = {0x03,0x01,0x05,0x07}; //DAC addr and range bit. first bytes to send to TLC5620   

//////////////// slave modbus REGISTERS ///////////////////
enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  IN1,   
  IN2,        
  IN3,
  IN4,
  IN5,
  IN6,  
  IN7,
  IN8,
  OUT1,
  OUT2,
  OUT3,
  OUT4,
  OUT5,
  OUT6,
  OUT7,
  OUT8,
  TOTAL_ERRORS,
  // leave this one
  TOTAL_REGS_SIZE 
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int holdingRegs[TOTAL_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

void setup() {
  
  // set the ChipSelect as an output:
  pinMode(adcChipSelect, OUTPUT);
  pinMode(dacLoadPin[0], OUTPUT);
  pinMode(dacLoadPin[1], OUTPUT);

  // initialize SPI:
  SPI.begin();
  
  //Serial.begin(9600);
  modbus_configure(9600, 1, A4, TOTAL_REGS_SIZE, 0);
    
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
  
  holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
  
  for (byte j = 0; j < 8; j++) {
    holdingRegs[j] = analogInput[j];
  }
  
  for (byte k = 0; k < 8; k++) {
    analogOutput[k] = holdingRegs[k+8];
  }
  //serialOut();
}

int adcChannelRead(byte readAddress) {
 
  byte dataMSB = 0;
  byte dataLSB = 0;
  byte JUNK = 0x00;
 
  SPI.beginTransaction(MCP3008);
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


