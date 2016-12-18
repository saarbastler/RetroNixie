/*
 * Timer1.h
 *
 * Created: 01.12.2013 18:14:44
 *  Author: Jörg
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

#include "integer.h"

class Timer1
{
public:
  static void init( BYTE prescaler, BYTE wgm );
  
  static WORD getTickerWord();
  
  static BYTE *getTickerLAddr();

  static WORD *getTickerAddr();
  
  static volatile WORD ticker;
  //static volatile BYTE tickerH;
};



#endif /* TIMER1_H_ */