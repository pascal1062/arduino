#include <SPI.h>
#include <Ethernet.h>

#include "Mudbus.h"

//#define DEBUG

Mudbus Mb;
//Function codes 1(read coils), 3(read registers), 5(write coil), 6(write register)
//signed int Mb.R[0 to 125] and bool Mb.C[0 to 128] MB_N_R MB_N_C
//Port 502 (defined in Mudbus.h) MB_PORT

int led = 0;
int value = 0;
String str = "";

void setup()
{
  uint8_t mac[]     = { 0x90, 0xA2, 0xDA, 0x00, 0x51, 0x06 };
  uint8_t ip[]      = { 192, 168, 0, 91 };
  uint8_t gateway[] = { 192, 168, 0, 1 };
  uint8_t subnet[]  = { 255, 255, 255, 0 };
  Ethernet.begin(mac, ip, gateway, subnet);
  //Avoid pins 4,10,11,12,13 when using ethernet shield

  delay(5000);  //Time to open the terminal
  Serial.begin(9600);
  
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop()
{
  Mb.Run();

  led = Mb.R[0];     
  value = Mb.R[1];

  //Serial.println(led);
  //Serial.println(value);
  //Serial.println(Mb.Data);
  //delay(1000);
  
  if (Mb.Data) {
    Serial.println(Mb.Data);
    Serial.println(led);
    Serial.println(value);
  }
}

/*
Modpoll commands
  Read the registers Mb.R[0], Mb.R[1], and Mb.R[2]
    ./modpoll -m tcp -t4 -r 1 -c 3 192.168.1.8
  Read the coil Mb.C[0]
    ./modpoll -m tcp -t0 -r 1 -c 1 192.168.1.8        
*/

