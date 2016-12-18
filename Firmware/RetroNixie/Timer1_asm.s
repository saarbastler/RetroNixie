
/*
 * Timer1.s
 *
 * Created: 01.12.2013 18:15:56
 *  Author: Jörg
 */ 

#include <avr/interrupt.h>

.extern Timer1_tickerL
.extern Timer1_tickerH

.global TIMER1_OVF_vect

.func TIMER1_OVF_vect
TIMER1_OVF_vect:
				push 	r16
				in 		r16, _SFR_IO_ADDR(SREG)
				push	r16

				lds		r16, Timer1_ticker
				inc		r16
				sts		Timer1_ticker, r16
        brne  noTick
      
				lds		r16, Timer1_ticker+1
				inc		r16
				sts		Timer1_ticker+1, r16

noTick:
				pop		r16
				out 	_SFR_IO_ADDR(SREG), r16
				pop 	r16
				reti
.endfunc
.end
