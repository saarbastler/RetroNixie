/* 
* TurnEncoder.h
*
* Created: 18.11.2016 19:37:39
* Author: Joerg
*/


#ifndef __TURNENCODER_H__
#define __TURNENCODER_H__

#include <stdint.h>

#include "CPPIO.h"

class TurnReceiver
{
public:
  virtual void turn( int8_t counter, int8_t increment ) = 0;

  virtual void click( bool down ) = 0;
};

template<CPP_SFR PIN, uint8_t PHASE1_BIT, uint8_t PHASE2_BIT, uint8_t BUTTON_BIT>
class TurnEncoder
{
public:
	TurnEncoder( volatile uint8_t *_ticker, TurnReceiver *_receiver )
  : ticker(_ticker), receiver(_receiver) 
  {
    uint8_t mask= PHASE1_BIT|PHASE2_BIT|BUTTON_BIT;

    CPP_SFREGISTER_ARRAY(PIN,1) &= ~mask; // DDR
    CPP_SFREGISTER_ARRAY(PIN,2) |= mask; // PORT

    waiting= false;
    counter= 0;

    oldValue= CPP_SFREGISTER(PIN) & mask;
  };

  void cyclic()
  {
    uint8_t mask= PHASE1_BIT|PHASE2_BIT|BUTTON_BIT;
    if( waiting )
    {
      uint8_t diff= *ticker - waitStart;
      if( diff > 2 )
      {
        waiting= false;
        uint8_t newValue= *(volatile uint8_t *)PIN & mask;
        if( newValue != oldValue )
        {
          if( (oldValue & BUTTON_BIT) != (newValue & BUTTON_BIT))
          {
            click( newValue & BUTTON_BIT );
          }
          if( (oldValue & PHASE1_BIT) == 0 && (newValue & PHASE1_BIT) )
          {
            if( newValue & PHASE2_BIT )
            turn(1);
            else
            turn(-1);
          }
          else if( (oldValue & PHASE2_BIT) && (newValue & PHASE2_BIT) == 0)
          {
            if( newValue & PHASE1_BIT )
            turn(1);
            else
            turn(-1);
          }
          oldValue= newValue;
        }
      }
    }
    else if( (*(volatile uint8_t *)PIN & mask) != oldValue )
    {
      waitStart= *ticker;
      waiting= true;
    }
  }
protected:

  void turn( int8_t increment)
  {
    counter += increment;

    if( receiver != nullptr )
      receiver->turn(counter, increment);
  }

  void click( uint8_t newValue )
  {
    if( receiver != nullptr )
      receiver->click( newValue == 0);
  }

  volatile uint8_t *ticker;

  TurnReceiver *receiver;

  uint8_t waitStart;
  bool waiting;
  uint8_t oldValue;

  int8_t counter;
}; //TurnEncoder

#endif //__TURNENCODER_H__
