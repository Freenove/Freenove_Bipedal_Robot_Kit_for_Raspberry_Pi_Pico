#ifndef _FREENOVE_VK16K33_H
#define _FREENOVE_VK16K33_H

  // include appropriate version of Arduino code
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

  // include Wire for I2C comms
  #include <Wire.h>
  #include "Adafruit_GFX.h"
  
  // different commands
  #define VK16K33_CMD_RAM     0x00
  #define VK16K33_CMD_SETUP   0x80
  #define VK16K33_CMD_DIMMING 0xE0

  // other options
  #define VK16K33_DISPLAY_OFF 0x00
  #define VK16K33_DISPLAY_ON  0x01
  #define VK16K33_BLINK_OFF   0x00
  #define VK16K33_BLINK_1HZ   0x02
  #define VK16K33_BLINK_2HZ   0x04
  #define VK16K33_BLINK_0HZ5  0x06

  //#define uint8_t unsigned char
  //#define uint16_t unsigned int

  // actual class
  class Freenove_VK16K33
  {
    public:
	  void init(unsigned char addr = 0x71);

#if defined (ESP8266) || defined(ESP32)
      void init(unsigned char addr= 0x71, unsigned char sda, unsigned char scl);
#endif

      // brightness control
      void setBrightness(unsigned char brightness);
      // blink controls
      void setBlink(unsigned char blink);
      // orientation
      void resetDirection(void);
      void reverse(void);
      void flipVertical(void);
      void flipHorizontal(void);
      // buffer stuff
      void clear(void);
	  //Sets the value of a particular row in the LED matrix
      void setRow(unsigned char row, unsigned char value, bool rowDirection = 0);
      // trun on led matrix
      void show(void);
	  
	  // show led matrix
      void showStaticArray(byte *array1, byte *array2);
	  void showLedMatrix(byte array[8][8],int x_offset=4,int y_offset=4);
	  
    private:
      unsigned int *_buffer;
      unsigned char  _i2c_addr;
      bool     _reversed;
      bool     _vFlipped;
      bool     _hFlipped;
      int      _brightness;
	  
      void writeRow(unsigned char row);
      void setPixel(unsigned char row, unsigned char col, unsigned char val, bool rowDirection);
  };

#endif // #VK16K33
