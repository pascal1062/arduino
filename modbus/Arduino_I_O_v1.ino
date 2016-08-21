
/*
  velleman board kit ka05 TEST
*/


int AI_1 = 0;
int AI_2 = 0;
int AI_3 = 0;
int AI_4 = 0;
int AI_5 = 0;
int AI_6 = 0;
String Analog_1 = "Analog Input 1: ";
String Analog_2 = "Analog Input 2: ";
String Analog_3 = "Analog Input 3: ";
String Analog_4 = "Analog Input 4: ";
String Analog_5 = "Analog Input 5: ";
String Analog_6 = "Analog Input 6: ";

int DI_1 = 0;
int DI_2 = 0;
int DI_3 = 0;
int DI_4 = 0;
int DI_5 = 0;
int DI_6 = 0;
String Binary_Input_1 = "Binary Input 1: ";
String Binary_Input_2 = "Binary Input 2: ";
String Binary_Input_3 = "Binary Input 3: ";
String Binary_Input_4 = "Binary Input 4: ";
String Binary_Input_5 = "Binary Input 5: ";
String Binary_Input_6 = "Binary Input 6: ";

int DO_1 = 0;
int DO_2 = 0;
int DO_3 = 0;
int DO_4 = 0;
int DO_5 = 0;
int DO_6 = 0;
  
void setup()
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT);  
}

void loop() {
  
  DI_1 = digitalRead(2);
  DI_2 = digitalRead(3);
  DI_3 = digitalRead(4);
  DI_4 = digitalRead(5);
  DI_5 = digitalRead(6);
  DI_6 = digitalRead(7);
  
  if (DI_1 == 1) { 
    digitalWrite(8, LOW);
  } else {
    digitalWrite(8, HIGH);    
  }    
  
  if (DI_2 == 1) { 
    digitalWrite(9, LOW);
  } else {
    digitalWrite(9, HIGH);    
  }
  
  if (DI_3 == 1) { 
    digitalWrite(10, LOW);
  } else {
    digitalWrite(10, HIGH);    
  }
  
  if (DI_4 == 1) { 
    digitalWrite(11, LOW);
  } else {
    digitalWrite(11, HIGH);    
  }  

  if (DI_5 == 1) { 
    digitalWrite(12, LOW);
  } else {
    digitalWrite(12, HIGH);    
  }   
   
  AI_1 = analogRead(A0);
  AI_2 = analogRead(A1);
  AI_3 = analogRead(A2);
  AI_4 = analogRead(A3);
  AI_5 = analogRead(A4);
  AI_6 = analogRead(A5);  
 
  Serial.println(Binary_Input_1 + DI_1);
  Serial.println(Binary_Input_2 + DI_2);
  Serial.println(Binary_Input_3 + DI_3);
  Serial.println(Binary_Input_4 + DI_4);
  Serial.println(Binary_Input_5 + DI_5);
  Serial.println(Binary_Input_6 + DI_6);

  Serial.println(Analog_1 + AI_1);
  Serial.println(Analog_2 + AI_2);
  Serial.println(Analog_3 + AI_3);
  Serial.println(Analog_4 + AI_4);
  Serial.println(Analog_5 + AI_5);
  Serial.println(Analog_6 + AI_6); 

  delay(1000);         
  
}
