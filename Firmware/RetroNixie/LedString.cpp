/*
 * LedString.cpp
 *
 * Created: 24.08.2013 19:25:28
 *  Author: Jörg
 */ 

#include <avr/io.h>
#include <string.h>

#include "LedString.h"

extern "C" 
{
  #include "WS2811.h"
};

#define BIT(B)           (0x01 << (uint8_t)(B))
#define SET_BIT_HI(V, B) (V) |= (uint8_t)BIT(B)
#define SET_BIT_LO(V, B) (V) &= (uint8_t)~BIT(B)

// Define the output function, using pin 0 on port b.
DEFINE_WS2811_FN(WS2811RGB, LED_STRING_PORT, LED_STRING_BIT)


RGB_t LedString::mem[STRING_SIZE];

void LedString::init()
{
  LED_STRING_DDR |= _BV( LED_STRING_BIT );
  LED_STRING_PORT &= ~_BV( LED_STRING_BIT );
  
  allOff();
  transmit();
}  
  // all Leds off
void LedString::allOff()
{
  memset( mem, 0, sizeof( mem ));
}  
  
// set all Leds to r,g,b
void LedString::setAll( BYTE r, BYTE g, BYTE b )
{
  BYTE *ptr= (BYTE *)mem;
  for(BYTE i=0;i < STRING_SIZE;i++)
  {
    *ptr++= r;
    *ptr++= g;
    *ptr++= b;
  }
}  

void LedString::setLed( BYTE index, RGB_t rgb )
{
  setLed( index, rgb.r, rgb.g, rgb.b );
}

void LedString::setLed( BYTE index, BYTE r, BYTE g, BYTE b )
{
  if( index < STRING_SIZE)
  {
    BYTE *ptr= (BYTE *)&mem[ index ];
    *ptr++= r;
    *ptr++= g;
    *ptr= b;
  }
}

void LedString::shift( BYTE fromIndex, BYTE toIndex, BYTE size )
{
  if( fromIndex < toIndex )
  {
    if( toIndex + size > STRING_SIZE )
    {
      size= STRING_SIZE - toIndex;
    }
  }
  else if( fromIndex > toIndex )
  {
    if( fromIndex + size > STRING_SIZE )
    {
      size= STRING_SIZE - fromIndex;
    }
  }
  BYTE *src= (BYTE *)&mem[ fromIndex ];
  BYTE *dst= (BYTE *)&mem[ toIndex ];
  
  memmove( dst, src, size * 3 );
}

void LedString::clearRange( BYTE from, BYTE size )
{
  if( from + size <= STRING_SIZE )
    memset(&mem[from], 0, size*3);
}
void LedString::transmit()
{
  WS2811RGB( mem, STRING_SIZE );
}
