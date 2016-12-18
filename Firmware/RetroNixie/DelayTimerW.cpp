/*
 * DelayTimer.cpp
 *
 * Created: 01.12.2013 18:23:19
 *  Author: Jörg
 */ 

#include "DelayTimerW.h"

DelayTimerWord::DelayTimerWord( WORD *_tickerL, WORD _wait )
{
  tickerL= _tickerL;
  setWait( _wait );
}

void DelayTimerWord::setWait( WORD wait )
{
  stop();
  timerWait= wait;
}

void DelayTimerWord::start()
{
  timerStart= *tickerL;
  timerDone= 0;
}

void DelayTimerWord::stop()
{
  timerDone= 2;
}

BYTE DelayTimerWord::isDone()
{
  if( timerDone == 2 )
  {
    return 0;
  }
  
  if( !timerDone )
  {
    WORD diff= *tickerL - timerStart;
    
    timerDone= diff >= timerWait;
  }
  
  return timerDone;
}

BYTE DelayTimerWord::isStopped()
{
  return timerDone == 2;  
}