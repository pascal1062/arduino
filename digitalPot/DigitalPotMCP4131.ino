// include the SPI library:
#include <SPI.h> 
 
// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
 
void setup()
{
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  // initialize SPI:
  SPI.begin(); 
}
 
void loop()
{
    //basically just ramp the wiper up and down the full range
    //but the upwards ramp is deliberately faster than the downwards ramp
     
    //for (int level = 0; level <= 127; level++)
    //{
      digitalPotWrite(126);
       
      //increase this number to slow things down
      //(10000) is a good number if you want to see the LED pulsing nicely
     // delay(500); 
    //}
     
    //for (int level = 127; level >= 0; level--)
    //{
      //digitalPotWrite(level);
       
      //increase this number to slow things down
      //(20000) is a good number if you want to see the LED pulsing nicely
      delay(10);
    //}
}
 
 
int digitalPotWrite(int value)
{
  //take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin, LOW);
  //send in the address and value via SPI:
  SPI.transfer(0);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin, HIGH); 
}
