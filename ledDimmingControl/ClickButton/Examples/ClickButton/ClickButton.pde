// ClickButton library example
//
// Usage: ClickButton buttonObject(pin [, active [,CLICK_PULLUP]]);
//
// where:
//   pin is the pin connected to the button
//   active is either LOW or HIGH, denoting an active LOW or HIGH button (default is LOW)
//   CLICK_PULLUP is only possible with active low buttons, using the Atmega's internal pullup resistor
//
// The circuit:
// - LED attached from pin 13 to resistor (say 220-ish ohms), other side of resistor to GND (ground)
// - pushbutton attached from pin 2 to GND
// No pullup needed, using the internal in this example.
//
// Based on the Arduino Debounce example.


#include "ClickButton.h"


// the LED
const int ledPin = 13;
int ledState = 0;

// the Button
const int buttonPin1 = 2;
ClickButton button1(buttonPin1, LOW, CLICK_PULLUP);

// Needs to store the previous returned button click code
int lastClickCode = 0;


void setup()
{
  pinMode(ledPin,OUTPUT);  
}


void loop()
{
  // update button state
  // Note: Will return the last click decoded, even if no new clicks are received!
  button1.Update();
  
  // Toggle LED on single clicks. But only if the click code is new
  if(button1.click == CLICK_SINGLECLICK && button1.click != lastClickCode) ledState = !ledState;

  // slow blink (must hold down button. 1 second long blinks)
  if(button1.click == CLICK_SINGLEHOLD) ledState = (millis()/1000)%2;

  // slower blink (must hold down button. 2 second loong blinks)
  if(button1.click == CLICK_DOUBLEHOLD) ledState = (millis()/2000)%2;

  // even slower blink (must hold down button. 3 second looong blinks)
  if(button1.click == CLICK_TRIPLEHOLD) ledState = (millis()/3000)%2;

  // blink faster if double clicked
  if(button1.click == CLICK_DOUBLECLICKED) ledState = (millis()/500)%2;

  // blink even faster if triple clicked
  if(button1.click == CLICK_TRIPLECLICKED) ledState = (millis()/200)%2;



  // update the LED
  digitalWrite(ledPin,ledState);

  // Save previous button click code
  lastClickCode = button1.click;
}




