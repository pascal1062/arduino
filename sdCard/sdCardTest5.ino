#include <SD.h>
#include <SPI.h>
 
File myFile;
File myFile2;

int myInts[10];
float myFloats[60];
float myFloatsB[60];
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
  
  for (int a=0;a<=59;a++) {
    Serial.println(myFloats[a]);
  }
  
  delay(5000);
  
  index=0;
  myFile2 = SD.open("logdata.txt");
  if (myFile2) {
    Serial.println("logdata.txt:");
 
    // read from the file until there's nothing else in it:
    while (myFile2.available()) {
        myFloatsB[index] = myFile2.parseFloat(); //2 chiffres apres le point
        index++;
    }
    myFile2.close();
  }
  
  Serial.println("");
  for (int a=0;a<=59;a++) {
    Serial.println(myFloatsB[a]);
  }  

}
 
void loop()
{
	// nothing happens after setup
}



