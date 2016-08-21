#include <SD.h>
#include <SPI.h>
 
File myFile;

int idx=-1;

float scaleRangeAI1[60];
float scaleRangeAI2[60];
float scaleRangeAI3[60];
float scaleRangeAI4[60];
float scaleRangeAI5[60];
float scaleRangeAI6[60];

 
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
  
  loadScaleRangeAI1();
  loadScaleRangeAI2();
  loadScaleRangeAI3();
 
}
 
void loop()
{
  if (idx >= 3)  idx=-1;
  idx++;
  
  if (idx==0) {
    Serial.println("index is:"+idx);
    for (int a=0;a<59;a++) {
      Serial.println(scaleRangeAI1[a]);
    }    
  } else if (idx==1) {
      Serial.println("index is:"+idx);
      for (int a=0;a<59;a++) {
        Serial.println(scaleRangeAI2[a]);
      }    
  } else if (idx==3) {
      Serial.println("index is:"+idx);
      for (int a=0;a<59;a++) {
        Serial.println(scaleRangeAI3[a]);
      }     
  }
	 
  delay(10000);
}


void loadScaleRangeAI1() {
  int index = 0;
  File dataFile = SD.open("scaleAI1.txt", FILE_READ);
  
  if (dataFile) {
     // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      scaleRangeAI1[index] = dataFile.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFile.close();
  } 
}

void loadScaleRangeAI2() {
  int index = 0;
  File dataFile = SD.open("scaleAI2.txt", FILE_READ);
  
  if (dataFile) {
     // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      scaleRangeAI2[index] = dataFile.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFile.close();
  } 
}

void loadScaleRangeAI3() {
  int index = 0;
  File dataFile = SD.open("scaleAI3.txt", FILE_READ);
  
  if (dataFile) {
     // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      scaleRangeAI3[index] = dataFile.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFile.close();
  } 
}

void loadScaleRangeAI4() {
  int index = 0;
  File dataFile = SD.open("scaleAI4.txt", FILE_READ);
  
  if (dataFile) {
     // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      scaleRangeAI4[index] = dataFile.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFile.close();
  } 
}

void loadScaleRangeAI5() {
  int index = 0;
  File dataFile = SD.open("scaleAI5.txt", FILE_READ);
  
  if (dataFile) {
     // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      scaleRangeAI5[index] = dataFile.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFile.close();
  } 
}

void loadScaleRangeAI6() {
  int index = 0;
  File dataFile = SD.open("scaleAI6.txt", FILE_READ);
  
  if (dataFile) {
     // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      scaleRangeAI6[index] = dataFile.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFile.close();
  } 
}

 
