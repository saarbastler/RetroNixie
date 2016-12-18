/* 
* RetroNixie.h
*
* Created: 06.11.2016 09:54:44
* Author: Joerg
*/


#ifndef __RETRONIXIE_H__
#define __RETRONIXIE_H__

#include <stdint.h>

#include "rgb.h"

class RetroNixie
{
public:

  /*
   *	digit is the digit no, 0..4
   *  value is the value to set 0..9
   *  color is the RGB color
   */
  static void setDigit(uint8_t digit, uint8_t value, RGB_t color);

  /*
   *	Sets the 4 digit number in display in that color
   */
  static void setNumber(uint16_t number, RGB_t color);

  /*
   *	Sets the 4 digit number in display, color is an array of four colors
   */
  static void setNumber(uint16_t number, RGB_t *color);

  /*
   *	sets the dot 0..3 color
   */
  static void setDot(uint8_t dot, RGB_t color);

  /*
   *	sets the all 4 dot color
   */
  static void setAllDot(RGB_t color);

}; //RetroNixie

#endif //__RETRONIXIE_H__
