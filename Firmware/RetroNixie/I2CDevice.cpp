/*
 * I2CDevice.cpp
 *
 * Created: 15.09.2013 11:42:25
 *  Author: Jörg
 */ 

#include "I2CDevice.h"
#include "I2CMaster.h"

#ifndef I2C_SYNCHONOUS
void i2cDeviceFinished( void *data )
{
  ((I2CDevice *)data)->i2cFinished();
}
#endif

I2CDevice::I2CDevice( BYTE _i2cAddress )
{
  i2cAddress= _i2cAddress;
}

/*BYTE I2CDevice::getI2CAddress()
{ 
  return i2cAddress; 
}*/

BYTE I2CDevice::read( )
{
  return I2CMaster::startRead(i2cAddress, buffer, 1
#ifndef I2C_SYNCHONOUS
                                            , i2cDeviceFinished, this);
#else
                                            );
#endif
}  
  
BYTE I2CDevice::write( BYTE data )
{
  I2CMaster::waitWhileBusy();
  /*uart.putch('W');
  uart.printHex( data );
  uart.crlf();*/
  
  buffer[0]= data;
  return I2CMaster::startWrite(i2cAddress, buffer, 1
#ifndef I2C_SYNCHONOUS
                                              , i2cDeviceFinished, this);
#else
                                              );
#endif
}

BYTE I2CDevice::readRegister( BYTE index, BYTE size, BYTE *buffer )
{
  return I2CMaster::startWriteBeforeRead( i2cAddress, index, buffer, size
#ifndef I2C_SYNCHONOUS
                                                                  , i2cDeviceFinished, this);
#else
                                                                  );
#endif

}

BYTE I2CDevice::writeRegister( BYTE index, BYTE data )
{
  I2CMaster::waitWhileBusy();

  buffer[0]= data;
  return I2CMaster::startWrite( i2cAddress, index, buffer, 1
#ifndef I2C_SYNCHONOUS
                                                      , i2cDeviceFinished, this);
#else
                                                      );
#endif
  
}

BYTE I2CDevice::writeRegister( BYTE index, BYTE size, BYTE *buffer )
{
  return I2CMaster::startWrite(i2cAddress, index, buffer, size
#ifndef I2C_SYNCHONOUS
                                                        , i2cDeviceFinished, this);
#else
                                                        );
#endif
  
}

BYTE I2CDevice::writeBits( BYTE index, BYTE mask, BYTE value )
{
  if( readRegister(index, 1, buffer) )
  {
    buffer[0] &= mask;
    buffer[0] |= (value & ~mask);
    
    return writeRegister(index, 1, buffer);
  }
  
  return 0;
}