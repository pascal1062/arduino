// Arduino Mega && Vellemann KA05 6 AIs, 6 DIs and 6 DOs

// Possible http commands are listed here:

// "xxx.xxx.xxx.xxx/arduino/digitaloutput/1/1"          -> Set digital output #1 to ON. 
// "xxx.xxx.xxx.xxx/arduino/digitaloutput/5/0"          -> Set digital output #5 to OFF. 
// "xxx.xxx.xxx.xxx/arduino/digitalinput/1"             -> Read digital input #1. Result ON or OFF. 
// "xxx.xxx.xxx.xxx/arduino/analoginput/advalue/1"      -> Read analog input #1 raw A/D value. Result 0 to 1024
// "xxx.xxx.xxx.xxx/arduino/analoginput/value/6"        -> Read analog input #6 scaled value, ex. result -40 to 105 DegC (no units)
// "xxx.xxx.xxx.xxx/arduino/analoginput/calibration/1"  -> Get and Reload analog inputs calibrations values from linux file. All 6 inputs in column view. 
// "xxx.xxx.xxx.xxx/arduino/scalerange/config/1"        -> Get analog input #1 scale range from linux file. 60 rows in column view.
// "xxx.xxx.xxx.xxx/arduino/scalerange/reload/1"        -> Reload analog input #1 scale range from linux file. Do this after files change.

// JSON returned values
// "xxx.xxx.xxx.xxx/data/get"                           -> Get all 6 AIs, 6 DIs and 6 DOs values in JSON string.
// "xxx.xxx.xxx.xxx/data/get/AI6"                       -> Get analog input #6 scaled value in JSON string, ex. result {"value":"20.54","key":"AI6","response":"get"}
// "xxx.xxx.xxx.xxx/data/get/DI2"                       -> Get digital input #2 value in JSON string. ex. result {"value":"0","key":"DI2","response":"get"}
// "xxx.xxx.xxx.xxx/data/get/DO3"                       -> Get digital output #3 state. ex. result {"value":"1","key":"DO3","response":"get"}


//2015-05-30 je n'utilise plus la barette USB... c'est elle qui tire trop de courant et fait chauffer le régulateur du Arduino.
//j'ai changé le chemin d'acces des fichiers scale ranges...je les mets direct dans le linux sous le USER root...a la place de la clef USB


#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <FileIO.h>

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

// Array of constant that hold pins number
const int digitalOutputPin[] = {22, 24, 26, 28, 30, 32}; //Arduino Mega pin 22-24-26-28-30-32
const int digitalInputPin[] = {2, 3, 4, 5, 6, 7}; //Arduino Uno pin 2 to 7
const int analogInputPin[] = {0, 1, 2, 3, 4, 5}; //Arduino Uno analog pin 0 to 5

// Array for holding digital values 
int digitalInput[6];
int digitalOutput[6];

// Analog Inputs Arrays of "Average" Values and volt caculations
int analogInAdValue[6]; 
float analogInVoltValue[6]; 
float analogInValue[6]; 
float newValue[6]; 
float lastValue[6];

// Arrays of values for scale ranges calculations. Those arrays will be filled from linux file on usb key.
float scaleRangeAI1[60];
float scaleRangeAI2[60];
float scaleRangeAI3[60];
float scaleRangeAI4[60];
float scaleRangeAI5[60];
float scaleRangeAI6[60];
float calibration[6] = {0.0,0.0,0.0,0.0,0.0,0.0}; //calibration of analog input...ex. -0.1 or -0.3 or 0.2 or 0.4 etc.......

// Misceallanous
long previousMillisA = 0;
int pinCount = 0;
int a = -1;
int inputPin = 0;
int total = 0;
int average = 0;
long previousMillisB = 0;
int readings[10];
long previousMillisC = 0;
long previousMillisD = 0;
long previousMillisE = 0;
long previousMillisF = 0;
int count = 0;
int fileCount = -1;

