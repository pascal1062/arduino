#include <Wire.h>

#define SLAVE_ADDRESS 0x12
int dataReceived = 0;
int fisrtByte = 0;
int val = 0;
int idx = 0;
byte analogInput[] = {0,17,34,51,68,85,102,119,136,153,170,187,204,221,238,255}; //simulate reading of an 8 analog input high/low bytes

void setup() {
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendAnalogIn2);
    
    Serial.println("Ready");
}

void loop() {
    val = analogRead(0);
    delay(10);
}

void receiveData(int byteCount){
    while(Wire.available()) {
        dataReceived = Wire.read();
        //fisrtByte = Wire.read();
        Serial.print("Donnee recue : ");
        Serial.println(dataReceived);
        if (dataReceived == 0xAA) {
          idx = 0;
        }
    }
}

void sendData(){
    int envoi = dataReceived + 1;
    Wire.write(envoi);
}

void sendAnalogIn() {
    byte high = (val >> 8) & 0xFF;
    byte low = val & 0xFF;
    Wire.write(high);
    Wire.write(low);
    //Wire.write((uint8_t *)&val, sizeof(val));
    Serial.println(val);
}

void sendAnalogIn2() {
  Wire.write(analogInput[idx]);
  idx++;
  if (idx>=15) {
    idx=0;
  }
  Serial.println(analogInput[idx]);
}


