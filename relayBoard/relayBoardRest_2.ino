/* 
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Serial port. See the README file for more details.
 
  Written in 2014 by Marco Schwartz under a GPL license. 
*/

// Libraries
#include <SPI.h>
#include <aREST.h>
#include <avr/wdt.h>

// Create aREST instance
aREST rest = aREST();

// Variables to be exposed to the API
const int numReadings = 10;
long previousMillis = 0;
int algPin = 0;
int idxNum = 0;
int total[5];
int analogReadDelay[5];
int readingsA0[numReadings];
int readingsA1[numReadings];
int readingsA2[numReadings];
int readingsA3[numReadings];
int readingsA4[numReadings];
int readingsA5[numReadings];
int analogIn0;
int analogIn1;
int analogIn2;
int analogIn3;
int analogIn4;
int analogIn5;
 

void setup(void) {  
  // Start Serial
  Serial.begin(115200);
  
  pinMode(8, OUTPUT); //DO_0
  pinMode(9, OUTPUT); //DO_1
  pinMode(10, OUTPUT); //DO_2
  pinMode(11, OUTPUT); //DO_3
  pinMode(12, OUTPUT); //DO_4
  pinMode(7, INPUT); //DI_0
  pinMode(6, INPUT); //DI_1
  pinMode(5, INPUT); //DI_2
  pinMode(4, INPUT); //DI_3
  pinMode(3, INPUT); //DI_4
  pinMode(2, INPUT); //DI_5 
  
  // Init variables and expose them to REST API
  analogIn0 = 0;
  analogIn1 = 0;
  analogIn2 = 0;
  analogIn3 = 0;
  analogIn4 = 0;
  analogIn5 = 0;  
  rest.variable("firstAlgIn",&analogIn0);
  rest.variable("secondAlgIn",&analogIn1);
  //rest.variable("thirdAnalog",&analogIn2);
  //rest.variable("fourthAnalog",&analogIn3);
  //rest.variable("fithAnalog",&analogIn4);
  //rest.variable("sixthAnalog",&analogIn5); 
  
  // Give name and ID to device
  rest.set_id("2");
  rest.set_name("serial");

  // Start watchdog
  wdt_enable(WDTO_4S);
  
  //intialize analog input arrays
  for (int a = 0; a <= numReadings; a++) {
    readingsA0[a] = 0; 
    readingsA1[a] = 0; 
    readingsA2[a] = 0; 
    readingsA3[a] = 0; 
    readingsA4[a] = 0;
    readingsA5[a] = 0;    
  }  
}

void loop() {  
  
  // Handle REST calls
  rest.handle(Serial);  
  wdt_reset();
  readAnalogIn();
  avgAnalogIn();
}

void avgAnalogIn() {
  total[0] = total[0] - readingsA0[idxNum];
  readingsA0[idxNum] = analogReadDelay[0];
  total[0]= total[0] + readingsA0[idxNum]; 
  //delayMicroseconds(200);

  total[1] = total[1] - readingsA1[idxNum];
  readingsA1[idxNum] = analogReadDelay[1];
  total[1]= total[1] + readingsA1[idxNum];
  //delayMicroseconds(200);
 
  total[2] = total[2] - readingsA2[idxNum];
  readingsA2[idxNum] = analogReadDelay[2];
  total[2]= total[2] + readingsA2[idxNum];
  //delayMicroseconds(200); 

  total[3] = total[3] - readingsA3[idxNum];
  readingsA3[idxNum] = analogReadDelay[3];
  total[3]= total[3] + readingsA3[idxNum];
  //delayMicroseconds(200);  
  
  total[4] = total[4] - readingsA4[idxNum];
  readingsA4[idxNum] = analogReadDelay[4];
  total[4]= total[4] + readingsA4[idxNum];
  //delayMicroseconds(200);
 
  total[5] = total[5] - readingsA5[idxNum];
  readingsA5[idxNum] = analogReadDelay[5];
  total[5]= total[5] + readingsA5[idxNum];
  //delayMicroseconds(200);   
  
  idxNum = idxNum + 1;
  
  if (idxNum >= numReadings) idxNum = 0; 
  
  analogIn0 = total[0] / numReadings;
  analogIn1 = total[1] / numReadings;
  analogIn2 = total[2] / numReadings;
  analogIn3 = total[3] / numReadings;
  analogIn4 = total[4] / numReadings;  
  analogIn5 = total[5] / numReadings;   
} 

void readAnalogIn() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > 5) {
    previousMillis = currentMillis;   
   
    if (algPin == 0) analogReadDelay[0] = analogRead(0);
    if (algPin == 1) analogReadDelay[1] = analogRead(1);
    if (algPin == 2) analogReadDelay[2] = analogRead(2);
    if (algPin == 3) analogReadDelay[3] = analogRead(3);
    if (algPin == 4) analogReadDelay[4] = analogRead(4);
    if (algPin == 5) analogReadDelay[5] = analogRead(5);
    
    algPin= algPin + 1;
    if (algPin >= 5) algPin = 0; 
  }
}  


