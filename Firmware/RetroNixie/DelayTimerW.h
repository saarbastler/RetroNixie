/*
 * DelayTimer.h
 *
 * Created: 01.12.2013 18:22:37
 *  Author: Jörg
 */ 


#ifndef DELAYTIMERW_H_
#define DELAYTIMERW_H_

#include "integer.h"

class DelayTimerWord
{
public:

  DelayTimerWord( WORD *_ticker, WORD _wait );
  
  void setWait( WORD wait );
  WORD getWait()              { return timerWait; }

  void start();
  void stop();
  BYTE isDone();
  BYTE isStopped();
  
private:

  WORD *tickerL;
  WORD timerStart;
  WORD timerWait;
  BYTE timerDone;
  
};



#endif /* DELAYTIMERW_H_ */