/* 
* RetroNixieClock.cpp
*
* Created: 20.11.2016 08:15:42
* Author: Joerg
*/

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <string.h>

#include "Device.h"
#include "Timer1.h"

#include "RetroNixie.h"
#include "RetroNixieClock.h"

#include "I2CDS1307.h"
#include "I2CMaster.h"

#ifdef ENABLE_SERIAL_MONITOR
#include "PrintStream.h"
extern PrintStream out;
#endif

#define MODE_SHOW_TIME    0
#define MODE_SET_HOURS    1
#define MODE_SET_MINUTES  2
#define MODE_STORE_WAIT   3

// two bits red, green, blue. 00= off, 01= half bright, 10= full bright
const PROGMEM uint8_t RetroNixieClock::COLOR_BITS[] = { 
  /*0b000000, */0b000001, 0b000010, 0b000100, 0b000101, 0b000110, 0b001000, 0b001001, 0b001010,
  0b010000, 0b010001, 0b010010, 0b010100, 0b010101, 0b010110, 0b011000, 0b011001, 0b011010,
  0b100000, 0b100001, 0b100010, 0b100100, 0b100101, 0b100110, 0b101000, 0b101001, 0b101010 };
#define COLOR_BITS_SIZE     (sizeof(COLOR_BITS)/sizeof(COLOR_BITS[0]))

RetroNixieClock::RetroNixieClock() 
  : encoder(Timer1::getTickerLAddr(), this), 
  secondTimer( Timer1::getTickerAddr(), 100 ),
  milliseconds(0), seconds(0), time(0), timeInitialized(false)

{
  colorIndex= 0;
  brightness= 0x7f;
  setColor(brightness, colorIndex, color);

  off.r=0;
  off.g=0;
  off.b=0;

  mode= MODE_SHOW_TIME;
  clearDigitColor();

  ADMUX= _BV(REFS0) | /*_BV(ADLAR) |*/ 7;
  ADCSRA= _BV(ADEN) | _BV(ADATE) | _BV(ADIF) | 7;
  ADCSRB= 0;
  //DIDR0 = 1;

  ADCSRA |= _BV(ADSC);
  analogCount= 0;
  lastAnalogInput= 0;

  I2CMaster::init(80);
}

void RetroNixieClock::init()
{
  Timer1::init(  2, 14 );
  ICR1= 2000;

  LedString::init();
    
  secondTimer.start();    

  displayTime( time );
}

void RetroNixieClock::idle()
{
  encoder.cyclic();

  if( ADCSRA & _BV(ADIF) )
  {
    lastAnalogInput= ADC;
    analogIn += lastAnalogInput;
    ADCSRA= _BV(ADEN)  | _BV(ADATE) | _BV(ADIF) | 7;

    if( ++analogCount & 0xc0 )
    {
      analogCount= 0;
      setBrightness( 255 - (analogIn >> 8));
      analogIn= 0;
    }
  }
}

void RetroNixieClock::loop()
{
  if( secondTimer.isDone() )
  {
    secondTimer.start();
    milliseconds++;

    loop100ms();
  }
}

void RetroNixieClock::loop100ms()
{
  readRTC();

  if( ds1307.isDataAvailable() )
  {
    uint8_t dsSeconds= ds1307.getSeconds();
    if( seconds != dsSeconds )
    {
      milliseconds= 0;
      seconds= dsSeconds;
      time= ds1307.getHoursMinutes();

#ifdef ENABLE_SERIAL_MONITOR
      //out.printf_P( PSTR("TI: %wd %bd\r\n"), time, seconds);
#endif

      if( mode == MODE_SHOW_TIME)
      {
        displayTime( time );
      }
      else
      {
        uint8_t diff = *Timer1::getTickerLAddr() - modeTimeout;
        if( diff > 300 )
          setMode( MODE_SHOW_TIME );
      }
    }

    ds1307.resetDataAvailable();
  }

  timeSetLoop();
}

