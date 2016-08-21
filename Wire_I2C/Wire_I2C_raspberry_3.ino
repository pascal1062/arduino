#include <Wire.h>

#define SLAVE_ADDRESS 0x12

#define FLOATS_SENT 2

float temperature = 10.5;
float luminosity = 5.2;
float data[FLOATS_SENT];
byte bytes[] = {12,24,45,234,123,67,34,87};

void setup() {
    pinMode(13, OUTPUT);
    Serial.begin(9600);
    
    data[0] = temperature;
    data[1] = luminosity;
    
    // initialize i2c as slave
    Wire.begin(SLAVE_ADDRESS);

    // define callbacks for i2c communication
    Wire.onRequest(sendData);
}

void loop() {
    delay(100);
}

void sendData(){
  //Wire.write((byte*) &temperature, FLOATS_SENT*sizeof(float));
  Wire.write((byte*) &bytes, sizeof(bytes));
}
