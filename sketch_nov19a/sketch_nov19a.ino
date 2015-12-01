

/*
A majority of code is taken from the examples provided by the neopixel and music maker library/examples from Adafruit.
The code runs as a state machine, with each question having its own state, followed by a period where the program will wait until the switch is thrown, causing the
interrupt function to fire. This will allow the waiting period to continue into the next question. This switch/interrupt is used in place of the sound sensor which was not implemented
due to jitter and inaccurate readings. Methods of mitigating the jitter suggested by Wendy are averaging or adding a capacitor, which are valid and worth a few trials.
The speaker level is quiet, one way to address this is to use a seperate speaker that has its own amplifier and power source and using the 3.5mm headphone jack attached to the board.
Libraries must be installed before uploading this code to the Arduino.

Guide to adding libraries: https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use/how-to-install-a-library
Neopixel Library: https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library
  -used  "simple" example
Music Maker Library: https://github.com/adafruit/Adafruit_VS1053_Library/archive/master.zip
  -used "player_simple" example
Sound Sensor info and example code:  http://www.seeedstudio.com/wiki/Grove_-_Sound_Sensor

pins for audio: 13,12,11,7,6,4,3 
pins for neopixel: 5 likely must be a pwm pin
pins for sound sensor: A0 can be changed

Color on the Neopixel ring can be changed by the function:
neopixel_object_name.setPixelColor(index_of_pixel, neopixel_object_name.Color(red_value, green_value, blue_value));  //RGB value from 0-255
neopixel_object.show();  //must use this function to update neopixel lights and display their colors. Can look at other examples for other methods of coloring

*/


/* Jian's Code
//assume the 3-pin RGB LED is on the following pins
int redPin = 11;
int greenPin = 10;
int bluePin = 9;

void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop()
{
  // use each 6 bits of the audio sensor input for RGB control
  int red = audiooutput & 0x3f;
  int green = (audiooutput >> 6) & 0x3f;
  int blue = (audiooutput >> 12) & 0x3f;

  setColor(red, green, blue);
  //repeat every 3 seconds
  delay(3000);
}

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, 255-red);
  analogWrite(greenPin, 255-green);
  analogWrite(bluePin, 255-blue);
}
*/

#include <Adafruit_NeoPixel.h>//neo pixel stuff
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN            5
#define NUMPIXELS      24

#define SOUND_SENSOR A0 //sound sensor pin
#define micPin A0
#define THRESHOLD_VALUE 250

#include <SPI.h> //mp3 player  stuff
#include <Adafruit_VS1053.h>
#include <SD.h>
#define SHIELD_RESET  -1
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6
#define CARDCS 4
#define DREQ 3

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //create neopixel object
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS); //create mp3 player object
int counter;
int state = 0; //determines state of state machine
//int sensorValue = 0; //microphone reading
//int average = 500;
boolean canProceed = false;

void setup() {
  pinMode(A0, INPUT);
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++) { //set all pixels to off
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  Serial.begin(9600);
  if (! musicPlayer.begin()) { // initialize the music player
    while (1);
  }
  SD.begin(CARDCS);
  musicPlayer.setVolume(00, 00); //(left,right), lower numbers means louder, currently at loudest
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  attachInterrupt(DigitalPinToInterrupt(2), proceed, CHANGE); //attach an interrupt so that if pin 2 value is changed, will run the proceed() function
}

void proceed() { //changes variable to allow state machine to continue from wait state
  canProceed = true;
  delay(100);
}

