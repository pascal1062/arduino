/*    ClickButton
 
 Arduino library that decodes clicks / pushes on one button
 and returns a function code according to click, doubleclick etc.
 
 Usage: ClickButton buttonObject(pin [LOW/HIGH, [CLICK_PULLUP]]);
 
  where LOW/HIGH denotes active LOW or HIGH button (default is LOW)
  CLICK_PULLUP is only possible with active low buttons.
 
 Returned click codes:
  -3 - tripleclicked (after release of button)
  -2 - doubleclicked (after release of button)
  -1 - singleclicked (after release of button)
   0 - unused (initial value)
   1 - single click, while it is pressed down
   2 - doubleclick, while the second click is pressed down
   3 - tripleclick, same as above just third press of the button
   4 - Single click-and-hold (Simply holding the button down more than CLICK_HELDDOWN_DELAY milliseconds)
   5 - doubleclick-and-hold  (one click then another press-and-hold down quickly afterwards)
   6 - tripleclick-and-hold  (doubleclick then another press-and-hold down quickly afterwards)
 
 NOTE: It returns the last function code used, even if no new button clicks are detected.
       One would have to cope for this in the code (IE by storing the last returned click code).
 
 Based on the Debounce example at arduino playground site
 
 Copyright (C) 2010,2012 raron
 
  
 GNU GPLv3 license
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.



 Contact: raronzen@gmail.com

 
 History:
 2012.01.31 - raron - Tiny update for Arduino 1.0 as well as retaining
                        pre-Arduino 1.0 compatibility. Fixed header comments.
 2010.06.15 - raron - First version. Basically just a small OOP
                        (Object Oriented programming) excersize.
 */

#include "ClickButton.h"

ClickButton::ClickButton(uint8_t buttonPin)
{
  pin = buttonPin;
  pinMode(pin, INPUT);
  // Some initial values
  active = LOW;          // Assume active-low button
  reading = !active;
  lastReading = reading;
  click = 0;
  buttonPressNr = 0;
  debouncedButton = 0;
  lastBounceTime = 0;
}


ClickButton::ClickButton(uint8_t buttonPin, boolean active)
{
  pin = buttonPin;
  pinMode(pin, INPUT);
  // Some initial values
  reading = !active;
  lastReading = reading;
  click = 0;
  buttonPressNr = 0;
  debouncedButton = 0;
  lastBounceTime = 0;
}

ClickButton::ClickButton(uint8_t buttonPin, boolean active, boolean internalPullup)
{
  pin = buttonPin;
  pinMode(pin, INPUT);
  // Turn on internal pullup resistor if applicable
  if (active == LOW && internalPullup == CLICK_PULLUP) digitalWrite(pin,HIGH);
  // Some initial values
  reading = !active;
  lastReading = reading;
  click = 0;
  buttonPressNr = 0;
  debouncedButton = 0;
  lastBounceTime = 0;
}



int ClickButton::Update()
{
  reading = digitalRead(pin);
  //Pascal: il faut enlever cette ligne avec une logique HIGH
  //if (!active) reading = !reading;   // Just to make the button logic in code active-high

  long now = (long)millis();         // prevents rollover issue by using signed arithmetic (I think)



  // If the switch changed, due to noise or pressing reset the debouncing timer
  if (reading != lastReading) lastBounceTime = now;

  // If no buttons were pressed within doubleClickDelay milliseconds, start a new buttonpress cycle
  if (now - lastBounceTime > CLICK_MULTICLICK_DELAY) buttonPressNr = 0;

  // Check if a debounced, changed state has occured
  if (now - lastBounceTime > CLICK_DEBOUNCE_DELAY && reading != debouncedButton)
  {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    debouncedButton = reading;

    // Check if the button is currently pressed down or not
    if (debouncedButton) click = ++buttonPressNr;
    else click = 0 - buttonPressNr;
  }

  // limit number of clicks / button presses
  if (buttonPressNr > CLICK_MAX_BUTTON_PRESSES) buttonPressNr = CLICK_MAX_BUTTON_PRESSES;

  // Check to see if a button is held down for a longer time
  if (debouncedButton && (now - lastBounceTime > CLICK_HELDDOWN_DELAY) && click < (CLICK_MAX_BUTTON_PRESSES+1))
  {
    // Set function according to a preceding doubleclick or not
    click += CLICK_MAX_BUTTON_PRESSES;
  }

  lastReading = reading;
  return click;
}