void setup() {
  pinMode(22, OUTPUT); //DO_0 //Arduino Mega pin 22-24-26-28-30-32
  pinMode(24, OUTPUT); //DO_1
  pinMode(26, OUTPUT); //DO_2
  pinMode(28, OUTPUT); //DO_3
  pinMode(30, OUTPUT); //DO_4
  pinMode(32, OUTPUT); //DO_4
  pinMode(7, INPUT); //DI_0
  pinMode(6, INPUT); //DI_1
  pinMode(5, INPUT); //DI_2
  pinMode(4, INPUT); //DI_3
  pinMode(3, INPUT); //DI_4
  pinMode(2, INPUT); //DI_5 
  
  // Bridge startup
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  
  FileSystem.begin();

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();

  loadCalibration();
  loadScaleRangeAI1();
  loadScaleRangeAI2();
  loadScaleRangeAI3();
  loadScaleRangeAI4();
  loadScaleRangeAI5();
  loadScaleRangeAI6(); 
}


void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client.connected()) {
    client.setTimeout(5); //this timeout increases responsiveness of the web service  
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
   
  readAvgAnalogPin();
  analogInFiltVoltValue();
  digitalStatusRead();
  updateDataStore();
  delayLed();
  loadFiles();
}


void delayLed() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillisE > 1000) {
    previousMillisE = currentMillis;   
    flashled();
  }
}

void flashled() { //On board led Pin #13
  static int ledState = LOW; 

  if (ledState == LOW) 
    ledState = HIGH;
  else
    ledState = LOW;
  digitalWrite(13, ledState);  
}


void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "digitalOutput" command?
  if (command == "digitaloutput") {
    digitalOutCommand(client);
  }

  // is "digitalIntput" command?
  if (command == "digitalinput") {
    digitalInRead(client);
  }

  // is "analog" command?
  if (command == "analoginput") {
    analogInRead(client);
  }
  
  // is "scaleRange" command?
  if (command == "scalerange") {
    scaleRangeRead(client);
  }  

}


void digitalOutCommand(YunClient client) {
  int num, value;

  // Read output number 
  num = client.parseInt(); 

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(digitalOutputPin[num-1], value); //digital write on pin matching the array substract -1 
     
    // Send feedback to client
    if (value >= 1) {
      client.println("ON");  
    } else if ((value <= 0)) {
      client.println("OFF");
    }
  }
  
}


void digitalInRead(YunClient client) {
  int num, value;

  // Read intput number 
  num = client.parseInt();  

  value = digitalInput[num-1]; //digital read on pin matching the array substract -1
    
  // Send feedback to client
  if (value == 1) {
    client.println("OFF");  
  } else if ((value == 0)) {
    client.println("ON");
  }
  
}


void analogInRead(YunClient client) {
  String command = client.readStringUntil('/');
  int num; 
  int value;
  float valueF; 
    
  if (command == "advalue") {
    num = client.parseInt();
    value = analogInAdValue[num-1];
    // Send feedback to client
    client.println(value);
  } 
  
  if (command == "value") {
    num = client.parseInt();
    valueF = analogInValue[num-1];
    // Send feedback to client
    client.println(valueF);
  }   

  if (command == "calibration") {
    loadCalibration(); //reload file automatically 
    // Send feedback to client
    for (int a=0;a<=5;a++) {
      client.println(calibration[a]);
    }
  } 
}


void scaleRangeRead(YunClient client) {
  String command = client.readStringUntil('/');
  int num;
 
  if (command == "config") {
    num = client.parseInt();  
  
    for (int a=0;a<=59;a++) {
      if (num==1) {
        client.println(scaleRangeAI1[a]);
      } else if (num==2) {
        client.println(scaleRangeAI2[a]);
      } else if (num==3) {
        client.println(scaleRangeAI3[a]);
      } else if (num==4) {
        client.println(scaleRangeAI4[a]);
      } else if (num==5) {
        client.println(scaleRangeAI5[a]);
      } else if (num==6) {
        client.println(scaleRangeAI6[a]);
      } 
    }    
  }
 
  if (command == "reload") {
    num = client.parseInt();
   
    if (num==1) {
      loadScaleRangeAI1();
      client.println("done");
    } else if (num==2) {
        loadScaleRangeAI2();
        client.println("done");
    } else if (num==3) {
        loadScaleRangeAI3();
        client.println("done");
    } else if (num==4) {
        loadScaleRangeAI4();
        client.println("done");
    } else if (num==5) {
        loadScaleRangeAI5();
        client.println("done");
    } else if (num==6) {
        loadScaleRangeAI6();
        client.println("done");
    }   
  }
}


