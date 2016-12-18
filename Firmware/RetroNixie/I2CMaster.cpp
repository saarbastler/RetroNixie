/*
 * I2CMaster.cpp
 *
 * Created: 03.09.2013 19:25:02
 *  Author: Jörg
 */ 

#include "Device.h"

#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include <string.h>

#include "I2CMaster.h"

// I2C Modes
#define I2C_READ			      0
#define I2C_ERROR			      1
// first write a byte then read data
#define I2C_WRITE_AND_READ  2
// first write an addition byte before writing more (register)
#define I2C_PRE_WRITE       3
#define I2C_FINISHED        4
#define I2C_TIMEOUT		      6
#define I2C_BUSY			      7


volatile BYTE I2CMaster::mode asm("I2CMaster_mode");
BYTE I2CMaster::address asm("I2CMaster_address");
BYTE I2CMaster::writeByte asm("I2CMaster_writeByte");
BYTE *I2CMaster::buffer asm("I2CMaster_buffer");
BYTE I2CMaster::size asm("I2CMaster_size");
BYTE I2CMaster::error asm("I2CMaster_error");

#ifndef I2C_SYNCHONOUS
void *I2CMaster::pfData;
PFI2CFinished I2CMaster::pfI2CFinished;
#endif

BYTE I2CMaster::buf[20];

#ifdef I2C_DEBUG_REGISTER
BYTE I2CMaster::intCount asm("I2CMaster_intCount");

BYTE I2CMaster::getIntCount() { return intCount; }
BYTE I2CMaster::getSize()     { return size; }
  
BYTE *memptr;
#endif

#ifdef DEBUG_TWI_INT
extern "C" void TWI_vect();
BYTE debugTWSR;
#endif

void I2CMaster::init( BYTE baudrate )
{
#ifdef DEBUG_TWI_INT
  debugTWSR= 8;
  TWI_vect();
#endif  

#ifndef I2C_SYNCHONOUS
  pfI2CFinished= 0;
#endif

  TWBR= baudrate;
  TWSR= 0;
  TWCR= _BV(TWEN);
  
  //TCCR0A= 0;
  //TCCR0B= 0;
  //TCNT0= 0;
  //TIMSK0= _BV( TOIE0 );
  
  //DDRD &= ~(_BV(2)|_BV(3));
  //PORTD |= (_BV(2)|_BV(3));
  
#ifdef I2C_DEBUG_REGISTER  
  memptr= (BYTE *)I2C_DEBUG_REGISTER_MEM;
#endif

  mode= 0;
}

BYTE I2CMaster::getMode()
{ 
  return mode; 
}

BYTE I2CMaster::getError()
{
  return error;  
}

BYTE I2CMaster::getIndex()
{
  return writeByte;
}

void I2CMaster::waitWhileBusy()
{
  while(isBusy())
  {
#ifdef I2C_SYNCHONOUS
    idle();
#else
    cyclic();
#endif
  }
}

BYTE I2CMaster::isBusy()
{
  if( mode & _BV(I2C_BUSY) )
  {
    return 1;
  }
  
  return 0; //(TWCR & (_BV(TWINT) & _BV(TWSTO))) == _BV(TWSTO);
}

BYTE I2CMaster::hasError()
{
  return mode & _BV(I2C_ERROR);
}

BYTE I2CMaster::isFinshed()
{
  return mode & _BV(I2C_FINISHED);
}

void I2CMaster::resetFinished()
{
  mode &= ~_BV(I2C_FINISHED);
}

BYTE I2CMaster::getAddress()
{
  return address;  
}

BYTE I2CMaster::start( BYTE _address, BYTE* _buffer, BYTE _size, BYTE _mode
#ifndef I2C_SYNCHONOUS
                                                                            , PFI2CFinished _pfI2CFinished, void *_data )
#else
                                                                            )
