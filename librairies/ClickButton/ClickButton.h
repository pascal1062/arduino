#ifndef ClickButton_H
#define ClickButton_H

#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#define CLICK_DEBOUNCE_DELAY 20
#define CLICK_MULTICLICK_DELAY 300
#define CLICK_HELDDOWN_DELAY 1000
#define CLICK_PULLUP HIGH

// For now limit to tripleclicks
#define CLICK_MAX_BUTTON_PRESSES 3

// the decoded functions
#define CLICK_TRIPLECLICKED -3
#define CLICK_DOUBLECLICKED -2
#define CLICK_SINGLECLICKED -1
#define CLICK_SINGLECLICK 1
#define CLICK_DOUBLECLICK 2
#define CLICK_TRIPLECLICK 3
#define CLICK_SINGLEHOLD CLICK_MAX_BUTTON_PRESSES + 1
#define CLICK_DOUBLEHOLD CLICK_MAX_BUTTON_PRESSES + 2
#define CLICK_TRIPLEHOLD CLICK_MAX_BUTTON_PRESSES + 3


class ClickButton
{
  public:
    ClickButton(uint8_t buttonPin);
    ClickButton(uint8_t buttonPin, boolean active);
    ClickButton(uint8_t buttonPin, boolean active, boolean internalPullup);
    int Update();
    int click;                 // Function code to return depending on button presses / clicks
  private:
    uint8_t pin;               // Arduino pin connected to the button
    boolean reading;           // Current button state
    boolean active;            // Type of button - active LOW or HIGH
    boolean lastReading;       // previous button reading
    int buttonPressNr;         // Number of times button is pressed within CLICK_MULTICLICK_DELAY milliseconds
    boolean debouncedButton;   // the currently debounced button state
    long lastBounceTime;       // the last time the button input pin was toggled, due to noise or a press
};

#endif