void loop() {
  switch (state) {
    case 0: //ask question about playground
      delay(1000);
      for (int i = 0; i < NUMPIXELS; i++) {//light blue
        pixels.setPixelColor(i, pixels.Color(20, 30, 20));
      }
      pixels.show();
      Serial.println(F("Playing track Playground"));
      musicPlayer.playFullFile("track001.mp3"); //file must be renamed as "track001.mp3", other file names did not work
      break;
      
    case 1: //ask question about oreos at lunchtime
      delay(500);
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(40, 30, 40)); //pink
      }
      pixels.show();
      Serial.println(F("Playing track Oreo Cookie"));
      musicPlayer.playFullFile("track002.mp3");
      break;
      
    case 2: //ask question about math class
      delay(500);
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(40, 40, 70)); //white purple
      }
      pixels.show();
      Serial.println(F("Playing Math"));
      musicPlayer.playFullFile("track003.mp3");
      break;
      
    case 3: //ask question about helping girl at library
      delay(500);
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(30, 30, 10)); //yellow
      }
      pixels.show();
      Serial.println(F("Playing Library"));
      musicPlayer.playFullFile("track004.mp3");
      break;
      
    case 4: //ask question about dropping ice cream
      delay(500);
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(30, 50, 20)); //light green
      }
      pixels.show();
      Serial.println(F("Playing Ice Cream Fell"));
      musicPlayer.playFullFile("track005.mp3");
      canProceed = false;
      break;

    case 5://end of questioning, will show purple
      while (!canProceed) {
        for (int i = 0; i < 24; i = i + 2) {
          pixels.setPixelColor(i, pixels.Color(100, 40, 100) );
        }
        for (int i = 1; i < 24; i = i + 2) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0) );
        }
        pixels.show();
        delay(1000);
        for (int i = 0; i < 24; i = i + 2) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0) );
        }
        for (int i = 1; i < 24; i = i + 2) {
          pixels.setPixelColor(i, pixels.Color(100, 40, 100) );
        }
        pixels.show();
        delay(1000);
        Serial.println("ended");
        if (canProceed) { //flipping switch again will cause the program to start questions over
          state = 0;
          break;
        }
      }
  }

  while (!canProceed) { //waiting for switch
    for (int i = 0; i < 24; i = i + 2) {//lights scrolling, waiting for response
      pixels.setPixelColor(i, pixels.Color(30, 127, 200) );
    }
    for (int i = 1; i < 24; i = i + 2) {
      pixels.setPixelColor(i, pixels.Color(100, 200, 50) );
    }
    pixels.show();
    delay(200);
    for (int i = 0; i < 24; i = i + 2) {
      pixels.setPixelColor(i, pixels.Color(100, 200, 50) );
    }
    for (int i = 1; i < 24; i = i + 2) {
      pixels.setPixelColor(i, pixels.Color(30, 127, 200) );
    }
    pixels.show();
    delay(200);
    Serial.println("waiting");
  }
  state++;
  canProceed = false;
  Serial.println(state);
  /* sound sensor, couldnt get to work
  sensorValue = analogRead(SOUND_SENSOR);
  int prevVal[] = {500, 500, 500, 500};
  while (sensorValue > THRESHOLD_VALUE && prevVal[0] > THRESHOLD_VALUE && prevVal[1] > THRESHOLD_VALUE && prevVal[2] > THRESHOLD_VALUE && prevVal[3] > THRESHOLD_VALUE && average > THRESHOLD_VALUE) {
    //lights should be stationary
    Serial.print(sensorValue);
    Serial.println(" checkingSilence");
    sensorValue = analogRead(SOUND_SENSOR);
    delay(100);
  }
  sensorValue = analogRead(SOUND_SENSOR);
  while (sensorValue < THRESHOLD_VALUE && prevVal[0] < THRESHOLD_VALUE && prevVal[1] < THRESHOLD_VALUE && prevVal[2] < THRESHOLD_VALUE && prevVal[3] < THRESHOLD_VALUE && average < THRESHOLD_VALUE) {
    sensorValue = analogRead(SOUND_SENSOR);
    average = (prevVal[0] + prevVal[1] + prevVal[2] + prevVal[3] + sensorValue) / 5;
    for (int i = 0; i < sizeof(prevVal) - 2; i++) {
      prevVal[i] = prevVal[i + 1];
    }
    prevVal[3] = sensorValue;
    delay(100);
    //lights should be thinking
  }
  state++;
  */

}