#endif
{ 
#ifdef DEBUG_PRINT_I2C_STATES  
  uart.print_P( PSTR("I2C A:"));
  uart.printHex( _address );
  if( _mode & (_BV(I2C_WRITE_AND_READ) | _BV(I2C_PRE_WRITE) ))
  {
    uart.print_P( PSTR(" REG:"));
    uart.printHex( writeByte );
  }
  if( !(mode & _BV(I2C_READ)) &&  _size > 0 )
  {
    uart.dumpMemory( PSTR(" W:"), _buffer, _size );
  }
  else
  {
    uart.crlf();    
  }
#endif
  
  waitWhileBusy();

  error= 0;
#ifndef I2C_SYNCHONOUS
  pfI2CFinished= _pfI2CFinished;
  pfData= _data;
#endif

  mode= _mode;
  if( mode & _BV(I2C_READ) )
  {
    address= _address | _BV(0);
  }
  else
  {
    address= _address & ~_BV(0);
  }	
	buffer= _buffer;
	size= _size;	

#ifdef I2C_DEBUG_REGISTER
	intCount= 0;
  memptr= (BYTE *)I2C_DEBUG_REGISTER_MEM;
  memset( (void *)I2C_DEBUG_REGISTER_MEM, 0, 0x100 );
#endif

#ifdef I2C_INTERRUPT
	TWCR= _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE);
#else                                
	TWCR= _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
#endif

#ifdef I2C_SYNCHONOUS
  do 
  {
    idle();
  } 
  while (mode & _BV(I2C_BUSY));
  
#ifdef DEBUG_PRINT_I2C_STATES
  if( mode & _BV(I2C_ERROR) )
  {
    uart.printf_P( PSTR("Error: "));
    uart.printHex( error );
  }
  if( mode & _BV(I2C_READ))
  {
    if( _size > 0 )
    {
      uart.dumpMemory( PSTR(" R:"), _buffer, _size );
    }
  }
  uart.printf_P( PSTR(" ...Done"));
  uart.crlf();
#endif

  return !(mode & _BV(I2C_ERROR));
#else
  return 0;
#endif

}

BYTE I2CMaster::startRead( BYTE _address, BYTE* _buffer, BYTE _size
#ifndef I2C_SYNCHONOUS
                                                                    , PFI2CFinished _pfI2CFinished, void *_data )
#else
                                                                    )
#endif
{
  if( _size < 1 )
  {
    _size= 1;
  }
  return start( _address, _buffer, _size, _BV(I2C_BUSY) | _BV(I2C_READ)
#ifndef I2C_SYNCHONOUS
                                                                , _pfI2CFinished, _data );
#else
                                                                );
#endif
}

BYTE I2CMaster::startWrite( BYTE _address, BYTE _byte )
{
  waitWhileBusy();
  
  writeByte= _byte;
  return startWrite( _address, &writeByte, 1
#ifndef I2C_SYNCHONOUS
                                      , 0, 0 );
#else
                                      );
#endif
}

BYTE I2CMaster::startWrite( BYTE _address, BYTE* _buffer, BYTE _size
#ifndef I2C_SYNCHONOUS
                                                                    , PFI2CFinished _pfI2CFinished, void *_data )
#else
                                                                    )
#endif
{
  return start( _address, _buffer, _size, _BV(I2C_BUSY)
#ifndef I2C_SYNCHONOUS
                                                , _pfI2CFinished, _data );
#else
                                                );
#endif
}

BYTE I2CMaster::startWriteBeforeRead( BYTE _address, BYTE _writeByte, BYTE* _buffer, BYTE _size
#ifndef I2C_SYNCHONOUS
                                                                                                , PFI2CFinished _pfI2CFinished, void *_data )
#else
                                                                                                )
#endif

{
  waitWhileBusy();  

  writeByte= _writeByte;
  return start( _address, _buffer, _size, _BV(I2C_BUSY) | _BV(I2C_WRITE_AND_READ)
#ifndef I2C_SYNCHONOUS
                                                                          , _pfI2CFinished, _data );
#else
                                                                          );
#endif
}

BYTE I2CMaster::startWrite( BYTE _address, BYTE _writeByte, BYTE* _buffer, BYTE _size
#ifndef I2C_SYNCHONOUS
                                                                                      , PFI2CFinished _pfI2CFinished, void *_data )
#else
                                                                                      )
#endif
{
  waitWhileBusy();
  	
	writeByte= _writeByte;
	return start( _address, _buffer, _size, _BV(I2C_BUSY) | _BV(I2C_PRE_WRITE)
#ifndef I2C_SYNCHONOUS
                                                                      , _pfI2CFinished, _data );
#else
                                                                      );
#endif
}

#ifdef I2C_DEBUG_REGISTER

#define WRITE_TWI( REG, VALUE )   \
  debugByte( VALUE );             \
  REG= VALUE                      \
  
void debugByte( BYTE b )
{
  if(memptr < (BYTE *)0x800 )
  {
    *memptr++= b;
  } 
}

#else

#define WRITE_TWI( REG, VALUE )   REG= VALUE

#endif

