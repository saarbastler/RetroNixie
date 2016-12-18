/*
 * Timer1.cpp
 *
 * Created: 01.12.2013 18:16:50
 *  Author: Jörg
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Timer1.h"

volatile WORD Timer1::ticker asm("Timer1_ticker");
//volatile BYTE Timer1::tickerL asm("Timer1_tickerL");
//volatile BYTE Timer1::tickerH asm("Timer1_tickerH");

void Timer1::init( BYTE prescaler, BYTE wgm )
{
  TCCR1A= wgm & 3;
  TCCR1B= ((wgm << 1) & (_BV(WGM13)|_BV(WGM12))) | (7 & prescaler);
  TCCR1C= 0;
  TIMSK1= _BV(TOIE1);
  
  ticker= 0;
}

BYTE *Timer1::getTickerLAddr()
{
  return (BYTE *)&ticker;
}

WORD *Timer1::getTickerAddr()
{
  return (WORD*)&ticker;
}

WORD Timer1::getTickerWord()
{
  return ticker;
  //asm( "lds r24, Timer1_tickerL" );
  //asm( "lds r25, Timer1_tickerH" );
}
