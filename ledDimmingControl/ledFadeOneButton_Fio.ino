#include <ClickButton.h>

const int ledPin9 = 9;
const int ledPin10 = 10;
const int ledPin13 = 13;
const int buttonPin2 = 2;
const int buttonPin3 = 3;

ClickButton button1(buttonPin2, HIGH);
ClickButton button2(buttonPin3, HIGH);

int lastClickCode1 = 0;
int increment1=0;
double fadeValue1=0;
int lastClickCode2 = 0;
int increment2=0;
double fadeValue2=0;
boolean x = false;
boolean y = false;

void setup()
{
  pinMode(ledPin9,OUTPUT);
  pinMode(ledPin10,OUTPUT);
  pinMode(ledPin13,OUTPUT);  
  Serial.begin(9600);  
}

void DimLed1()
{
  button1.Update();
  
  // 
  if(button1.click == CLICK_SINGLECLICKED) {
    increment1 = 0;
    x = true;
  } 

  // (must hold down button)
  if(button1.click == CLICK_SINGLEHOLD) {
    x = false;    
    increment1 = constrain(increment1 += 1, 0, 1024);
    fadeValue1 = round(pow(2.0,increment1/128.0)-1);
    fadeValue1 = constrain(fadeValue1, 1.0, 255.0);
    //Serial.println(fadeValue1);
    delay(5);                            
  } 

  if(x == true) {
    if(fadeValue1 > 0) {
      fadeValue1 = constrain(fadeValue1--, 0.0, 255.0);
      delay(5);
    }  
    else {
      fadeValue1 = 0;
      x == false;
    }  
  }
  analogWrite(ledPin9, fadeValue1);   
  
  // Save previous button click code
  lastClickCode1 = button1.click;  
}

void DimLed2()
{
  button2.Update();
  
  // 
  if(button2.click == CLICK_SINGLECLICKED) {
    increment2 = 0;
    y = true;
  } 

  // (must hold down button)
  if(button2.click == CLICK_SINGLEHOLD) {
    y = false;    
    increment2 = constrain(increment2 += 1, 0, 1024);
    fadeValue2 = round(pow(2.0,increment2/128.0)-1);
    fadeValue2 = constrain(fadeValue2, 1.0, 255.0);
    //Serial.println(fadeValue2);
    delay(5);                            
  } 

  if(y == true) {
    if(fadeValue2 > 0) {
      fadeValue2 = constrain(fadeValue2--, 0.0, 255.0);
      delay(5);
    }  
    else {
      fadeValue2 = 0;
      y == false;
    }  
  }
  analogWrite(ledPin10, fadeValue2);   
  
  // Save previous button click code
  lastClickCode2 = button2.click;  
}

//void flashled() {
//   digitalWrite(ledPin13, HIGH);   // sets the LED on
//   delay(1000);                  // waits for a second
//   digitalWrite(ledPin13, LOW);    // sets the LED off
//   delay(1000);     
//}

void loop()
{
  DimLed1();
  DimLed2();
  
  //flashled();
}
