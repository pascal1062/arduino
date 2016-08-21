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
long previousMillis = 0;
int inputPin = 0;
int lastPin = -1;
int analogIn[6];

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
  rest.variable("algIn1",&analogIn[0]);
  rest.variable("algIn2",&analogIn[1]);
  rest.variable("algIn3",&analogIn[2]);
  rest.variable("algIn4",&analogIn[3]);
  rest.variable("algIn5",&analogIn[4]);
  rest.variable("algIn6",&analogIn[5]);  
  
  // Function to be exposed
  rest.function("ledDim",ledControl);
  
  // Give name and ID to device
  rest.set_id("2");
  rest.set_name("serial");

  // Start watchdog
  wdt_enable(WDTO_4S);
 
}

void loop() {  
  
  // Handle REST calls
  rest.handle(Serial);  
  wdt_reset();
  delaySub();
  readAnalogPin();
}

void readAnalogPin() {
  
   if (inputPin != lastPin) {
     int total = 0;
     int average = 0;
     int readings[10];

     for (int a=0; a<=9; a++) {
       readings[a] = analogRead(inputPin); 
       total= total + readings[a];       
       average = total / 10; 
       delay(1);
     }
     analogIn[inputPin] = average;
   }
   lastPin = inputPin;
}

void delaySub() {
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > 25) {
    previousMillis = currentMillis;   
    
    if (inputPin >= 5) inputPin = -1;
    inputPin = inputPin + 1;
  }
}

// Custom function accessible by the API
int ledControl(String command) {
  
  // Get state from command
  int value = command.toInt();
  
  analogWrite(9,value);
  return 1;
}


