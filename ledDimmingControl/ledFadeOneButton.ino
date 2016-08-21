#include "ClickButton.h"

const int ledPin = 9;
const int buttonPin1 = 2;

ClickButton button1(buttonPin1, LOW);

int lastClickCode = 0;
int increment=0;
double fadeValue=0;
boolean x = false;

void setup()
{
  pinMode(buttonPin1, INPUT);
  pinMode(ledPin,OUTPUT);
  Serial.begin(9600);  
}

void loop()
{
  button1.Update();
  
  // 
  if(button1.click == CLICK_SINGLECLICKED) {
    increment = 0;
    x = true;
  } 

  // (must hold down button)
  if(button1.click == CLICK_SINGLEHOLD) {
    x = false;    
    increment = constrain(increment += 1, 0, 1024);
    fadeValue = round(pow(2.0,increment/128.0)-1);
    fadeValue = constrain(fadeValue, 1.0, 255.0);
    //Serial.println(fadeValue);
    delay(5);                            
  } 

  if(x == true) {
    if(fadeValue > 0) {
      fadeValue = constrain(fadeValue--, 0.0, 255.0);
      delay(5);
    }  
    else {
      fadeValue = 0;
      x == false;
    }  
  }
  
  analogWrite(ledPin, fadeValue);   
 
  // Save previous button click code
  lastClickCode = button1.click;
}
