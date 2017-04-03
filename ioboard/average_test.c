
int analogInputAvg1[10];
int count = 0;

void readAvgAnalogIn() {  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > 2) {
    previousMillis = currentMillis;

    //after 10 count reset to  0 and count again...
    analogInputAvg1[count] = analogInput[0];
    analogInputAvg2[count] = analogInput[2];
    analogInputAvg3[count] = analogInput[3];
    .....and so on to input #8


    analogInputAvg[0] = averageCalc(0);  //moyenne des 10 cases....
    analogInputAvg[1] = averaIngeCalc(1);  //moyenne des 10 cases....
    analogInputAvg[2] = averaIngeCalc(2);  //moyenne des 10 cases....
    .....and so on to input #8
  }
 

}


int averageCalc(byte inputNumber) {
    
    for (int i = 0; i < 10; i++) {
       total= total + analogInputAvg1[i]; 
       average = total / 10; 
    }
    
    return average;
    
}
