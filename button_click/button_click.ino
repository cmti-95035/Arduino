/*
 Debounce

 Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
 press), the output pin is toggled from LOW to HIGH or HIGH to LOW.  There's
 a minimum delay between toggles to debounce the circuit (i.e. to ignore
 noise).

 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached from pin 2 to +5V
 * 10K resistor attached from pin 2 to ground

 * Note: On most Arduino boards, there is already an LED on the board
 connected to pin 13, so you don't need any extra components for this example.


 created 21 November 2006
 by David A. Mellis
 modified 30 Aug 2011
 by Limor Fried
 modified 28 Dec 2012
 by Mike Walters

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Debounce
 */

/* Serial port to use for printing informational messages to the user */
#define LOG_SERIAL Serial

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 8;    // the number of the pushbutton pin

// Variables will change:
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 1500;    // the debounce time; increase if the output flickers

void setup() {
  LOG_SERIAL.begin(9600);
  pinMode(buttonPin, INPUT);
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
//  LOG_SERIAL.print("reading:"); LOG_SERIAL.println(reading);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState && reading != LOW) {
    // reset the debouncing timer
    lastDebounceTime = millis();
      LOG_SERIAL.print("reading changed. lastButtonState:"); LOG_SERIAL.println(lastButtonState);
            LOG_SERIAL.print("lastDebounceTime:"); LOG_SERIAL.println(lastDebounceTime);
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;    
      LOG_SERIAL.print("buttonState changed. buttonState:"); LOG_SERIAL.println(buttonState);
    }   
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

