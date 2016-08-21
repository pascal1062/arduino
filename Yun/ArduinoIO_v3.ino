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
#include <Process.h>

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

// Analog Inputs Arrays of "Average" Values 
int analogInAdValue[6]; 
float analogInVoltValue[6];  
float newValue[6]; 
float lastValue[6];

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
  startScaleRangeCalc();
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

//******ca marche pas*************
void startScaleRangeCalc() {
  Process p;
  
  p.begin("python");
  p.addParameter("/mnt/sd/arduino/scalerange.py");
  p.runAsynchronously();
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


