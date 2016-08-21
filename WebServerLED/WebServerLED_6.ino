#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <XBee.h>

//Ethernet
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
uint8_t ip[] = { 192, 168, 0, 91 };
uint8_t gateway[] = { 192, 168, 0, 1 };
uint8_t subnet[]  = { 255, 255, 255, 0 };

//Modbus TCP
Mudbus Mb;
int led = 0;
int value = 0;

//Xbee
XBee xbee = XBee();
uint8_t payload[] = { 0, 0, 0 };
Tx16Request tx1 = Tx16Request(0x453F, payload, sizeof(payload)); //évier addr. led 1 et 2
Tx16Request tx2 = Tx16Request(0x454F, payload, sizeof(payload)); //coin grille-pain addr. led 3 et 4
TxStatusResponse txStatus = TxStatusResponse();

//Divers
long previousMillis1 = 0;
boolean powerUp = true;

void setup()
{
  //Serial.begin(9600);    
  pinMode(6, OUTPUT);    
  Ethernet.begin(mac, ip, gateway, subnet);
  xbee.begin(9600); 
  powerUp = true;
  delay(1000);
}

void ModbusReceive()
{
  //Réception Modbus dans les registres Hold 0 et 1 (40000 et 40001). reg0=>addresse led, reg1=>dim value
  led = Mb.R[0];     
  value = Mb.R[1];  
  
  //Si Mb.Data == true, signifie qu'il y a réception modbus sur le port ethernet
  if (Mb.Data) {
    XbeeTxLedValue(led,value);   
  } 

} 

void XbeeTxLedValue(int address, int level)
{
   payload[0] = 15;
   payload[1] = address;
   payload[2] = map(level, 0, 100, 0, 255);
   if ((address == 1) || (address == 2)) { //envoi sur les leds 1 et 2 évier
     xbee.send(tx1);    
   } 
   if ((address == 3) || (address == 4)) { //envoi sur les leds 3 et 4 coin toaster
     xbee.send(tx2);    
   }   
   //if (address == 2) { analogWrite(6,level); }
   delay(500);
}

void loop()
{
  Mb.Run();
  ModbusReceive();
  
}
