/*
 * I2CMaster.h
 *
 * Created: 03.09.2013 19:24:39
 *  Author: Jörg
 */ 


#ifndef I2CMASTER_H_
#define I2CMASTER_H_

// define I2C_INTERRUPT to user interrupt handling of I2C
// if not defined, I2CMast.cyclic must be called as often as possible.
//#define I2C_INTERRUPT


// define I2C_DEBUG_REGISTER to write all access to the TWI to the memory pointer memptr
// #define I2C_DEBUG_REGISTER

// define I2C_SYNCHONOUS will simulate a sync call to Methods, i2cIdle has 
// to be declared and is is called cyclic
//#define I2C_SYNCHONOUS

#include "integer.h"
#include "Device.h"

#ifdef I2C_SYNCHONOUS
extern void idle();
#endif

#ifndef I2C_SYNCHONOUS
typedef void (*PFI2CFinished)( void *data );
#endif

class I2CMaster
{
public:

  static void init( BYTE baudrate );
  
#ifndef I2C_SYNCHONOUS  
  static void cyclic();
#endif

  static BYTE getMode();
  static BYTE getError();
  static BYTE getIndex();
  static BYTE getAddress();
  
  static BYTE isBusy();
  static void waitWhileBusy();

  static BYTE isFinshed();
  static void resetFinished();
  static BYTE hasError();
  
#ifdef I2C_SYNCHONOUS  
  // start a read cycle
  static BYTE startRead( BYTE _address, BYTE* _buffer, BYTE _size );
  
  // start a write cycle
  static BYTE startWrite( BYTE _address, BYTE* _buffer, BYTE _size );

  // start a write cycle
  static BYTE startWrite( BYTE _address, BYTE _byte );

  // first write a byte, then read bytes
  static BYTE startWriteBeforeRead( BYTE _address, BYTE _writeByte, BYTE* _buffer, BYTE _size );
  
  // first write a byte, then write buffer
  static BYTE startWrite( BYTE _address, BYTE _writeByte, BYTE* _buffer, BYTE _size );
#else

  // start a read cycle
  static BYTE startRead( BYTE _address, BYTE* _buffer, BYTE _size, PFI2CFinished pfI2CFinished, void *data  );
  
  // start a write cycle
  static BYTE startWrite( BYTE _address, BYTE* _buffer, BYTE _size, PFI2CFinished pfI2CFinished, void *data  );

  // start a write cycle
  static BYTE startWrite( BYTE _address, BYTE _byte );

  // first write a byte, then read bytes
  static BYTE startWriteBeforeRead( BYTE _address, BYTE _writeByte, BYTE* _buffer, BYTE _size, PFI2CFinished pfI2CFinished = 0, void *data = 0 );
  
  // first write a byte, then write buffer
  static BYTE startWrite( BYTE _address, BYTE _writeByte, BYTE* _buffer, BYTE _size, PFI2CFinished pfI2CFinished = 0, void *data = 0 );
#endif
  
private:

#ifdef I2C_SYNCHONOUS
  static BYTE start( BYTE _address, BYTE* _buffer, BYTE _size, BYTE _mode );
#else
  static BYTE start( BYTE _address, BYTE* _buffer, BYTE _size, BYTE _mode, PFI2CFinished pfI2CFinished, void *data );
  static void finished();
#endif

  static volatile BYTE mode;
  static BYTE address;
  static BYTE writeByte;
  static BYTE *buffer;
  static BYTE size;
  static BYTE error;

#ifndef I2C_SYNCHONOUS  
  static void *pfData;
  static PFI2CFinished pfI2CFinished;
#endif
  
  static BYTE intCount;
  
 public:
 
  static BYTE buf[];
  
  static BYTE getIntCount();
  static BYTE getSize();
  static void *getBuffer()  { return buf; }
};


#endif /* I2CMASTER_H_ */
