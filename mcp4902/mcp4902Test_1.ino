// inslude the SPI library:
#include <SPI.h>

SPISettings MCP4902(2000000, MSBFIRST, SPI_MODE0);

// set pin 10 as the slave select  
const int chipSelect = 10;

long previousMillis = 0;
byte DACAfirstByte = 0x00;
byte DACAsecondByte = 0x00;
byte DACBfirstByte = 0x00;
byte DACBsecondByte = 0x00;
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
  
  //writeAnalogOut(DACAfirstByte, DACAsecondByte);
  //writeAnalogOut(DACBfirstByte, DACBsecondByte);
  
  serialOut();

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
  
  if(currentMillis - previousMillis > 100) {
    // save the last time   
    previousMillis = currentMillis; 
  
    if (flasher) {
      writeAnalogOut(0x30, 0x00); //write 0 to DACA
      writeAnalogOut(0xBF, 0xFF); //write 255 to DACB
      flasher = false;
    } else {
      writeAnalogOut(0x3F, 0xFF); //write 255 to DACA
      writeAnalogOut(0xB0, 0x00); //write 0 to DACB
      flasher = true;     
    } 
    Serial.println("writing to DACs");
  }   
}
