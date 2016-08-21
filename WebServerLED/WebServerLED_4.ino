#include <SPI.h>
#include <Ethernet.h>
#include <XBee.h>

//Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0, 90);
EthernetServer server(80);

//Xbee
XBee xbee = XBee();
uint8_t payload[] = { 0, 0, 0 };
Tx16Request tx1 = Tx16Request(0x453F, payload, sizeof(payload));
Tx16Request tx2 = Tx16Request(0x454F, payload, sizeof(payload));
TxStatusResponse txStatus = TxStatusResponse();

//Divers
String Led;

void setup()
{
  //Serial.begin(9600);    
  pinMode(6, OUTPUT);    
  Ethernet.begin(mac, ip);
  server.begin();
  xbee.begin(9600);  
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
   delay(1000);
}

void loop()
{
   DisplayHTML();  

}
