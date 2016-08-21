#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0, 90);

EthernetServer server(80);

void setup()
{
  Serial.begin(9600);    
  pinMode(8, OUTPUT);    
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop()
{
  
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    String buffer = "";  
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);   
        buffer+=c;       
        if (c == '\n' && currentLineIsBlank) {
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          if (digitalRead(8)){  
            client.print(" LED is <font color='green'>ON</font>");
          }else{
            client.print(" LED is <font color='red'>OFF</font>");
          }
          client.println("<br />");
          
          
          client.print("<FORM action=\"http://192.168.0.90/\" >");
          client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"1\">ON");
          client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"0\">OFF");
          client.print("<P> <INPUT type=\"submit\" value=\"Submit\"> </FORM>");
            
          break;
        }
        if (c == '\n') {
          
          currentLineIsBlank = true;
          buffer="";       
        } else if (c == '\r') {            
          if(buffer.indexOf("GET /?status=1")>=0)
            digitalWrite(8,HIGH);  
          
          if(buffer.indexOf("GET /?status=0")>=0)
            digitalWrite(8,LOW);  
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
