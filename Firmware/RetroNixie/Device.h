/*
 * Device.h
 *
 * Created: 30.11.2013 21:22:14
 *  Author: J?rg
 */ 


#ifndef DEVICE_H_
#define DEVICE_H_

#include "integer.h"

#define MINIMAL_BRIGHTNESS      0x10

#define STRING_SIZE             84

#define LED_STRING_PORT         PORTC
#define LED_STRING_DDR          DDRC
#define LED_STRING_BIT          0

#define TURN_ENCODER_PIN        PINC
#define TURN_ENCODER_PHASEA     3
#define TURN_ENCODER_PHASEB     2
#define TURN_ENCODER_BUTTON     1

#define F_CPU 16000000

#define I2C_SYNCHONOUS
#define I2C_INTERRUPT

#endif /* DEVICE_H_ */