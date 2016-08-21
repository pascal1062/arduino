#include <SimpleModbusSlave.h>

byte i = 0;
byte Relay1 = 0;
byte Relay2 = 0;
byte Relay3 = 0;
byte Relay4 = 0;
byte Relay5 = 0;

enum 
{     
  // The first register starts at address 0
  AI_1, AI_2, AI_3, AI_4, AI_5, AI_6,  
  DI_1, DI_2, DI_3, DI_4, DI_5, DI_6,
  DO_1, DO_2, DO_3, DO_4, DO_5, DO_6,
  TOTAL_ERRORS,
  // leave this one
  TOTAL_REGS_SIZE 
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int holdingRegs[TOTAL_REGS_SIZE];

/* Arduino avec board vellemann ka05: 6 analog IN, 6 digital IN et 6 digital OUT (le dernier pin 13 relais pas utilisé)
   
*/

void setup()
{
  /* parameters(long baudrate, 
                unsigned char ID, 
                unsigned char transmit enable pin, 
                unsigned int holding registers size)
                
     The transmit enable pin is used in half duplex communication to activate a MAX485 or similar
     to deactivate this mode use any value < 2 because 0 & 1 is reserved for Rx & Tx (8N1)
  */
  
  //modbus_configure(9600, 1, 13, TOTAL_REGS_SIZE, holdingRegs); 
  modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 13, TOTAL_REGS_SIZE, holdingRegs);
  
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
}

void loop()
{
  // modbus_update() is the only method used in loop(). It returns the total error
  // count since the slave started. You don't have to use it but it's useful
  // for fault finding by the modbus master.
  modbus_update();
  
  //read 6 analog inputs (Arduino pin A0 to A5) and put in hold register from 0 to 5
  for (i = 0; i <= 5; i++)
  {
    holdingRegs[i] = analogRead(i);
    delayMicroseconds(500);	     
  }
  
  //read 6 binary inputs (Arduino pin 2 to 7) and put in hold register from 6 to 11
  for (i = 6; i <= 11; i++)
  {
    holdingRegs[i] = digitalRead(i-4);
  }
  
  //read the digital output register value and set the ka05 relay high or low with function 16
  Relay1 = holdingRegs[DO_1]; 
  Relay2 = holdingRegs[DO_2];
  Relay3 = holdingRegs[DO_3];
  Relay4 = holdingRegs[DO_4];
  Relay5 = holdingRegs[DO_5]; 
 
  if (Relay1) { // set Relay1 on Arduino pin 8
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);    
  }   
 
  if (Relay2) { // set Relay2 on Arduino pin 9
    digitalWrite(9, HIGH);
  } else {
    digitalWrite(9, LOW);    
  } 

  if (Relay3) { // set Relay3 on Arduino pin 10
    digitalWrite(10, HIGH);
  } else {
    digitalWrite(10, LOW);    
  } 

  if (Relay4) { // set Relay4 on Arduino pin 11
    digitalWrite(11, HIGH);
  } else {
    digitalWrite(11, LOW);    
  } 

  if (Relay5) { // set Relay5 on Arduino pin 12
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);    
  }    
  
}

