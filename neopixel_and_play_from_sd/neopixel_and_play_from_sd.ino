/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            13

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 2       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

  // Parameter 1 = number of pixels in ring
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
 
void setup() {
  ring.begin();
  ring.setBrightness(5);  // Lower brightness and save eyeballs!
  ring.show(); // Initialize all pixels to 'off'
  
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
//  Serial.println(F("VS1053 found"));
//  
//  SD.begin(CARDCS);    // initialise the SD card
//  
//  // Set volume for left, right channels. lower numbers == louder volume!
//  musicPlayer.setVolume(0,0);
//
//  // Timer interrupts are not suggested, better to use DREQ interrupt!
//  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int
//
//  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
//  // audio playing
//  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
//  
//  // Play one file, don't return until complete
//  Serial.println(F("Playing track 001"));
//  musicPlayer.playFullFile("track001.mp3");
//  // Play another file in the background, REQUIRES interrupts!
//  Serial.println(F("Playing track 002"));
//  musicPlayer.startPlayingFile("track002.mp3");
}

void loop() {

//  clockwise(ring.Color(0,0,0), 25); // Black
//  clockwise(ring.Color(64, 0, 0), 100); // Red
//  clockwise(ring.Color(0, 64, 0), 100); // Green
//  counterClockwise(ring.Color(0, 0, 64), 100); // Blue
//  sineColorWave(75);
//  clockwise(ring.Color(0,0,0), 100); // Black
  rainbow(15);
  clockwise(ring.Color(0,0,0), 100); // Black
  rainbowCycle(15);
//  clockwise(ring.Color(0,0,0), 100); // Black
//  sineColorWave(30);  
  // File is playing in the background
//  if (musicPlayer.stopped()) {
//    Serial.println("Done playing music");
////    //while (1);
//
//      clockwise(ring.Color(0,0,0), 25); // Black
//  clockwise(ring.Color(64, 0, 0), 100); // Red
//  clockwise(ring.Color(0, 64, 0), 100); // Green
//  clockwise(ring.Color(0, 0, 64), 100); // Blue
//  sineColorWave(75);
//  clockwise(ring.Color(0,0,0), 100); // Black
//  rainbow(15);
//  clockwise(ring.Color(0,0,0), 100); // Black
//  rainbowCycle(15);
//  clockwise(ring.Color(0,0,0), 100); // Black
//  sineColorWave(30);
//  }
//  if (Serial.available()) {
//    char c = Serial.read();
//    
//    // if we get an 's' on the serial console, stop!
//    if (c == 's') {
//      musicPlayer.stopPlaying();
//    }
//    
//    // if we get an 'p' on the serial console, pause/unpause!
//    if (c == 'p') {
//      if (! musicPlayer.paused()) {
//        Serial.println("Paused");
//        musicPlayer.pausePlaying(true);
//      } else { 
//        Serial.println("Resumed");
//        musicPlayer.pausePlaying(false);
//      }
//    }
//  }

  delay(100);
}

// light up all LEDs with one color clockwise
void clockwise(uint32_t color, uint8_t wait) {
  for(uint16_t i=0; i<ring.numPixels(); i++) {
      ring.setPixelColor(i, color);
      ring.show();
      delay(wait);
  }
}

// light up all LEDs with one color clockwise
void counterClockwise(uint32_t color, uint8_t wait) {
  for(uint16_t i= ring.numPixels() - 1; i>=0; i--) {
      ring.setPixelColor(i, color);
      ring.show();
      delay(wait);
  }
}

// showing rainbow colors
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel((i+j) & 255));
    }
    ring.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel(((i * 256 / ring.numPixels()) + j) & 255));
    }
    ring.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// generate r-g-b color in a sine wave pattern
void sineColorWave(uint8_t wait) {
  int i, j, ringsize, cycle;
  float ang, rsin, gsin, bsin, offset;

  int tick = 1;
  
  ringsize = ring.numPixels();
  cycle = ringsize * 25; // times around the circle...

  while (++tick < ringsize) {
    offset = map2PI(tick);

    for (i=0; i<ringsize; i++) {
//    for (i = ringsize -1; i>=0;   i++) {
      
      ang = map2PI(i) - offset;
      rsin = sin(ang);
      gsin = sin(2.0 * ang / 3.0 + map2PI(int(ringsize/6)));
      bsin = sin(4.0 * ang / 5.0 + map2PI(int(ringsize/3)));
      if(i==0)
      ring.setPixelColor(0, ring.Color(0, 64, 0));
      else
      ring.setPixelColor(i, ring.Color(trigScale(rsin), trigScale(gsin), trigScale(bsin)));
    }

    ring.show();
    delay(wait);
  }

}

/**
 * Scale a value returned from a trig function to a byte value.
 * [-1, +1] -> [0, 254] 
 * Note that we ignore the possible value of 255, for efficiency,
 * and because nobody will be able to differentiate between the
 * brightness levels of 254 and 255.
 */
byte trigScale(float val) {
  val += 1.0; // move range to [0.0, 2.0]
  val *= 127.0; // move range to [0.0, 254.0]

  return int(val) & 255;
}

/**
 * Map an integer so that [0, ringlength] -> [0, 2PI]
 */
float map2PI(int i) {
  return PI*2.0*float(i) / float(ring.numPixels());
}
