
#include <Console.h>
#include <FileIO.h>

float myFloats[60];
int index=0;

void setup() {
  Bridge.begin();   // Initialize the Bridge
  Console.begin();
  FileSystem.begin();
  
  // Wait for Console port to connect
  while (!Console);
  
  Console.println("Test sur la sd card");
}

void loop() {
  File myFile = FileSystem.open("/mnt/sd/arduino/datalog.txt", FILE_READ);
  
  if (myFile) {
    Console.println("datalog.txt:");
 
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
    Console.println("error opening test.txt");
  }
  
  for (int a=0;a<=59;a++) {
    Console.println(myFloats[a]);
  } 

  Console.flush();
  delay(5000);  // wait 5 seconds before you do it again
}