void RetroNixieClock::timeSetLoop()
{
  switch( mode )
  {
    case MODE_SET_HOURS:
    setColor( 15 + (setCounter<<4), setColorIndex, digitColor[0]);
    digitColor[1]= digitColor[0];

    if( setCounter >= 15 )
    {
      setCounterDir= -1;
    }
    else if( setCounter == 0 )
    {
      setCounterDir= 1;
      setColorIndex= (setColorIndex+1) % COLOR_BITS_SIZE;
    }

    setCounter += setCounterDir;
    
    RetroNixie::setNumber( timeSet, digitColor );
    LedString::transmit();
    break;
    case MODE_SET_MINUTES:
    setColor( 15 + (setCounter<<4), setColorIndex, digitColor[2]);
    digitColor[3]= digitColor[2];

    if( setCounter >= 15 )
    {
      setCounterDir= -1;
    }
    else if( setCounter == 0 )
    {
      setCounterDir= 1;
      setColorIndex= (setColorIndex+1) % COLOR_BITS_SIZE;
    }

    setCounter += setCounterDir;
    
    RetroNixie::setNumber( timeSet, digitColor );
    LedString::transmit();
    break;

    case MODE_STORE_WAIT:
    setColor( 15 + (setCounter<<4), setColorIndex, digitColor[0]);
    digitColor[1]= digitColor[0];
    digitColor[2]= digitColor[0];
    digitColor[3]= digitColor[0];

    if( setCounter != 0 )
    {
      setCounter--;
    }
    RetroNixie::setNumber( timeSet, digitColor );
    LedString::transmit();
    break;
  }
}

void RetroNixieClock::readRTC()
{
  if(!I2CMaster::isBusy())
  {
    if(ds1307.isModified())
    {
#ifdef ENABLE_SERIAL_MONITOR
      out.print_P(PSTR("write Device\r\n"));
#endif
      ds1307.writeDevice();

      if( I2CMaster::hasError())
      {
#ifdef ENABLE_SERIAL_MONITOR
        out.printf_P(PSTR("DS1307 Error: %bx\r\n"), I2CMaster::getError());
#endif
      }
    }
    else if( !ds1307.isDataAvailable() )
    {
#ifdef ENABLE_SERIAL_MONITOR
      if(!timeInitialized)
        out.print_P(PSTR("read Device\r\n"));
#endif
      ds1307.readDevice();
      
      if( I2CMaster::hasError())
      {
#ifdef ENABLE_SERIAL_MONITOR
        out.printf_P(PSTR("DS1307 Error: %bx\r\n"), I2CMaster::getError());
#endif
      }
      else
      {
        if( !ds1307.isClockEnabled() )
        {
          ds1307.enableClock(true);
        }
        else if(!timeInitialized)
        {
          seconds= ds1307.getSeconds();
          time= ds1307.getHoursMinutes();
          colorIndex= ds1307.getExtra(0);
          if( colorIndex >= COLOR_BITS_SIZE )
          colorIndex= 0;
        }

#ifdef ENABLE_SERIAL_MONITOR
        if(!timeInitialized)
        {
          for( BYTE b=0;b < 8;b++)
          {
            out.printHex( ds1307.buffer[b] );
            out.putch(' ');
          }
          out.crlf();
        }
#endif

        timeInitialized= true;
      }
    }
  }
}

void RetroNixieClock::setTime( uint16_t value )
{
  if( value < 2359 && (value%100) < 60)
  {
    seconds= 0;
    time= value;
    displayTime( time );

#ifdef ENABLE_SERIAL_MONITOR
    out.printf_P( PSTR("set time: %wd\r\n"), time);
#endif

    ds1307.setSeconds(seconds);
    ds1307.setHoursMinutes(value);

#ifdef ENABLE_SERIAL_MONITOR
    for( BYTE b=0;b < 8;b++)
    {
      out.printHex( ds1307.buffer[b] );
      out.putch(' ');
    }
    out.crlf();
#endif
  }
}

void RetroNixieClock::setBrightness( uint8_t b )
{
  if( b < MINIMAL_BRIGHTNESS )
    brightness= MINIMAL_BRIGHTNESS;
  else
    brightness= b;

  setColor(brightness, colorIndex, color);

  if( mode == MODE_SHOW_TIME )
    displayTime( time );
}

