/* 
* I2CDS1307.h
*
* Created: 24.11.2016 19:22:40
* Author: Joerg
*/


#ifndef __I2CDS1307_H__
#define __I2CDS1307_H__

#include <stdint.h>

#include "integer.h"
#include "I2CDevice.h"
#include "I2CMaster.h"

template<uint8_t EXTRA_SIZE>
class I2CDS1307 : public I2CDevice
{
public:
	I2CDS1307() : I2CDevice(0xd0), modified(false), dataAvailable(false)
  {
  }

  void readDevice()
  {
    readRegister(0, 8 + EXTRA_SIZE, buffer);
    if(!I2CMaster::hasError()) 
      dataAvailable= true;
  }

  void writeDevice()
  {
    writeRegister(0,8 + EXTRA_SIZE,buffer);
    if(!I2CMaster::hasError()) 
      modified= false;
  }

  bool isModified()
  {
    return modified;
  }

  bool isDataAvailable()
  {
    return dataAvailable;
  }

  void resetDataAvailable()
  {
    dataAvailable= false;
  }

  bool isClockEnabled()
  {
    return (buffer[0] & 0x80) == 0;
  }

  void enableClock(bool enable)
  {
    if( enable )
      buffer[0] &= ~0x80;
    else
      buffer[0] |= 0x80;

    modified= true;
  }

  uint8_t getSeconds()
  {
    return 10 * ((buffer[0] & 0x70) >> 4) + (buffer[0] & 0xf);
  }

  void setSeconds( uint8_t v)
  {
    uint8_t b= (uint8_t)((v / 10) << 4) & 0x70;
    v %= 10;
    b |= v;

    buffer[0]= (buffer[0] & 0x80) | b;
    modified= true;
  }

  uint16_t getHoursMinutes()
  {
    return 1000 * ((buffer[2] & 0x30)>>4) + 100 *(buffer[2] & 0xf) + 10 * ((buffer[1] & 0x70)>>4) + (buffer[1] & 0x0f);
  }

  void setHoursMinutes( uint16_t v )
  {
    if( v < 2400 && (v%100) < 60)
    {
      uint8_t b = (uint8_t)((v / 1000) << 4) & 0x30;
      v %= 1000;
      b |= (uint8_t)(v / 100);
      v %= 100;
      buffer[2]= b;

      b= (uint8_t)((v / 10) << 4) & 0x70;
      v%=10;
      b |= (uint8_t)v;
      buffer[1]= b;
      modified= true;
    }
  }

  uint8_t getExtra( uint8_t index )
  {
    if( index < EXTRA_SIZE )
      return buffer[8 + index];
    else
      return 0;
  }

  void setExtra( uint8_t index, uint8_t value )
  {
    if( index < EXTRA_SIZE )
    {
      buffer[8 + index]= value;
      modified= true;
    }
  }

protected:

  bool modified;
  bool dataAvailable;
public:
  uint8_t buffer[9 + EXTRA_SIZE];
}; //I2CDS1307

#endif //__I2CDS1307_H__