void readAvgAnalogPin() {  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillisB > 5) {
    previousMillisB = currentMillis;

    if (a >= 9) {
      a = 0;
      if (pinCount >= 5) pinCount = -1;
        pinCount = pinCount + 1;  
        analogInAdValue[inputPin] = average; 
        total = 0;
        for (int b=0; b<=9; b++) {
          readings[b] = 0;
        }
    } else {
      a++;
    }	 

    inputPin = analogInputPin[pinCount];
    readings[a] = analogRead(inputPin); 
    total= total + readings[a];       
    average = total / 10; 
  
  }
}


void analogInFiltVoltValue() {
   unsigned long currentMillis = millis();
   
   if(currentMillis - previousMillisC > 803) {
     previousMillisC = currentMillis;
  
     for (int a=0; a<=5; a++) {
       newValue[a] = analogInAdValue[a] * (5.0/1023.0);
       analogInVoltValue[a] = lastValue[a] + (0.1 * (newValue[a] - lastValue[a]));
       lastValue[a] = newValue[a];
     }
     
     if (count >= 5) count = -1;
       count = count + 1; 
       analogInValue[count] = (scaleRangeCalc(analogInVoltValue[count],count+1))+calibration[count];
   }  
}


void digitalStatusRead() {
  unsigned long currentMillis = millis();
  
    if(currentMillis - previousMillisD > 27) {
      previousMillisD = currentMillis;
    
    for (int a=0; a<=5; a++) {
      digitalInput[a] = digitalRead(digitalInputPin[a]);
    }
      digitalOutput[0] = bitRead(PORTA,0); //Arduino Mega read Output pin state
      digitalOutput[1] = bitRead(PORTA,2);
      digitalOutput[2] = bitRead(PORTA,4);
      digitalOutput[3] = bitRead(PORTA,6);
      digitalOutput[4] = bitRead(PORTC,7);
      digitalOutput[5] = bitRead(PORTC,5);
   }
}

void loadCalibration() {
  int index = 0;
  File dataFileA = FileSystem.open("/root/personal/analogInCalib.txt", FILE_READ);
  
  if (dataFileA) {
     // read from the file until there's nothing else in it:
    while (dataFileA.available()) {
      calibration[index] = dataFileA.parseFloat(); 
      index++;
    }
    // close the file:
    dataFileA.close();
  }  
}

