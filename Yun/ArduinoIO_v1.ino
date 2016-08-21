// Possible commands are listed here:
//
// "digital/13"     -> digitalRead(13)
// "digital/13/1"   -> digitalWrite(13, HIGH)
// "analog/2/123"   -> analogWrite(2, 123)
// "analog/2"       -> analogRead(2)

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

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

// Array of constant that hold pins number
const int digitalOutputPin[] = {8, 9, 10, 11, 12, 13}; //Arduino Uno pin 8 to 12
const int digitalInputPin[] = {2, 3, 4, 5, 6, 7}; //Arduino Uno pin 2 to 7
const int analogInputPin[] = {0, 1, 2, 3, 4, 5}; //Arduino Uno analog pin 0 to 5

// Array for holding digital values 
int digitalInput[6];
int digitalOuputt[6];

// Analog Inputs Arrays of "Average" Values 
int analogInAdValue[6]; //make a float instead.........
float analogInVoltValue[6];
float analogInPresentValue[6];

// Misceallanous
int lastPin = -1;
int pinCount = -1;

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

  delay(40); // Poll every 50ms
  
  if (pinCount >= 5) pinCount = -1;
  pinCount = pinCount + 1; 

  readAvgAnalogPin(); 
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
    digitalOuputt[num-1] = value; //*****bitRead(PORTB,digitalOutputPin[num-1])
    
    // Send feedback to client
    if (value >= 1) {
      client.println("ON");  
    } else if ((value <= 0)) {
      client.println("OFF");
    }
  }

  // Update datastore key with the current pin value
  //String key = "digitalOutput";
  //key += num;
  //Bridge.put(key, String(value));
}


void digitalInRead(YunClient client) {
  int num, value;

  // Read intput number 
  num = client.parseInt();  

  value = digitalRead(digitalInputPin[num-1]); //digital read on pin matching the array substract -1 
    
  // Send feedback to client
  if (value == 1) {
    client.println("OFF");  
  } else if ((value == 0)) {
    client.println("ON");
  }
 
  // Update datastore key with the current pin value
  //String key = "digitalInput";
  //key += num;
  //Bridge.put(key, String(value));
}


void analogInRead(YunClient client) {
  String command = client.readStringUntil('/');
  int num; 
  float value;
  String key = "";
  
  
  if (command == "adValue") {
    num = client.parseInt();
    value = (float)analogInAdValue[num-1];
    key = "analogInputAdValue";
  } 
    
  if (command == "voltValue") {
    num = client.parseInt();
    value = analogInVoltValue[num-1];
    key = "analogInputVoltValue";
  }

  if (command == "presentValue") {
    num = client.parseInt();
    value = analogInPresentValue[num-1];
    key = "analogInputPresentValue";
  }   

  // Send feedback to client
  client.println(value);

  // Update datastore key with the current pin value
  //key += num;
  //Bridge.put(key, String(value));
}

void readAvgAnalogPin() {
   int inputPin;
   
   inputPin = analogInputPin[pinCount];
  
   if (inputPin != lastPin) {
     int total = 0;
     int average = 0; //make a float instead....
     int readings[10];

     for (int a=0; a<=9; a++) {
       readings[a] = analogRead(inputPin); 
       total= total + readings[a];       
       average = total / 10; 
       delay(1);
     }
     analogInAdValue[inputPin] = average;
     analogInVoltValue[inputPin] = average * (5000.0/1023.0); //millivolts...***5000.0/1023.0*** faire plutot 5/1024 pour des volts???? 
     analogInPresentValue[inputPin] = (analogInVoltValue[inputPin] - 500.0)/10.0; //***500.0)/100.0; temporaire....futur calcul selon scalerange python....
   }
   lastPin = inputPin;
}

void updateDataStore() {
  String key; 
  //*********refaire l'ordre pour avoir une meilleur vue dans le retour en JSON**********peut-etre faire sa au secondes
  for (int a=0; a<5; a++) {
    key = "digitalInput";
    key += (a+1);
    digitalInput[a] = digitalRead(digitalInputPin[a]);
    Bridge.put(key, String(digitalInput[a]));
 
    key = "digitalOutput";
    key += (a+1);
    Bridge.put(key, String(digitalOuputt[a]));
 
    key = "analogInputAdValue";
    key += (a+1);
    Bridge.put(key, String(analogInAdValue[a])); 

    key = "analogInputVoltValue";
    key += (a+1);
    Bridge.put(key, String(analogInVoltValue[a]));

    key = "analogInputPresentValue";
    key += (a+1);
    Bridge.put(key, String(analogInPresentValue[a]));    
  }
  
}
 


