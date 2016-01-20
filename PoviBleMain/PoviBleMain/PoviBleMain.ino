/*
 * Copyright (c) 2015 Intel Corporation.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <BleCharacteristic.h>
#include <BleDevice.h>
#include <BleService.h>
#include <BleCommon.h>
#include <BleDescriptor.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            8

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#include <SPI.h> //mp3 player  stuff
#include <Adafruit_VS1053.h>
#include <SD.h>
#define SHIELD_RESET  -1
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6
#define CARDCS 4
#define DREQ 2

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS); //create mp3 player object
/*
 * This sketch partially implements the standard Bluetooth Low-Energy "Automation IO" service.
 * It is a relatively complex example which demonstrates use of BLE descriptors and
 * multi-instance characteristics for data input and data output.
 *
 * For more information: https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
 */

/* Device name: Appears in advertising packets. Must not exceed 20 characters in length */
#define DEVICE_NAME               "POVI101"
/* UUID for Automation I/O service */
#define SERVICE_UUID_AUTOMATIONIO (0x1815)
/* UUID for digital characteristic */
#define CHAR_UUID_DIGITAL         (0x2A56)
/* UUID for analog characteristic */
#define CHAR_UUID_ANALOG          (0x2A58)
/* UUID for number_of_digitals descriptor */
#define DESC_UUID_NUMDIGITALS     (0x2909)

/* Structure to encapsulate information for each pin
 * This helps us to build lists of pins below to allow
 * flexible pin assignments to different modes of operation
 */
struct DigitalPinConfig {
  unsigned          pin;
  const char        *name;
  BleCharacteristic characteristic;
  BleDescriptor     numDigitalsDesc;
  uint8_t           val;
};

struct AnalogPinConfig {
  unsigned          pin;
  const char        *name;
  BleCharacteristic characteristic;
  uint16_t          val;
};

#define USE_MUSIC_SHIELD  0
/* Macros to simplify the definition of a new PinConfig struct for a given pin number
 * Note that input pins are only readable by the remote device, while output pins are
 * only writable.  Different characteristic UUIDs are used for digital and analog pins */
