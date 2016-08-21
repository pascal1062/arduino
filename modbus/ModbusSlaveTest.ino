#include <SimpleModbusSlave.h>

#define  ledPin  6 // onboard led 
int photoPin = A0;

/* This example code has 8 holding registers. 6 analogue inputs, 1 digital output
   and 1 register to indicate errors encountered since started.
   Function 5 (write single coil) is not implemented so I'm using a whole register
   and function 16 to set the onboard Led on the Atmega328P.
   
   The modbus_update() method updates the holdingRegs register array and checks
   communication.

   Note:  
   The Arduino serial ring buffer is 128 bytes or 64 registers.
   Most of the time you will connect the arduino to a master via serial
   using a MAX485 or similar.
 
   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 122 bytes or 61 registers.
 
   In a function 16 request the master will attempt to write to your 
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS, 
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   118 bytes or 59 registers.
 
   Using a USB to Serial converter the maximum bytes you can send is 
   limited to its internal buffer which differs between manufactures. 
*/
 

// Using the enum instruction allows for an easy method for adding and 
// removing registers. Doing it this way saves you #defining the size 
// of your slaves register array each time you want to add more registers
// and at a glimpse informs you of your slaves register layout.

//////////////// registers of your slave ///////////////////
enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  ADC0,     
  ADC1,        
  ADC2,
  ADC3,
  ADC4,
  ADC5,  
  led_pwm,
  TOTAL_ERRORS,
  // leave this one
  HOLDING_REGS_SIZE 
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

void setup()
{
  /* parameters(long baudrate, 
                unsigned char ID, 
                unsigned char transmit enable pin, 
                unsigned int holding registers size,
                unsigned int* holding register array)
    Pascal: attention....ici la version 5 et il n'y a pas possibilité de mettre disable le Txenable pin.
            On peut le faire dans la version 4. le Txenable pin est bon pour le chip RS485   
    Pascal: c'est 8N1 par défaut.            
  */
  
  modbus_configure(9600, 1, 2, HOLDING_REGS_SIZE, holdingRegs);    
  
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  // modbus_update() is the only method used in loop(). It returns the total error
  // count since the slave started. You don't have to use it but it's useful
  // for fault finding by the modbus master.
  
  holdingRegs[TOTAL_ERRORS] = modbus_update();
  
  for (byte i = 0; i < 5; i++)
  {
    holdingRegs[i] = i;
    delayMicroseconds(500);	     
  }
  
  holdingRegs[ADC5] = analogRead(photoPin);
  
  // read the led_pwm register value and set the onboard LED 
  // function 16 and one whole register is used 0000h == OFF
  // and anything except zero is ON
  //digitalWrite(ledPin, holdingRegs[led_pwm]);
  analogWrite(ledPin,holdingRegs[led_pwm]);
}

