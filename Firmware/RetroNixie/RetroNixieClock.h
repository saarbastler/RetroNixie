/* 
* RetroNixieClock.h
*
* Created: 20.11.2016 08:15:42
* Author: Joerg
*/


#ifndef __RETRONIXIECLOCK_H__
#define __RETRONIXIECLOCK_H__

#include "Device.h"
#include "TurnEncoder.h"
#include "DelayTimerW.h"
#include "LedString.h"
#include "I2CDS1307.h"

class RetroNixieClock : public TurnReceiver
{
//functions
public:
	RetroNixieClock();

  void init();

  // called, if I2C is idle waiting
  void idle();
  
  void loop();

  virtual void turn( int8_t counter, int8_t increment );

  virtual void click( bool down );

  void setTime( uint16_t value );

  void setBrightness( uint8_t b );

  uint16_t getAnalogIn()
  {
    return lastAnalogInput;
  }

private:

  /*
   *	called every 100ms
   */
  void loop100ms();

  /*
   *	read the RTC time
   */
  void readRTC();

  void timeSetLoop();

  void setColor( uint8_t value, uint8_t index, RGB_t& rgb);

  void displayTime( WORD t );
  void clearDigitColor();
  void setMode( uint8_t newMode );

  TurnEncoder<(CPP_SFR)&TURN_ENCODER_PIN,_BV(TURN_ENCODER_PHASEA),_BV(TURN_ENCODER_PHASEB),_BV(TURN_ENCODER_BUTTON)> encoder;
  DelayTimerWord secondTimer;

  BYTE milliseconds;
  BYTE seconds;
  WORD time;

  uint8_t colorIndex;
  RGB_t color;
  RGB_t off;
  RGB_t digitColor[4];

  uint8_t modeTimeout;
  uint8_t mode;
  int8_t setCounter;
  int8_t setCounterDir;
  uint8_t setColorIndex;

  uint16_t timeSet;
  uint16_t analogIn;
  uint8_t analogCount;
  uint8_t brightness;

  uint16_t lastAnalogInput;
  bool timeInitialized;

  const static uint8_t COLOR_BITS[];

  I2CDS1307<1> ds1307;
}; //RetroNixieClock

#endif //__RETRONIXIECLOCK_H__