#define DIGITAL_INPUT_PINCONFIG(pin) \
  { (pin), #pin, {CHAR_UUID_DIGITAL, sizeof(uint8_t), BLE_CLIENT_ACCESS_READ_ONLY, BLE_CLIENT_NOTIFY_ENABLED}, {DESC_UUID_NUMDIGITALS, sizeof(uint8_t), BLE_CLIENT_ACCESS_READ_ONLY} }
#define DIGITAL_OUTPUT_PINCONFIG(pin) \
  { (pin), #pin, {CHAR_UUID_DIGITAL, sizeof(uint32_t), BLE_CLIENT_ACCESS_WRITE_ONLY, BLE_CLIENT_NOTIFY_WITH_ACK}, {DESC_UUID_NUMDIGITALS, sizeof(uint32_t), BLE_CLIENT_ACCESS_READ_ONLY} }
  //{ (pin), #pin, {CHAR_UUID_DIGITAL, sizeof(uint16_t), BLE_CLIENT_ACCESS_WRITE_ONLY, BLE_CLIENT_NOTIFY_WITH_ACK}, {DESC_UUID_NUMDIGITALS, sizeof(uint16_t), BLE_CLIENT_ACCESS_READ_ONLY} }  
#define ANALOG_INPUT_PINCONFIG(pin) \
  { (pin), #pin, {CHAR_UUID_ANALOG, sizeof(uint16_t), BLE_CLIENT_ACCESS_READ_ONLY, BLE_CLIENT_NOTIFY_ENABLED} }
#define ANALOG_OUTPUT_PINCONFIG(pin) \
  { (pin), #pin, {CHAR_UUID_ANALOG, sizeof(uint16_t), BLE_CLIENT_ACCESS_WRITE_ONLY, BLE_CLIENT_NOTIFY_DISABLED} }
//
//#define DIGITAL_OUTPUT_PINCONFIG(pin, name) \
//  { (pin), name, {CHAR_UUID_DIGITAL, sizeof(uint32_t), BLE_CLIENT_ACCESS_WRITE_ONLY, BLE_CLIENT_NOTIFY_WITH_ACK}, {DESC_UUID_NUMDIGITALS, sizeof(uint32_t), BLE_CLIENT_ACCESS_READ_ONLY} }
/* The following lists of pins are configured and presented to
 * the remote BLE device as digital/analog input/output pins
 *
 * These lists can be changed, but with the following caveats:
 * - each pin should only be included once, and cannot be included by multiple lists
 * - only valid pins should be used
 *   - pins 0-19 are valid for digital input/output
 *   - pins 14-19 are valid for analog input
 *   - pins 3,5,6,9 are valid for analog output
 * - a maximum of 16 pins are currently supported (limited by number of characteristics)
 */
struct DigitalPinConfig digitalInputPins[] = {
  DIGITAL_INPUT_PINCONFIG(2),
  DIGITAL_INPUT_PINCONFIG(4),
  DIGITAL_INPUT_PINCONFIG(7),
};

struct DigitalPinConfig digitalOutputPins[] = {
  DIGITAL_OUTPUT_PINCONFIG(11),
  DIGITAL_OUTPUT_PINCONFIG(12),  
  DIGITAL_OUTPUT_PINCONFIG(13),
};
//
//struct DigitalPinConfig digitalOutputPins[] = {
//  DIGITAL_OUTPUT_PINCONFIG(11, "PLAY_AUDIO_PIN"),
//  DIGITAL_OUTPUT_PINCONFIG(12, "FILE_UPLOAD_CONTROL_PIN"),
//  DIGITAL_OUTPUT_PINCONFIG(13, "FILE_DATA_PIN"),
//};

struct AnalogPinConfig analogInputPins[] = {
  ANALOG_INPUT_PINCONFIG(14),
  ANALOG_INPUT_PINCONFIG(15),
  ANALOG_INPUT_PINCONFIG(16),
  ANALOG_INPUT_PINCONFIG(17),
};

struct AnalogPinConfig analogOutputPins[] = {
  ANALOG_OUTPUT_PINCONFIG(3),
  ANALOG_OUTPUT_PINCONFIG(5),
  ANALOG_OUTPUT_PINCONFIG(6),
  ANALOG_OUTPUT_PINCONFIG(9),
};

/* Serial port to use for printing informational messages to the user */
#define LOG_SERIAL Serial

// our own defined UUID for file data transfer characteristic
#define CHAR_UUID_FILE_DATA (0xFDAD)
BleCharacteristic fileDataChar(CHAR_UUID_FILE_DATA, 20, BLE_CLIENT_ACCESS_WRITE_ONLY, BLE_CLIENT_NOTIFY_WITH_ACK);
void fileDataCharEventCb(BleCharacteristic &characteristic, BleCharacteristicEvent event, void *arg)
{
    LOG_SERIAL.println("received file data event");
  if (BLE_CHAR_EVENT_WRITE == event) {
    char data[20];
    characteristic.getValue(data);
  }
}
/* BLE Peripheral Device (this Intel Curie device) */
BlePeripheral bleDevice;

/* BLE Automation I/O Service */
BleService ioService(SERVICE_UUID_AUTOMATIONIO);

/* The standard allows for multiple digital pins to be included per characteristic,
 * where the pin states are encapsulated as an array of 2-bit values
 *
 * This simplified implementation assumes just one pin per characteristic for now.
 */
#define DIGITAL_PIN_STATE_TO_VAL(pin, state) (uint8_t)((state) ? 0x1 << 6 : 0)
#define VAL_TO_DIGITAL_PIN_STATE(pin, val)   (uint8_t)((val) ? HIGH : LOW)

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x)[0])


/* For convenience, this macro will invoke a specified function call and will
 * check the status value returned to ensure it is successful.  If not, it will
 * print an error message to the serial port and will return from the current function
 */
#define CHECK_STATUS(op)                               \
  do {                                                 \
    BleStatus status = op;                             \
    if (BLE_STATUS_SUCCESS != status) {                \
      LOG_SERIAL.print(#op" returned error status: "); \
      LOG_SERIAL.println(status);                      \
      return;                                          \
    }                                                  \
  } while(0)

#define PLAY_AUDIO_PIN  11
#define FILE_UPLOAD_CONTROL_PIN 12
#define FILE_DATA_PIN   13

uint8_t fileUploading = false;
uint8_t fileIndex = 0;
File uploading; // the file that's uploading to
/* This function will be called when a BLE GAP event is detected by the
 * Intel Curie BLE device */
void blePeripheralEventCb(BlePeripheral &peripheral, BlePeripheralEvent event, void *arg)
{
  if (BLE_PERIPH_EVENT_CONNECTED == event) {
    LOG_SERIAL.println("Got CONNECTED event");
  } else if (BLE_PERIPH_EVENT_DISCONNECTED == event) {
    LOG_SERIAL.println("Got DISCONNECTED event");
  } else {
    LOG_SERIAL.println("Got UNKNOWN peripheral event");
  }
}

/* This function will be called when a connected remote peer sets a new value for a digital output characteristic */
void digitalOutputCharEventCb(BleCharacteristic &characteristic, BleCharacteristicEvent event, void *arg)
{
  unsigned pin = (unsigned)arg;
  uint32_t val;

  LOG_SERIAL.print("Got digital output characteristic event from pin:");
  LOG_SERIAL.println(pin);
  if (BLE_CHAR_EVENT_WRITE == event) {
    uint16_t size;
    uint8_t data[20];
    memset(data, 0, 20);
    /* The remote client has updated the value for this pin, get the current value */
    characteristic.getValue(data, size);
    if(size >0)
      val = data[0];
    /* Update the state of the pin to reflect the new value */
    digitalWrite(pin, VAL_TO_DIGITAL_PIN_STATE(pin, val));
    LOG_SERIAL.print("digital write value:");
    LOG_SERIAL.println(val);
    switch(pin){
      case FILE_UPLOAD_CONTROL_PIN:
        if(!fileUploading && val == 0x40) {
          fileUploading = true;
          fileIndex = 0;
          LOG_SERIAL.println("Start uploading! "); 
          clockwise(ring.Color(255,0,0), 100); // Red
          clockwise(ring.Color(0,0,0), 10); // Black          
        }
        else if(fileUploading && val == 0x0)
          stopUpload();  
        else
          LOG_SERIAL.println("Invalid state!!! Wrong uploading state or wrong input for upload control! ");  
        break;
      case FILE_DATA_PIN:
        if(fileUploading)
          writeToFile(data, size);
        else
          LOG_SERIAL.println("Invalid state!!!! Uploading not started!");
        break;  
      case PLAY_AUDIO_PIN:
        playAudio(val);
        break;
      default:
        LOG_SERIAL.println("event from unrelated pin");
        break;
    }
  } else LOG_SERIAL.println("Got UNKNOWN characteristic event");
  
}

// method to write to a file trunk by trunk
void writeToFile(uint8_t* val, uint16_t size) {
  char filename[15];
  strcpy(filename, "track001.mp3");
  if(fileIndex == 0) {
    // this is the index received from the sender, open file for write
    filename[5] = '0' + val[0]/100;
    filename[6] = '0' + val[0]/10;
    filename[7] = '0' + val[0]%10;
    if(USE_MUSIC_SHIELD) {
      uploading = SD.open(filename, FILE_WRITE);
      if (! uploading) {
         Serial.println("Couldn't open file to upload!");
         while (1);
      } else {
        LOG_SERIAL.print("open file "); LOG_SERIAL.println(filename);
        fileIndex = val[0];
      }
    } else {
      fileIndex = val[0];
      LOG_SERIAL.print("open file "); LOG_SERIAL.println(filename);
    }
  } else {
    if(USE_MUSIC_SHIELD) {
      // this is the file data, write to file
      if(!uploading.write(val, size)) {
        Serial.println("Couldn't write to file ");
        while(1);
      }
    } else {
      LOG_SERIAL.print("write #bytes to file: "); LOG_SERIAL.println(size);
    }
  }
}
void stopUpload(){
  if(USE_MUSIC_SHIELD)
    uploading.flush();
  fileUploading = false;
  Serial.println("Stop uploading");
  clockwise(ring.Color(0,255,0), 100); // Green
  clockwise(ring.Color(0,0,0), 10); // Black 
}

/* This function will be called when a connected remote peer sets a new value for an analog output characteristic */
void analogOutputCharEventCb(BleCharacteristic &characteristic, BleCharacteristicEvent event, void *arg)
{
  unsigned pin = (unsigned)arg;
  uint16_t val;

  if (BLE_CHAR_EVENT_WRITE == event) {
    /* The remote client has updated the value for this pin, get the current value */
    characteristic.getValue(val);
    /* Update the state of the pin to reflect the new value */
    analogWrite(pin, val);
  }
  else
    LOG_SERIAL.println("Got UNKNOWN characteristic event");
}

void setup() {
  LOG_SERIAL.begin(115200);
  
    LOG_SERIAL.println("serial started");

  /* Set a name for the BLE device */
  CHECK_STATUS(bleDevice.setName(DEVICE_NAME));

  /* First, initialise the BLE device */
  CHECK_STATUS(bleDevice.init());

  /* Set a function to be called whenever a BLE GAP event occurs */
  bleDevice.setEventCallback(blePeripheralEventCb);

  /* Add the Automation I/O Service, and include the UUID in BLE advertising data */
  CHECK_STATUS(bleDevice.addPrimaryService(ioService, true));

  /* Add characteristics for the Digital Inputs */
  for (unsigned i = 0; i < ARRAY_SIZE(digitalInputPins); i++) {
    DigitalPinConfig *pin = &digitalInputPins[i];

    /* Configure this pin as an input */
    pinMode(pin->pin, INPUT);

    /* Add the User-Description for this pin */
    CHECK_STATUS(pin->characteristic.addUserDescription(pin->name));
    /* Add the Presentation-Format for this pin
     *   format:      0x2    (unsigned 2-bit integer)
     *   exponent:    0      (Not Applicable)
     *   unit:        0x2700 (BLE GATT Unit UUID: unitless)
     *   nameSpace:   0x1    (Bluetooth standard namespace)
     *   description: pin+1  (Instance number of this characteristic)
     */
    CHECK_STATUS(pin->characteristic.addPresentationFormat(0x2, 0, 0x2700, 0x1, pin->pin + 1));
    /* Add the characteristic for this pin */
    CHECK_STATUS(ioService.addCharacteristic(pin->characteristic));
    /* Set an initial value for this characteristic; refreshed later in the loop() function */
    pin->val = digitalRead(pin->pin);
    CHECK_STATUS(pin->characteristic.setValue(DIGITAL_PIN_STATE_TO_VAL(pin->pin, pin->val)));
    /* Add a number_of_digitals descriptor for this characteristic */
    CHECK_STATUS(pin->characteristic.addDescriptor(pin->numDigitalsDesc));
    CHECK_STATUS(pin->numDigitalsDesc.setValue((uint8_t) 1));
  }

  /* Add characteristics for the Digital Outputs */
  for (unsigned i = 0; i < ARRAY_SIZE(digitalOutputPins); i++) {
    DigitalPinConfig *pin = &digitalOutputPins[i];

    /* Configure this pin as an output */
    pinMode(pin->pin, OUTPUT);

    /* Add the User-Description for this pin */
    CHECK_STATUS(pin->characteristic.addUserDescription(pin->name));
    /* Add the Presentation-Format for this pin
     *   format:      0x2    (unsigned 2-bit integer)
     *   exponent:    0      (Not Applicable)
     *   unit:        0x2700 (BLE GATT Unit UUID: unitless)
     *   nameSpace:   0x1    (Bluetooth standard namespace)
     *   description: pin+1  (Instance number of this characteristic)
     */
    CHECK_STATUS(pin->characteristic.addPresentationFormat(0x2, 0, 0x2700, 0x1, pin->pin + 1));
    /* Add the characteristic for this pin */
    CHECK_STATUS(ioService.addCharacteristic(pin->characteristic));
    /* Add a callback to be triggered if the remote device updates the value for this pin */
    pin->characteristic.setEventCallback(digitalOutputCharEventCb, (void*)pin->pin);
    /* Add a number_of_digitals descriptor for this characteristic */
    CHECK_STATUS(pin->characteristic.addDescriptor(pin->numDigitalsDesc));
    CHECK_STATUS(pin->numDigitalsDesc.setValue((uint8_t) 1));
  }

//  /* Add characteristics for the Analog Inputs */
//  for (unsigned i = 0; i < ARRAY_SIZE(analogInputPins); i++) {
//    AnalogPinConfig *pin = &analogInputPins[i];
//
//    /* Add the User-Description for this pin */
//    CHECK_STATUS(pin->characteristic.addUserDescription(pin->name));
//    /* Add the Presentation-Format for this pin
//     *   format:      0x6    (unsigned 16-bit integer)
//     *   exponent:    0      (Not Applicable)
//     *   unit:        0x2700 (BLE GATT Unit UUID: unitless)
//     *   nameSpace:   0x1    (Bluetooth standard namespace)
//     *   description: pin+1  (Instance number of this characteristic)
//     */
//    CHECK_STATUS(pin->characteristic.addPresentationFormat(0x6, 0, 0x2700, 0x1, pin->pin + 1));
//    /* Add the characteristic for this pin */
//    CHECK_STATUS(ioService.addCharacteristic(pin->characteristic));
//    /* Set an initial value for this characteristic; refreshed later in the loop() function */
//    pin->val = analogRead(pin->pin);
//    CHECK_STATUS(pin->characteristic.setValue(pin->val));
//  }
//
//  /* Add characteristics for the Analog Outputs */
//  for (unsigned i = 0; i < ARRAY_SIZE(analogOutputPins); i++) {
//    AnalogPinConfig *pin = &analogOutputPins[i];
//
//    /* Add the User-Description for this pin */
//    CHECK_STATUS(pin->characteristic.addUserDescription(pin->name));
//    /* Add the Presentation-Format for this pin
//     *   format:      0x6    (unsigned 16-bit integer)
//     *   exponent:    0      (Not Applicable)
//     *   unit:        0x2700 (BLE GATT Unit UUID: unitless)
//     *   nameSpace:   0x1    (Bluetooth standard namespace)
//     *   description: pin+1  (Instance number of this characteristic)
//     */
//    CHECK_STATUS(pin->characteristic.addPresentationFormat(0x6, 0, 0x2700, 0x1, pin->pin + 1));
//    /* Add the characteristic for this pin */
//    CHECK_STATUS(ioService.addCharacteristic(pin->characteristic));
//    /* Add a callback to be triggered if the remote device updates the value for this pin */
//    pin->characteristic.setEventCallback(analogOutputCharEventCb, (void*)pin->pin);
//  }


  fileDataChar.setEventCallback(fileDataCharEventCb);
  // add the file data characteristic
  ioService.addCharacteristic(fileDataChar);
  /* Now activate the BLE device.  It will start continuously transmitting BLE
   * advertising packets and thus become visible to remote BLE central devices
   * (e.g smartphones) until it receives a new connection */
  CHECK_STATUS(bleDevice.start());
  LOG_SERIAL.println("Bluetooth device active, waiting for connections...");

  ring.begin();
  ring.setBrightness(5);  // Lower brightness and save eyeballs!
  ring.show(); // Initialize all pixels to 'off'
  clockwise(ring.Color(0,0,0), 10); // Black

  if(USE_MUSIC_SHIELD){
    if (! musicPlayer.begin()) { // initialize the music player
      while (1);
    }
    SD.begin(CARDCS);
    musicPlayer.setVolume(00, 00); //(left,right), lower numbers means louder, currently at loudest
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  }
}

void loop() {
  //LOG_SERIAL.println("in loop");
  /* Update the digital input characteristics based on current pin reading */
  for (unsigned i = 0; i < ARRAY_SIZE(digitalInputPins); i++) {
    DigitalPinConfig *pin = &digitalInputPins[i];
    uint8_t newVal = digitalRead(pin->pin);
    if (newVal != pin->val) {
      //LOG_SERIAL.println("found new digital value");
      CHECK_STATUS(pin->characteristic.setValue(DIGITAL_PIN_STATE_TO_VAL(pin->pin, newVal)));
      pin->val = newVal;
    }
  }
  /* Update the analog input characteristics based on current pin reading*/
//  for (unsigned i = 0; i < ARRAY_SIZE(analogInputPins); i++) {
//    AnalogPinConfig *pin = &analogInputPins[i];
//    uint16_t newVal = analogRead(pin->pin);
//    if (newVal != pin->val) {      
//      //LOG_SERIAL.println("found new analog value");
//      CHECK_STATUS(pin->characteristic.setValue(newVal));
//      pin->val = newVal;
//    }
//  }

  /* Repeat the loop every 500ms - can be changed if desired */
//  delay(1500);
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
  clockwise(ring.Color(0,0,0), 10); // Black
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel(((i * 256 / ring.numPixels()) + j) & 255));
    }
    ring.show();
    delay(wait);
  }
  clockwise(ring.Color(0,0,0), 10); // Black
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
  clockwise(ring.Color(0,0,0), 10); // Black
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

/**
 * based on the value of the input play the audio file accordingly on the SD card
 */
void playAudio(uint8_t val){
    char filename[15];
    strcpy(filename, "track001.mp3");
    filename[5] = '0' + val/100;
      filename[6] = '0' + val/10;
      filename[7] = '0' + val%10;

      rainbowCycle(10);
      Serial.print("Playing file: ");
      Serial.println(filename);
      if(USE_MUSIC_SHIELD)
        musicPlayer.playFullFile(filename);
  }
