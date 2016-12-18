/* 
* RetroNixie.cpp
*
* Created: 06.11.2016 09:54:44
* Author: Joerg
*/


#include "RetroNixie.h"
#include "LedString.h"

/*
 *	Digit LED
 *  0     0, 10
 *  1     9, 19
 *  2     1, 11
 *  3     8, 18
 *  4     2, 13
 *  5     7, 17
 *  6     3, 13
 *  7     6, 16
 *  8     4, 14
 *  9     5, 15
 */
void RetroNixie::setDigit(uint8_t digit, uint8_t value, RGB_t color)
{
  if( digit < 4 && value < 10)
  {
    BYTE offset= digit*20;
    if( digit > 1)
      offset += 4;
    
    if( value & 1 )
      value= 9 - (value >> 1);
    else
      value= (value >> 1);

    LedString::clearRange(offset, 20);
    LedString::setLed(offset+value,color);
    LedString::setLed(offset+value+10, color);
  }  
}

 void RetroNixie::setNumber(uint16_t number, RGB_t color)
 {
    if( number < 10000)
    {
      setDigit(0, number / 1000, color);
      number %= 1000;
      setDigit(1, number / 100, color);
      number %= 100;
      setDigit(2, number / 10, color);
      number %= 10;
      setDigit(3, number , color);
    }
 }

 void RetroNixie::setNumber(uint16_t number, RGB_t *color)
 {
    if( number < 10000)
    {
      setDigit(0, number / 1000, *color++);
      number %= 1000;
      setDigit(1, number / 100, *color++);
      number %= 100;
      setDigit(2, number / 10, *color++);
      number %= 10;
      setDigit(3, number , *color);
    }
 }

 void RetroNixie::setDot(uint8_t dot, RGB_t color)
 {
    if( dot < 4)
    {
      if( dot & 1)
        dot= 3 - (dot >> 1);
      else
        dot >>= 1;

      LedString::setLed(40+dot,color);
    }
 }

 void RetroNixie::setAllDot(RGB_t color)
 {
    setDot(0, color);
    setDot(1, color);
    setDot(2, color);
    setDot(3, color);
 }