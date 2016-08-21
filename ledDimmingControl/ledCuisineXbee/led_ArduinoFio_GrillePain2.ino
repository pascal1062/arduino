#include <ClickButton.h>
#include <XBee.h>

//Xbee (Receive)
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
uint8_t data0 = 0;
uint8_t data1 = 0;
uint8_t data2 = 0;

const int ledPin9 = 9;
const int ledPin10 = 10;
const int ledPin13 = 13;
const int buttonPin2 = 2;
const int buttonPin3 = 3;

//Bouton click
ClickButton button1(buttonPin2, HIGH);
ClickButton button2(buttonPin3, HIGH);

//Divers boutons et dimming leds
int increment[] = { 0, 0 };
double fadeValue[] = { 0, 0 };
int led = 0; //choix du 1er ou 2e "set de leds" a dimmer

void setup()
{
  pinMode(ledPin9,OUTPUT);
  pinMode(ledPin10,OUTPUT);
  pinMode(ledPin13,OUTPUT);  
  xbee.begin(9600);
  //Serial.begin(9600);  
}

void DimUp(int delai) {
  increment[led] = constrain(increment[led] += 1, 0, 1024);
  fadeValue[led] = round(pow(2.0,increment[led]/128.0)-1);
  fadeValue[led] = constrain(fadeValue[led], 0.0, 255.0);
  delay(delai);  
}

void DimDown(int delai) {
  increment[led] = constrain(--increment[led], 0, 1024);
  fadeValue[led] = round(pow(2.0,increment[led]/128.0)-1);
  fadeValue[led] = constrain(fadeValue[led], 0.0, 255.0);
  delay(delai);   
} 

void DimZero() {
  increment[led] = 0;
  if (fadeValue[led] > 0.0) {
    fadeValue[led] = constrain(fadeValue[led]--, 0.0, 255.0);
    delay(5);  
  }
  else {
   fadeValue[led] = 0.0; 
  }  
} 

void XbeeFlushData(){
  data0 = 0;
  data1 = 0;
  data2 = 0;
}  

void flashled() {
  static long previousMillis = 0;
  static int ledState = LOW;  
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > 1000) {
    previousMillis = currentMillis;   

    if (ledState == LOW) 
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin13, ledState);
  }  
}  

void loop()
{
   //******************************* 1er bouton ******************************** 
   button1.Update();
   
   if(button1.click == CLICK_SINGLECLICKED) {
     led = 0; //0 pour 1er set de led (zero indexed)
     DimZero();
     XbeeFlushData();
   } 
   if(button1.click == CLICK_SINGLEHOLD) {
     led = 0;
     DimUp(5); 
     XbeeFlushData();
   }
   //***************************************************************************

   //******************************* 2e bouton ********************************* 
   button2.Update();
   
   if(button2.click == CLICK_SINGLECLICKED) {
     led = 1; //1 pour 2e set de led (zero indexed)
     DimZero();
     XbeeFlushData();   
   } 
   if(button2.click == CLICK_SINGLEHOLD) {
     led = 1;
     DimUp(5);
     XbeeFlushData();
   }
   //************************************************************************
     
   //Lecture packets Xbee
   xbee.readPacket();

   if (xbee.getResponse().isAvailable()) {
     if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
        xbee.getResponse().getRx16Response(rx16);
        data0 = rx16.getData(0);
        data1 = rx16.getData(1);
        data2 = rx16.getData(2);       
     }
   } 
   
   //Bytes venant de Xbee ex: 15 - 03 - 127
   //15 --> byte de welcome, 03 --> adresse de la led a dimmer, 127 --> taux de modulation (de 0 @ 255)
   //Ici adresses des led "3 et 4" installé dans le coin des armoires. 3 --> 1er set de led
   if ((data0 == 15) && (data1 == 3)) { //Data In Xbee pour 1er set de led (addr. 3)
     led = 0;
   }
   if ((data0 == 15) && (data1 == 4)) { //Data In Xbee pour 2e set de led (addr. 4)
     led = 1;
   }
   
   if ((data0 == 15 && (data1 == 3 || data1 == 4))) {
     button1.click = 0; 
     button2.click = 0; 
     if (data2 == 0) {  
       DimZero();
     } else if (fadeValue[led] < ((double)data2) - 1.0) {
        DimUp(5);  
     } else if (fadeValue[led] > ((double)data2) + 1.0) {
        DimDown(5);
     } else {
        XbeeFlushData();  
     }
     if ((data2 == 0) && (fadeValue[led] == 0.0)) { XbeeFlushData();}
   }
   
   analogWrite(ledPin9, fadeValue[0]); 
   analogWrite(ledPin10, fadeValue[1]);
   
   flashled(); //en signe de vie  
}
