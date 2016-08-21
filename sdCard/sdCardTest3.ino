#include <SD.h>
#include <SPI.h>
 
File myFile;

int myInts[10];
float myFloats[60];
int index=0;
int sum;
float sumF;
float voltIn;
float resultOut;
 
void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);
   digitalWrite(10, HIGH);
 
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  // open the file for reading:
  myFile = SD.open("datalog.txt");
  if (myFile) {
    Serial.println("datalog.txt:");
 
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	//Serial.write(myFile.read());
        //myInts[index] = myFile.read();
        //myInts[index] = myFile.parseInt();
        //myFloats[index] = myFile.parseFloat();
        //myInts[index] = myFile.readStringUntil(',').toInt(); //***non***
        myFloats[index] = myFile.parseFloat(); //2 chiffres apres le point
        index++;
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  //sum = myInts[0] + myInts[1];
  sumF = myFloats[0] + myFloats[1];
  //Serial.println(myInts[5]);
  Serial.println(myFloats[9]);
  //Serial.println(sum);
  Serial.println(sumF);
  
  for (int a=0;a<=59;a++) {
    Serial.println(myFloats[a]);
  } 
  voltIn = 2.71;
  resultOut = scaleRangeCalc(voltIn);
  Serial.println(resultOut);
}
 
void loop()
{
	// nothing happens after setup
}


float scaleRangeCalc(float volt) {
  float result = 0.0;
  bool reverse = true;
  float diffVolt = 0.0;
  float diffCalc = 0.0;
  
  if (myFloats[30] > myFloats[59]) {
    reverse = true;
  } else if (myFloats[30] < myFloats[59]) {
    reverse = false;
  }
  
  if (reverse) { //Calcul de la pente action inverse ex. sonde 10K NTC
    for (int a=0;a<=29;a++) {
      if (volt<=myFloats[59]) {
        result = myFloats[29]; 
      } else if (volt>=myFloats[30]) {
        result = myFloats[0]; 
      } else if ((volt>=myFloats[a+31]) && (volt<=myFloats[a+30])) {
        diffVolt = myFloats[a+30]-myFloats[a+31];
        diffCalc = myFloats[a+1]-myFloats[a];
        result = myFloats[a+1]-((volt-myFloats[a+31])/(diffVolt/diffCalc));
      }
    }     
  } else if (!reverse) { //Calcul de la pente action direct
      for (int a=0;a<=29;a++) {
        if (volt<=myFloats[30]) {
          result = myFloats[0]; 
        } else if (volt>=myFloats[59]) {
          result = myFloats[29]; 
        } else if ((volt>=myFloats[a+30]) && (volt<=myFloats[a+31])) {
          diffVolt = myFloats[a+31]-myFloats[a+30];
          diffCalc = myFloats[a+1]-myFloats[a];
          result = myFloats[a+1]-((myFloats[a+31]-volt)/(diffVolt/diffCalc));
        }
    }    
  }
  return result; 
}
