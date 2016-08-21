// Possible commands are listed here:

// Arduino && Vellemann KA05 6 AIs, 6 DIs and 6 DOs

// "digitalOutput/1/1"     -> set digital Output #1 to ON (possible values are 1 to 6)
// "digitalOutput/2/0"     -> set digital Output #2 to OFF (possible values are 1 to 6)
// "digitalInput/1"        -> read digital Input #1 (possible values are 1 to 6)
// "digitalInput/2"        -> read digital Input #2 (possible values are 1 to 6)
// "analogInput/1"        -> read analog Input #1 (possible values are 1 to 6)
// "analogInput/2"        -> read analog Input #2 (possible values are 1 to 6)

// Cette version avec un call vers un process python pour le calcul des Present Value analog Input

// ***** ca marche pas!!!! j'abandonne ******

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
float analogInValue[6];

// Misceallanous
long previousMillis = 0;
int stepDelay = 0;
int pinCount = 0;
int a = -1;
int inputPin = 0;
int total = 0;
int average = 0;
long previousMillis2 = 0;
int readings[10];
long previousMillis3 = 0;
int instance = -1;

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

  delay(50); // Poll every 50ms
 
  readAvgAnalogPin();
  digitalStatusRead();
  //analogInCalcValueExec();
  //updateDataStore();
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
  float valueFl;
  
  if (command == "adValue") {
    num = client.parseInt();
    value = analogInAdValue[num-1];
    client.println(value);
  } 
  
  if (command == "Value") {
    num = client.parseInt();
    analogInCalcValue(num-1, analogInAdValue[num-1]);
    valueFl = analogInValue[num-1];
    client.println(valueFl);
  }

  // Send feedback to client
  //client.println(value);
  
}


void readAvgAnalogPin() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis2 > 5) {
    previousMillis2 = currentMillis;

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


void digitalStatusRead() {
  for (int a=0; a<=5; a++) {
    digitalInput[a] = digitalRead(digitalInputPin[a]);
 }
  digitalOutput[0] = bitRead(PORTH,5); //Arduino Mega read Output pin state
  digitalOutput[1] = bitRead(PORTH,6);
  digitalOutput[2] = bitRead(PORTB,4);
  digitalOutput[3] = bitRead(PORTB,5);
  digitalOutput[4] = bitRead(PORTB,6);
  digitalOutput[5] = bitRead(PORTB,7);
}

void analogInCalcValueExec() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis3 > 1000) {
    previousMillis3 = currentMillis;

  if (instance >= 5) instance = -1;
    instance++;  
    analogInCalcValue(instance, analogInAdValue[instance]); 
  }
}

void analogInCalcValue(int inst, int adValue) {
  Process p; 

  p.begin("python");
  p.addParameter("/mnt/sd/arduino/scale.py");
  p.addParameter(String(adValue));
  p.run();  
  
  while(p.running());
  
  while (p.available()) {
    //analogInValue[inst] = p.parseFloat(); 
    analogInValue[inst] = p.parseInt();    
  }
  //analogInValue[inst] = adValue + 1.0; //la fonction est bien exécuté
} 


void updateDataStore() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > 100) {
    previousMillis = currentMillis;
   
    if (stepDelay >= 18) stepDelay = -1;
      stepDelay++;
  
     
    if (stepDelay==0) {
	Bridge.put("DI1", String(digitalInput[0])); //DI 1 to 6
    } else if (stepDelay==1) {
	Bridge.put("DI2", String(digitalInput[1]));    
    } else if (stepDelay==2) {
	Bridge.put("DI3", String(digitalInput[2])); 
    } else if (stepDelay==3) {
	Bridge.put("DI4", String(digitalInput[3])); 
    } else if (stepDelay==4) {
	Bridge.put("DI5", String(digitalInput[4])); 
    } else if (stepDelay==5) {
	Bridge.put("DI6", String(digitalInput[5]));  
    } else if (stepDelay==6) {
	Bridge.put("DO1", String(digitalOutput[0])); //DO 1 to 6
    } else if (stepDelay==7) {
	Bridge.put("DO2", String(digitalOutput[1]));
    } else if (stepDelay==8) {
	Bridge.put("DO3", String(digitalOutput[2]));
    } else if (stepDelay==9) {
	Bridge.put("DO4", String(digitalOutput[3]));
    } else if (stepDelay==10) {
	Bridge.put("DO5", String(digitalOutput[4]));
    } else if (stepDelay==11) {
	Bridge.put("DO6", String(digitalOutput[5]));
    } else if (stepDelay==12) {
	Bridge.put("AI1.ADValue", String(analogInAdValue[0])); //AI 1 to 6
    } else if (stepDelay==13) {
	Bridge.put("AI2.ADValue", String(analogInAdValue[1]));
    } else if (stepDelay==14) {
	Bridge.put("AI3.ADValue", String(analogInAdValue[2]));
    } else if (stepDelay==15) {
	Bridge.put("AI4.ADValue", String(analogInAdValue[3]));
    } else if (stepDelay==16) {
	Bridge.put("AI5.ADValue", String(analogInAdValue[4]));
    } else if (stepDelay==17) {
	Bridge.put("AI6.ADValue", String(analogInAdValue[5]));    
    } else if (stepDelay==18) {
	Bridge.put("AI1.Value", String(analogInValue[0]));    
    }     
  }
 
} 


