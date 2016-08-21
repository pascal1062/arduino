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
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<meta name=\"viewport\"content=\"user-scalable=no,width=device-width\"/>");
          client.println("<body><form method=get>");

          if (digitalRead(8)){  
            client.print(" LED is <font color='green'>ON</font>");
          }else{
            client.print(" LED is <font color='red'>OFF</font>");
          }
          client.println("<br />");
     
          client.print("<p style=\"text-align: center\"> <input type=\"submit\" name=\"status\" value=\"Marche\"/></p>");
          client.print("<p style=\"text-align: center\"> <input type=\"submit\" name=\"status\" value=\"Arret\"/></p>");
          client.println("</form></body></html>"); 
          break;
        }
        if (c == '\n') {
          
          currentLineIsBlank = true;
          buffer="";       
        } else if (c == '\r') {            
          if(buffer.indexOf("GET /?status=Marche")>=0)
            digitalWrite(8,HIGH);  
          
          if(buffer.indexOf("GET /?status=Arret")>=0)
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