void loadScaleRangeAI1() {
  int index = 0;
  File dataFileB = FileSystem.open("/root/personal/scalerangeAI1.txt", FILE_READ);
  
  if (dataFileB) {
     // read from the file until there's nothing else in it:
    while (dataFileB.available()) {
      scaleRangeAI1[index] = dataFileB.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFileB.close();
  }  
}


void loadScaleRangeAI2() {
  int index = 0;
  File dataFileC = FileSystem.open("/root/personal/scalerangeAI2.txt", FILE_READ);
  
  if (dataFileC) {
     // read from the file until there's nothing else in it:
    while (dataFileC.available()) {
      scaleRangeAI2[index] = dataFileC.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFileC.close();
  }  
}


void loadScaleRangeAI3() {
  int index = 0;
  File dataFileD = FileSystem.open("/root/personal/scalerangeAI3.txt", FILE_READ);
  
  if (dataFileD) {
     // read from the file until there's nothing else in it:
    while (dataFileD.available()) {
      scaleRangeAI3[index] = dataFileD.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFileD.close();
  }  
}


void loadScaleRangeAI4() {
  int index = 0;
  File dataFileE = FileSystem.open("/root/personal/scalerangeAI4.txt", FILE_READ);
  
  if (dataFileE) {
     // read from the file until there's nothing else in it:
    while (dataFileE.available()) {
      scaleRangeAI4[index] = dataFileE.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFileE.close();
  }  
}


void loadScaleRangeAI5() {
  int index = 0;
  File dataFileF = FileSystem.open("/root/personal/scalerangeAI5.txt", FILE_READ);
  
  if (dataFileF) {
     // read from the file until there's nothing else in it:
    while (dataFileF.available()) {
      scaleRangeAI5[index] = dataFileF.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFileF.close();
  }  
}


void loadScaleRangeAI6() {
  int index = 0;
  File dataFileG = FileSystem.open("/root/personal/scalerangeAI6.txt", FILE_READ);
  
  if (dataFileG) {
     // read from the file until there's nothing else in it:
    while (dataFileG.available()) {
      scaleRangeAI6[index] = dataFileG.parseFloat(); //2 chiffres apres le point
      index++;
    }
    // close the file:
    dataFileG.close();
  }  
}

float scaleRangeCalc(float volt, int inst) {
  float result = 0.0;
  bool reverse = true;
  float diffVolt = 0.0;
  float diffCalc = 0.0;
  float sr[60];
  
  for (int a=0;a<=59;a++) {
    if (inst==1) {
      sr[a]=scaleRangeAI1[a];
    } else if (inst==2) {
        sr[a]=scaleRangeAI2[a];
    } else if (inst==3) {
        sr[a]=scaleRangeAI3[a];
    } else if (inst==4) {
        sr[a]=scaleRangeAI4[a];
    } else if (inst==5) {
        sr[a]=scaleRangeAI5[a];
    } else if (inst==6) {
        sr[a]=scaleRangeAI6[a];
    } 
  }
  
  if (sr[30] > sr[59]) {
    reverse = true;
  } else if (sr[30] < sr[59]) {
    reverse = false;
  }
  
  if (reverse) { //Calcul de la pente action inverse ex. sonde 10K NTC
    for (int a=0;a<=29;a++) {
      if (volt<=sr[59]) {
        result = sr[29]; 
      } else if (volt>=sr[30]) {
        result = sr[0]; 
      } else if ((volt>=sr[a+31]) && (volt<=sr[a+30])) {
        diffVolt = sr[a+30]-sr[a+31];
        diffCalc = sr[a+1]-sr[a];
        result = sr[a+1]-((volt-sr[a+31])/(diffVolt/diffCalc));
      }
    }     
  } else if (!reverse) { //Calcul de la pente action direct
      for (int a=0;a<=29;a++) {
        if (volt<=sr[30]) {
          result = sr[0]; 
        } else if (volt>=sr[59]) {
          result = sr[29]; 
        } else if ((volt>=sr[a+30]) && (volt<=sr[a+31])) {
          diffVolt = sr[a+31]-sr[a+30];
          diffCalc = sr[a+1]-sr[a];
          result = sr[a+1]-((sr[a+31]-volt)/(diffVolt/diffCalc));
        }
    }    
  }
  return result; 
}


void updateDataStore() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillisA > 3500) {
    previousMillisA = currentMillis;

    Bridge.put("AI1", String(analogInValue[0])); //AI 1 to 6  
    Bridge.put("AI2", String(analogInValue[1]));
    Bridge.put("AI3", String(analogInValue[2]));
    Bridge.put("AI4", String(analogInValue[3]));
    Bridge.put("AI5", String(analogInValue[4]));
    Bridge.put("AI6", String(analogInValue[5]));
   
    Bridge.put("DI1", String(!digitalInput[0])); //DI 1 to 6
    Bridge.put("DI2", String(!digitalInput[1]));
    Bridge.put("DI3", String(!digitalInput[2]));
    Bridge.put("DI4", String(!digitalInput[3]));
    Bridge.put("DI5", String(!digitalInput[4]));
    Bridge.put("DI6", String(!digitalInput[5]));
    
    Bridge.put("DO1", String(digitalOutput[0])); //DO 1 to 6
    Bridge.put("DO2", String(digitalOutput[1]));
    Bridge.put("DO3", String(digitalOutput[2]));
    Bridge.put("DO4", String(digitalOutput[3]));
    Bridge.put("DO5", String(digitalOutput[4]));
    Bridge.put("DO6", String(digitalOutput[5]));
  } 
 
} 


void loadFiles() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillisF > 5000) {
    previousMillisF = currentMillis;   
    
    if (fileCount >= 7) {
      fileCount = 7;
    } else {
      fileCount++;
    }

    if (fileCount == 0) {
      loadCalibration(); 
    } else if (fileCount == 1) {
      loadScaleRangeAI6(); 
    } else if (fileCount == 2) {
      loadScaleRangeAI5(); 
    } else if (fileCount == 3) {
      loadScaleRangeAI4(); 
    } else if (fileCount == 4) {
      loadScaleRangeAI3(); 
    } else if (fileCount == 5) {
      loadScaleRangeAI2(); 
    } else if (fileCount == 6) {
      loadScaleRangeAI1();
    }
  }
}
