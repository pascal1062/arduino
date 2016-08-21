#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <XBee.h>

//Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,90);
EthernetServer server(80);

//Udp (heure sur site ntp)
unsigned int localPort = 8888; 
IPAddress timeServer(66,219,116,140); // time.nist.gov NTP server
const int NTP_PACKET_SIZE= 48; //NTP 48 bytes messages
byte packetBuffer[ NTP_PACKET_SIZE];
EthernetUDP Udp;
unsigned long secsArduino = 3569628630UL;
String heure = "00:00:00";

//Xbee
XBee xbee = XBee();
uint8_t payload[] = { 0, 0, 0 };
Tx16Request tx1 = Tx16Request(0x453F, payload, sizeof(payload));
Tx16Request tx2 = Tx16Request(0x454F, payload, sizeof(payload));
TxStatusResponse txStatus = TxStatusResponse();

//Divers
String Led;
long previousMillis1 = 0;
boolean powerUp = true;

void setup()
{
  //Serial.begin(9600);    
  pinMode(6, OUTPUT);    
  Ethernet.begin(mac, ip);
  server.begin();
  Udp.begin(localPort);
  xbee.begin(9600); 
  powerUp = true;
}

void DisplayHTML()
{
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    String buffer = "";  
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.print(c);   
        buffer+=c;       
        if (c == '\n' && currentLineIsBlank) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<head><meta name=\"viewport\"content=\"user-scalable=no,width=device-width\"/></head>");
        client.println("<body>");
        
        client.println("<h3 style=\"text-align: center\">"+ heure);
        client.println("</h3>");
        for(int i=1;i <= 4 ;i++){
          Led = String("Led") + i;
          client.println("<h4 style=\"text-align: center\">"+ Led);
          client.println("<form style=\"text-align: center\" method=get style=\"display:inline\">");
          client.println("<input type=submit name="+ Led);
          client.print("value=\"Off\">");
          client.println("<input type=\"submit\" name="+ Led);
          client.print("value=\"25%\">");
          client.println("<input type=\"submit\" name="+ Led);
          client.print("value=\"50%\">");
          client.println("<input type=\"submit\" name="+ Led);
          client.print("value=\"75%\">");
          client.println("<input type=\"submit\" name="+ Led);
          client.print("value=\"100%\">");
          client.println("</form> </h4>"); 
        }
        client.println("</body></html>");
        break;
        }
        if (c == '\n') {
          
          currentLineIsBlank = true;
          buffer=""; 
          //Check pour bouton html appuyé...et ensuite envoi via Xbee      
        } else if (c == '\r') {
          for(int i=1;i <= 4 ;i++){ 
            Led = String("Led") + i;
            if(buffer.indexOf("GET /?"+Led+"=Off")>=0)
              XbeeTxLedValue(i,0);            
            if(buffer.indexOf("GET /?"+Led+"=25%")>=0)
              XbeeTxLedValue(i,35);            
            if(buffer.indexOf("GET /?"+Led+"=50%")>=0)
              XbeeTxLedValue(i,80);            
            if(buffer.indexOf("GET /?"+Led+"=75%")>=0)
              XbeeTxLedValue(i,150);            
            if(buffer.indexOf("GET /?"+Led+"=100%")>=0)
              XbeeTxLedValue(i,255);            
          }  
        }
        else {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }  
} 

void XbeeTxLedValue(int address, int level)
{
   payload[0] = 15;
   payload[1] = address;
   payload[2] = level;   
   if ((address == 1) || (address == 2)) { //envoi sur les leds 1 et 2 évier
     xbee.send(tx1);    
   } 
   if ((address == 3) || (address == 4)) { //envoi sur les leds 3 et 4 coin toaster
     xbee.send(tx2);    
   }   
   if (address == 2) { analogWrite(6,level); }
   delay(500);
}

unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 		   
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
   
  delay(1000);  
  unsigned long secsSince1900;
  if ( Udp.parsePacket()) {  
    Udp.read(packetBuffer,NTP_PACKET_SIZE);   
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    secsSince1900 = highWord << 16 | lowWord; 
  } 
  if (secsArduino != secsSince1900) secsArduino = secsSince1900;
  powerUp = false;
}

void timeClock()
{
  ++secsArduino;
    
  unsigned long tzOffset = 18000; //offset pour heure de l'est 300 min. X 60 sec
  const unsigned long seventyYears = 2208988800UL;     
  unsigned long epoch = secsArduino - seventyYears;  
  heure = String(((epoch-tzOffset) % 86400L) / 3600); 
  heure = heure + ":";  
  if (((epoch % 3600) / 60) < 10) heure = heure + '0';
  heure = heure + String(((epoch  % 3600) / 60));  
  heure = heure + ":"; 
  if ((epoch % 60) < 10 ) heure = heure + '0';
  heure = heure + String(epoch %60);
  delay(1000);  
}

void loop()
{
  if (powerUp == true) {
    sendNTPpacket(timeServer);
  } else {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis1 > 3600000) {
      previousMillis1 = currentMillis;   
      sendNTPpacket(timeServer);
   }  
  }
  timeClock();
  DisplayHTML();  
  
}
