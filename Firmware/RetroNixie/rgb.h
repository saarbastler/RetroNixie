/*
 * rgb.h
 *
 * Created: 24.08.2013 19:32:06
 *  Author: Jörg
 */ 


#ifndef RGB_H_
#define RGB_H_

// RGB value structure.
typedef struct __attribute__ ((__packed__)) {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RGB_t;

#endif /* RGB_H_ */