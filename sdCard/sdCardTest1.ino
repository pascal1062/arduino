#include <SD.h>
#include <SPI.h>
 
File myFile;

int myInts[10];
float myFloats[20];
int index=0;
int sum;
float sumF;
 
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
  
  for (int a=0;a<=19;a++) {
    Serial.println(myFloats[a]);
  }  
}
 
void loop()
{
	// nothing happens after setup
}
