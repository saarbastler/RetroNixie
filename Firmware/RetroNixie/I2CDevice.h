/*
 * I2CDevice.h
 *
 * Created: 15.09.2013 11:30:27
 *  Author: Jörg
 */ 


#ifndef I2CDEVICE_H_
#define I2CDEVICE_H_

#include "integer.h"
#include "device.h"

class I2CDevice
{
public:

  I2CDevice( BYTE _i2cAddress );
  
  // read a single BYTE into buffer
  BYTE read( );
  
  // write a single BYTE
  BYTE write( BYTE data );
  
  // read BYTEs from a register
  BYTE readRegister( BYTE index, BYTE size, BYTE *buffer );
  
  // write a single BYTE to a register
  BYTE writeRegister( BYTE index, BYTE data );
  
  // write multiple BYTEs to a register
  BYTE writeRegister( BYTE index, BYTE size, BYTE *buffer );
  
  // read the register, modify (oldValue & mask) | value and write again
  BYTE writeBits( BYTE index, BYTE mask, BYTE value );
  
protected:

  BYTE i2cAddress;
  BYTE buffer[1];

#ifndef I2C_SYNCHONOUS
  friend void i2cDeviceFinished( void *data );
  virtual void i2cFinished() = 0;
#endif

};



#endif /* I2CDEVICE_H_ */