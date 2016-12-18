/*
 * LedString.h
 *
 * Created: 24.08.2013 19:25:06
 *  Author: Jörg
 */ 


#ifndef LEDSTRING_H_
#define LEDSTRING_H_

#include "Device.h"
#include "integer.h"
#include "rgb.h"

//#ifndef STRING_SIZE
//#define STRING_SIZE 40
//#endif

class LedString
{
public:
  static void init();
  
  // all Leds off
  static void allOff();
  
  // set all Leds to r,g,b
  static void setAll( BYTE r, BYTE g, BYTE b );
  static void clearRange( BYTE from, BYTE size );

  static void setLed( BYTE index, BYTE r, BYTE g, BYTE b );
  static void setLed( BYTE index, RGB_t rgb );
  
  static void shift( BYTE fromIndex, BYTE toIndex, BYTE size );
  
  static void transmit();
  
private:

  static RGB_t mem[STRING_SIZE];
};



#endif /* LEDSTRING_H_ */