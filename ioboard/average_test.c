
int analogInputAvg[8]; 
int readingsAI1[10];
int readingsAI2[10];
int readingsAI3[10];
int readingsAI4[10];
int readingsAI5[10];
int readingsAI6[10];
int readingsAI7[10];
int readingsAI8[10];
byte count = 0;
long previousMillisB = 0;

void readAvgAnalogPin() {  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillisA > 5) {
    previousMillisA = currentMillis;

    if (count > 9) count = 0;
    readingsAI1[count] = analogInput[0];
    readingsAI2[count] = analogInput[1];
    readingsAI3[count] = analogInput[2];
    readingsAI4[count] = analogInput[3];
    readingsAI5[count] = analogInput[4];
    readingsAI6[count] = analogInput[5];
    readingsAI7[count] = analogInput[6];
    readingsAI8[count] = analogInput[7]; 
    count++;   

    analogInputAvg[0] = getAgerage(readingsAI1);
    analogInputAvg[1] = getAgerage(readingsAI2);
    analogInputAvg[2] = getAgerage(readingsAI3);
    analogInputAvg[3] = getAgerage(readingsAI4);
    analogInputAvg[4] = getAgerage(readingsAI5);
    analogInputAvg[5] = getAgerage(readingsAI6);
    analogInputAvg[6] = getAgerage(readingsAI7);
    analogInputAvg[7] = getAgerage(readingsAI8);
    
  }
}

int getAgerage(int readings[10]) {
    int sum = 0;
    int avg = 0;
    
    for (int i = 0; i < 10; i++) {
       sum += readings[i]; 
    }
    avg = sum / 10; 
   
    return avg;
}