#ifndef I2C_SYNCHONOUS
void I2CMaster::cyclic()
{
#ifndef I2C_INTERRUPT
  BYTE twcr= TWCR;
  if( (twcr & (_BV(TWINT) |_BV(TWEN))) == (_BV(TWINT) |_BV(TWEN)))
  {
    //TCNT0 = 0;
    BYTE twsr= TWSR & 0xf8;
    
    /*uart.putch('T');
    uart.putch('W');
    uart.putch('S');
    uart.putch('R');
    uart.putch(':');
    uart.printHex( twsr );
    uart.crlf();*/
    
#ifdef I2C_DEBUG_REGISTER
    intCount++;
    debugByte( twsr );
#endif
    
#ifdef DEBUG_PRINT_I2C_STATES
  uart.print_P( PSTR("TWSR:"));
  uart.printHex( twsr );
  uart.crlf();
#endif

	  switch( twsr )
	  {
  	  case TW_START:
      case TW_REP_START:
        WRITE_TWI( TWDR, address );
        WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWEN) );
    	  break;

  	  case TW_MT_SLA_ACK:
      // überflüssig ?
        if( mode & _BV(I2C_WRITE_AND_READ) )
        {          
          //mode |= _BV(I2C_READ);
          //address |= _BV(0);
          //mode &= ~_BV(I2C_WRITE_AND_READ);
          
          WRITE_TWI( TWDR, writeByte );
          WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWEN) );         
        }
        else if( mode & _BV(I2C_PRE_WRITE))
        {
          mode &= ~_BV(I2C_PRE_WRITE);
          
          WRITE_TWI( TWDR, writeByte );
          WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWEN) );          
        }
        else
        {
          if( size > 0 )
          {
            WRITE_TWI( TWDR, *(BYTE *)buffer );
            buffer++;
            size--;
            WRITE_TWI( TWCR, _BV( TWINT) | _BV(TWEN) );
          }
          else
          {
            WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWSTO) | _BV(TWEN) );
            
            finished();
          }          
        }
        break;
        
      case TW_MT_DATA_ACK:
        if( mode & _BV(I2C_WRITE_AND_READ) )
        {
          mode |= _BV(I2C_READ);
          address |= _BV(0);
          mode &= ~_BV(I2C_WRITE_AND_READ);
          
          WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) );
        }
        else
        {
          if( size > 0 )
          {          
            WRITE_TWI( TWDR, *(BYTE *)buffer );
            buffer++;
            size--;
            WRITE_TWI( TWCR, _BV( TWINT) | _BV(TWEN) );
    	    }
          else
          {
    	      WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWSTO) | _BV(TWEN) );
            
            finished();
          }
        }
  	    break;

      case TW_MR_SLA_ACK:
        if( size > 1 )
        {
          WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWEN) | _BV(TWEA) );
        }
        else
        {
          WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWEN) );
        }
        break;
    
    case TW_MR_DATA_NACK:
      *(BYTE *)buffer= TWDR;
#ifdef I2C_DEBUG_REGISTER
      //debugByte( *(BYTE *)buffer );
#endif
      size--;
      WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWSTO) | _BV(TWEN) );
      mode |= _BV(I2C_FINISHED);
      error= 0;

      finished();      
      break;
          
    case TW_MR_DATA_ACK:
      *(BYTE *)buffer= TWDR;
#ifdef I2C_DEBUG_REGISTER
      //debugByte( *(BYTE *)buffer );
#endif
      
      buffer++;
      if( --size > 1 )
      {
        WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWEN) | _BV(TWEA) );
      }
      else
      {
        WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWEN) );
      }
      break;
      
  	default:
      error= twsr;
  	  WRITE_TWI( TWCR, _BV(TWINT) | _BV(TWSTO) | _BV(TWEN) );
      mode |= _BV( I2C_ERROR ) | _BV(I2C_FINISHED);
      
      finished();
    	break;
	  }
  }

#endif
}
#endif

#ifndef I2C_SYNCHONOUS
void I2CMaster::finished()
{
  mode &= ~_BV( I2C_BUSY );
  //TCCR0B= 0;
 
  /*uart.printf_P( PSTR("I2C Finished error: %bx pfI2CFinished: %wx data: %wx\r\n")
    , error, pfI2CFinished, pfData );*/
    
  if( pfI2CFinished != 0 )
  {
    PFI2CFinished tmp= pfI2CFinished;
    pfI2CFinished= 0;
    tmp( pfData );
  }  
}
#endif
