// Possible commands are listed here:

// Arduino && Vellemann KA05 6 AIs, 6 DIs and 6 DOs

// "digitalOutput/1/1"     -> set digital Output #1 to ON (possible values are 1 to 6)
// "digitalOutput/2/0"     -> set digital Output #2 to OFF (possible values are 1 to 6)
// "digitalInput/1"        -> read digital Input #1 (possible values are 1 to 6)
// "digitalInput/2"        -> read digital Input #2 (possible values are 1 to 6)
// "analogInput/1"        -> read analog Input #1 (possible values are 1 to 6)
// "analogInput/2"        -> read analog Input #2 (possible values are 1 to 6)

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <FileIO.h>

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

// Array of constant that hold pins number
const int digitalOutputPin[] = {8, 9, 10, 11, 12, 13}; //Arduino Uno pin 8 to 12
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

// Arrays of values for scale ranges calculations. Those arrays will be filled from file on sd card.
float scaleRangeAI1[60];
float scaleRangeAI2[60];
float scaleRangeAI3[60];
float scaleRangeAI4[60];
float scaleRangeAI5[60];
float scaleRangeAI6[60];

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

void setup() {
  pinMode(8, OUTPUT); //DO_0
  pinMode(9, OUTPUT); //DO_1
  pinMode(10, OUTPUT); //DO_2
  pinMode(11, OUTPUT); //DO_3
  pinMode(12, OUTPUT); //DO_4
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

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  FileSystem.begin();
  
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
  if (client) {
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
}


void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "digitalOutput" command?
  if (command == "digitalOutput") {
    digitalOutCommand(client);
  }

  // is "digitalIntput" command?
  if (command == "digitalInput") {
    digitalInRead(client);
  }

  // is "analog" command?
  if (command == "analogInput") {
    analogInRead(client);
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
  
  if (command == "adValue") {
    num = client.parseInt();
    value = analogInAdValue[num-1];
  } 

  // Send feedback to client
  client.println(value);
  
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
   
   if(currentMillis - previousMillisC > 800) {
     previousMillisC = currentMillis;
  
     for (int a=0; a<=5; a++) {
       newValue[a] = analogInAdValue[a] * (5.0/1023.0);
       analogInVoltValue[a] = lastValue[a] + (0.1 * (newValue[a] - lastValue[a]));
       lastValue[a] = newValue[a];
     }
     analogInValue[0] = scaleRangeCalc(analogInVoltValue[0],1);
   }  
}


void digitalStatusRead() {
  for (int a=0; a<=5; a++) {
    digitalInput[a] = digitalRead(digitalInputPin[a]);
 }
  digitalOutput[0] = bitRead(PORTB,0); 
  digitalOutput[1] = bitRead(PORTB,1);
  digitalOutput[2] = bitRead(PORTB,2);
  digitalOutput[3] = bitRead(PORTB,3);
  digitalOutput[4] = bitRead(PORTB,4);
  digitalOutput[5] = bitRead(PORTB,5);
}

void loadScaleRangeAI1() {
  int index = 0;
  File dataFile = FileSystem.open("/mnt/sd/arduino/scalerangeAI1.txt", FILE_READ);
  
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
  File dataFile = FileSystem.open("/mnt/sd/arduino/scalerangeAI2.txt", FILE_READ);
  
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
  File dataFile = FileSystem.open("/mnt/sd/arduino/scalerangeAI3.txt", FILE_READ);
  
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
  File dataFile = FileSystem.open("/mnt/sd/arduino/scalerangeAI4.txt", FILE_READ);
  
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
  File dataFile = FileSystem.open("/mnt/sd/arduino/scalerangeAI5.txt", FILE_READ);
  
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
  File dataFile = FileSystem.open("/mnt/sd/arduino/scalerangeAI6.txt", FILE_READ);
  
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
  
  if(currentMillis - previousMillisA > 1000) {
    previousMillisA = currentMillis;

    Bridge.put("AI1.Volt", String(analogInVoltValue[0])); //AI 1 to 6  
    Bridge.put("AI2.Volt", String(analogInVoltValue[1]));
    Bridge.put("AI3.Volt", String(analogInVoltValue[2]));
    Bridge.put("AI4.Volt", String(analogInVoltValue[3]));
    Bridge.put("AI5.Volt", String(analogInVoltValue[4]));
    Bridge.put("AI6.Volt", String(analogInVoltValue[5]));
  }
  
  if(currentMillis - previousMillisD > 3000) {
    previousMillisD = currentMillis;
    
    Bridge.put("DI1", String(digitalInput[0])); //DI 1 to 6
    Bridge.put("DI2", String(digitalInput[1]));
    Bridge.put("DI3", String(digitalInput[2]));
    Bridge.put("DI4", String(digitalInput[3]));
    Bridge.put("DI5", String(digitalInput[4]));
    Bridge.put("DI6", String(digitalInput[5]));
    
    Bridge.put("DO1", String(digitalOutput[0])); //DO 1 to 6
    Bridge.put("DO2", String(digitalOutput[1]));
    Bridge.put("DO3", String(digitalOutput[2]));
    Bridge.put("DO4", String(digitalOutput[3]));
    Bridge.put("DO5", String(digitalOutput[4]));
    Bridge.put("DO6", String(digitalOutput[5]));
  } 
 
} 


