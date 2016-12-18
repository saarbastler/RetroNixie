/*
 * MotorController.cpp
 *
 * Created: 04.10.2016 20:11:01
 * Author : Joerg
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "Device.h"
#include "Timer1.h"
#include "DelayTimerW.h"

#include "RetroNixieClock.h"
#include "I2CMaster.h"

/*extern "C"
{
  void __cxa_pure_virtual()
  {
    
  }
}*/

RetroNixieClock retroNixieClock;

int main(void)
{  
  sei();
  
  retroNixieClock.init();
  for(;;)
  {
    retroNixieClock.loop();
    idle();
  }
}

void idle()
{
  retroNixieClock.idle();
}

