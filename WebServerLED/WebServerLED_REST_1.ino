
// Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <aREST.h>
#include <avr/wdt.h>
#include <XBee.h>

// Enter a MAC address for your controller below.
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
uint8_t ip[] = { 192, 168, 0, 91 };
uint8_t gateway[] = { 192, 168, 0, 1 };
uint8_t subnet[]  = { 255, 255, 255, 0 };

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

//Xbee
XBee xbee = XBee();
uint8_t payload[] = { 0, 0, 0 };
Tx16Request tx1 = Tx16Request(0x453F, payload, sizeof(payload)); //évier addr. led 1 et 2
Tx16Request tx2 = Tx16Request(0x454F, payload, sizeof(payload)); //coin grille-pain addr. led 3 et 4
TxStatusResponse txStatus = TxStatusResponse();

//Variables
int led = 0;
int value = 0;

void setup(void)
{  
  // Start Serial
  //Serial.begin(115200); //attention faut enlever a cause du Xbee.......
 
  // Function to be exposed
  rest.function("led1",ledControl1);
  rest.function("led2",ledControl2);
  rest.function("led3",ledControl3);
  rest.function("led4",ledControl4);

  // Give name and ID to device
  rest.set_id("001");
  rest.set_name("webServerLed");

  // Start the Ethernet connection and the server
  //if (Ethernet.begin(mac) == 0) {
    //Serial.println("Failed to configure Ethernet using DHCP"); //attention faut enlever a cause du Xbee.......
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, gateway, subnet);
  //}
  
  xbee.begin(9600); 
  server.begin();
  //Serial.print("server is at "); //attention faut enlever a cause du Xbee.......
  //Serial.println(Ethernet.localIP()); //attention faut enlever a cause du Xbee.......

  // Start watchdog
  wdt_enable(WDTO_4S);
  delay(1000);
}

void loop() {  
  
  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();
  
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
   //delay(500);
}

// Custom function accessible by the API
int ledControl1(String command) {  

  int value = command.toInt();
  XbeeTxLedValue(1,value); 
  
  return 1;
}

int ledControl2(String command) {  

  int value = command.toInt();
  XbeeTxLedValue(2,value); 
  
  return 1;
}

int ledControl3(String command) {  

  int value = command.toInt();
  XbeeTxLedValue(3,value); 
  
  return 1;
}

int ledControl4(String command) {  

  int value = command.toInt();
  XbeeTxLedValue(4,value); 
  
  return 1;
}


