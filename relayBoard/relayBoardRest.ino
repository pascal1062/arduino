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
int temperature;
int humidity;
int analogIn1;
int readings[10];
int total=0;
int index=0;

void setup(void)
{  
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
  temperature = 24.1;
  humidity = 40;
  analogIn1 = 1023;
  rest.variable("AI1",&analogIn1);
  rest.variable("temperature",&temperature);  
  rest.variable("humidity",&humidity);

  // Function to be exposed
  rest.function("led",ledControl);
  
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
  readAnalogIn();  
}

// Custom function accessible by the API
int ledControl(String command) {
  
  // Get state from command
  int state = command.toInt();
  
  digitalWrite(6,state);
  return 1;
}

void readAnalogIn() {
  //int reading = analogRead(0);
  //float analogIn1volt = reading * (5.0/1024.0);
  //analogIn1 = analogIn1volt;

  total= total - readings[index];         
  readings[index] = analogRead(0); 
  total= total + readings[index];       
  index = index + 1;                    
  if (index >= 10) {           
    index = 0;   
    analogIn1 = total / 10;   
  }  
} 
