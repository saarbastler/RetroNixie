/*
 * CPPIO.h
 *
 * Created: 24.11.2016 17:20:05
 *  Author: Joerg
 */ 


#ifndef CPPIO_H_
#define CPPIO_H_

#include <stdint.h>

typedef volatile uint16_t CPP_SFR;

#define CPP_SFREGISTER(x)               (*(volatile uint8_t *)x)
#define CPP_SFREGISTER_ARRAY(x,index)   (((volatile uint8_t *)x)[index])



#endif /* CPPIO_H_ */