// inslude the SPI library:
#include <SPI.h>

SPISettings MCP4902(2000000, MSBFIRST, SPI_MODE0);

// set pin 10 as the slave select  
const int chipSelect = 10;

long previousMillis = 0;
byte dacA[] = {0x00,0x00};
byte dacB[] = {0x00,0x00};
bool flasher = false;

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
  
  writeAnalogOut(dacA[0], dacA[1]); //write to DACA from potentiometer. H and L bytes
  writeAnalogOut(dacB[0], dacB[1]); //write to DACB from flasher. H and L bytes
  
  serialOut();
  writeToDACA();
}

void writeAnalogOut(byte high, byte low) {
 
  SPI.beginTransaction(MCP4902);
  digitalWrite(chipSelect, LOW);
  SPI.transfer(high);
  SPI.transfer(low);
  digitalWrite(chipSelect, HIGH);
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
    Serial.print(dacB[0]);
    Serial.print(";");
    Serial.print(dacB[1]);
    Serial.println();
    Serial.print(dacA[0]);
    Serial.print(";");
    Serial.print(dacA[1]);
    Serial.println();    
  }   
}

void writeToDACA() {
  
  int potValue = analogRead(A0);
  potValue = map(potValue, 0, 1023, 0, 255);
  dacA[0] = (3 << 4) | (potValue >> 4);
  dacA[1] = (potValue & 0x0F)<< 4;    
  
}