void RetroNixieClock::displayTime( WORD t )
{
  LedString::allOff();
  
  uint16_t b= brightness * ((seconds & 1) ? milliseconds : 10-milliseconds);
  uint8_t dotBrightness= b / 10;
  if( dotBrightness < MINIMAL_BRIGHTNESS )
    dotBrightness= MINIMAL_BRIGHTNESS;

  RGB_t dotColor;
  setColor( dotBrightness, colorIndex, dotColor);
  RetroNixie::setDot(seconds / 15, dotColor);

  RetroNixie::setNumber(t, color);

  LedString::transmit();
}


void RetroNixieClock::turn( int8_t counter, int8_t increment )
{
#ifdef ENABLE_SERIAL_MONITOR
  //out.printf_P( PSTR("%bx: %ch1\r\n"), counter, (increment < 0) ? '-' : '+');
#endif
  modeTimeout= *Timer1::getTickerLAddr();
  switch(mode)
  {
    case MODE_SHOW_TIME:
      colorIndex += increment;
      if( colorIndex >= COLOR_BITS_SIZE )
      {
        if( increment > 0 )
          colorIndex= 0;
        else
          colorIndex= COLOR_BITS_SIZE-1;
      }
      setColor(brightness, colorIndex,color);
      displayTime(time);      
      ds1307.setExtra(0, colorIndex);
      break;

    case MODE_SET_HOURS:
      if( (timeSet/100) == 0 && increment < 0 )
        timeSet += 2300;
      else
      {
        timeSet += 100 * increment;
        if( (timeSet/100) >= 24 )
          timeSet -= 2400;
      }
      break;
    case MODE_SET_MINUTES:
      if( (timeSet%100) == 0 && increment < 0 )
      timeSet += 59;
      else
      {
        timeSet += increment;
        if( (timeSet%100) >= 60 )
          timeSet -= 60;
      }
      break;
  }
}

void RetroNixieClock::click( bool down )
{
#ifdef ENABLE_SERIAL_MONITOR
  //out.printf_P( PSTR("Click: %bx\r\n"), down ? 1 : 0);
#endif
  modeTimeout= *Timer1::getTickerLAddr();
  switch( mode )
  {
    case MODE_SHOW_TIME:
      if(!down)
        setMode( MODE_SET_HOURS );
      break;
    case MODE_SET_HOURS:
      if(!down)
        setMode( MODE_SET_MINUTES );
      break;
    case MODE_SET_MINUTES:
      if(down)
      {
        setMode( MODE_STORE_WAIT );
      }
      break;
    case MODE_STORE_WAIT:
      if(!down)
      {
        if(setCounter == 0)
        {
          setTime( timeSet );
        }
        setMode(MODE_SHOW_TIME);
      }
      break;
  }
}

void RetroNixieClock::setMode( uint8_t newMode )
{
  if( newMode != mode )
  {
#ifdef ENABLE_SERIAL_MONITOR
    out.printf_P( PSTR("M%bx->%bx\r\n"), mode , newMode );
#endif

    switch(newMode)
    {
      case MODE_SHOW_TIME:
        displayTime(time);
        break;
      case MODE_SET_HOURS:
        timeSet=time;
        setCounter= 0;
        setCounterDir= 1;
        setColorIndex= 0;
        clearDigitColor();
        digitColor[2]= color;
        digitColor[3]= color;
        break;
      case MODE_SET_MINUTES:
        setCounter= 0;
        setCounterDir= 1;
        setColorIndex= 0;
        clearDigitColor();
        digitColor[0]= color;
        digitColor[1]= color;
        break;
      case MODE_STORE_WAIT:
        setCounter= 15;
        digitColor[2]= color;
        digitColor[3]= color;
        break;
    }

    mode= newMode;
  }

}

void RetroNixieClock::clearDigitColor()
{
  memset(digitColor, 0, sizeof(digitColor));
}

void RetroNixieClock::setColor( uint8_t value, uint8_t index, RGB_t& rgb)
{
  uint8_t bits=  pgm_read_byte(&COLOR_BITS[index]);
  uint8_t *color= (uint8_t*)&rgb;
  for( uint8_t i=0;i < 3;i++,color++,bits>>=2)
  {
    uint8_t c= bits & 3;
    if( c == 1 )
      *color= value >> 1;
    else if( c == 2 )
      *color= value;
    else
      *color= 0;
  }
}