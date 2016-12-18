
/*
 * I2CMaster_asm.s
 *
 * Created: 03.09.2013 20:04:34
 *  Author: Jörg
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "Device.h"

// I2C Modes
#define I2C_READ			      0
#define I2C_ERROR			      1
#define I2C_WRITE_AND_READ  2
// first write an addition byte before writing more (register)
#define I2C_PRE_WRITE       3
#define I2C_FINISHED        4
#define I2C_TIMEOUT		      6
#define I2C_BUSY			      7

#ifdef I2C_INTERRUPT

.extern I2CMaster_mode
.extern I2CMaster_address
.extern I2CMaster_writeByte
.extern I2CMaster_buffer
.extern I2CMaster_size
.extern I2CMaster_error

#ifdef I2C_DEBUG_REGISTER
.extern I2CMaster_intCount
.extern memptr
#endif

#ifdef DEBUG_TWI_INT
.extern debugTWSR
#endif

;.global test
;test:

.global TWI_vect
.func TWI_vect
TWI_vect:
	        push	r16
	        in		r16, _SFR_IO_ADDR(SREG)
	        push	r16
          push  r17
	        push	r18
          push  r30
          push  r31

#ifdef I2C_DEBUG_REGISTER
          lds r16, I2CMaster_intCount
          inc r16
          sts I2CMaster_intCount, r16
#endif
          ;ldi   r16, 0
          ;out   _SFR_IO_ADDR(TCNT0), r16

	        lds		r18, I2CMaster_mode

#ifdef DEBUG_TWI_INT
          lds   r17, debugTWSR
#else
#if _SFR_IO_REG_P(TWSR)
	        in		r17, _SFR_IO_ADDR(TWSR)
#else
          lds   r17,_SFR_MEM_ADDR(TWSR)
#endif
#endif
          andi	r17, 0xf8
#ifdef I2C_DEBUG_REGISTER
          lds		r30, memptr
          lds		r31, memptr + 1
          st		Z+, r17
          sts		memptr, r30
#endif
          cpi   r17, TW_MR_DATA_NACK+8
          brsh  twError
          mov   r16, r17
          lsr   r16
          lsr   r16

          ldi   r30, lo8( twiJumpTable )
          ldi   r31, hi8( twiJumpTable )
          add   r30, r16
          clr   r16
          adc   r31, r16
          lpm   r16, Z+
          lpm   r31, Z
          mov   r30, r16
          ijmp

twiJumpTable:
          .word  pm( twError )
          .word  pm( twStart )      ; TW_START
          .word  pm( twStart )      ; TW_REP_START
          .word  pm( twMtSlaAck )   ; TW_MT_SLA_ACK
          .word  pm( twError )      ; TW_MT_SLA_NACK
          .word  pm( twMtDataAck )  ; TW_MT_DATA_ACK
          .word  pm( twError )      ; TW_MT_DATA_NACK
          .word  pm( twError )      ; TW_MT_ARB_LOST
          .word  pm( twMrSlaAck )   ; TW_MR_SLA_ACK
          .word  pm( twError )      ; TW_MR_SLA_NACK
          .word  pm( twMrDataAck )  ; TW_MR_DATA_ACK
          .word  pm( twMrDataNack ) ; TW_MR_DATA_NACK

twError: 
	        sts		I2CMaster_error, r17	; Store Error Condition
	        sbr		r18, _BV(I2C_ERROR)
	        cbr		r18, _BV(I2C_BUSY)

	        ldi		r16, _BV(TWINT)
	        rjmp	i2cWriteTWCR

twMtSlaAck:
          sbrc  r18, I2C_WRITE_AND_READ
          rjmp  writeByte
          sbrs  r18, I2C_PRE_WRITE
          rjmp  writeData
writeByte:
          cbr   r18, I2C_PRE_WRITE
          lds   r16, I2CMaster_writeByte
          rjmp  i2cWriteTWDR
writeData:
          lds   r16, I2CMaster_size
          and   r16, r16
          breq  i2cResetBusy
moreData:
          dec   r16
          sts   I2CMaster_size, r16
          lds   r30, I2CMaster_buffer
          lds   r31, I2CMaster_buffer+1
          ld    r16, Z+
          sts   I2CMaster_buffer, r30
          sts   I2CMaster_buffer+1, r31
          rjmp  i2cWriteTWDR

twMtDataAck:
          sbrs  r18, I2C_WRITE_AND_READ
          rjmp  writeData

          cbr   r18, _BV(I2C_WRITE_AND_READ)
          sbr   r18, _BV(I2C_READ)

          lds   r16, I2CMaster_address
          sbr   r16, _BV(0)
          sts   I2CMaster_address, r16
          ldi   r16, _BV(TWINT) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE)
          rjmp  i2cWriteTWCR

twMrSlaAck:
          lds   r16, I2CMaster_size
checkSize:
          cpi   r16, 1
          brlo  i2cStartCond
          ldi   r16,  _BV(TWINT) | _BV(TWEA)  | _BV(TWEN) | _BV(TWIE)
          rjmp  i2cWriteTWCR

twMrDataNack:
twMrDataAck:
#if _SFR_IO_REG_P(TWDR)
	        in		r16, _SFR_IO_ADDR(TWDR)	; Read data
#else
          lds   r16, _SFR_MEM_ADDR(TWDR)
#endif
				  lds		r30, I2CMaster_buffer
				  lds		r31, I2CMaster_buffer + 1
				  st		Z+, r16
				  sts		I2CMaster_buffer, r30
				  sts		I2CMaster_buffer + 1, r31

          lds   r16, I2CMaster_size
          dec   r16
          sts   I2CMaster_size, r16

          cpi   r17, TW_MR_DATA_NACK
          brne  checkSize

i2cResetBusy:
	        cbr		r18, _BV(I2C_BUSY)
          sbr   r18, _BV(I2C_FINISHED)

	        ldi		r16, _BV(TWINT) | _BV(TWSTO) | _BV(TWIE) | _BV(TWEN)	; Stop Condition
          rjmp  i2cWriteTWCR

twStart:
	        lds		r16, I2CMaster_address
	        ;bst		r18, I2C_READ
	        ;bld		r16, 0

i2cWriteTWDR:
#if _SFR_IO_REG_P(TWDR)
	        out		_SFR_IO_ADDR(TWDR), r16
#else
          sts   _SFR_MEM_ADDR(TWDR), r16
#endif

#ifdef I2C_DEBUG_REGISTER
          lds		r30, memptr
          lds		r31, memptr + 1
          st		Z+, r16
          sts		memptr, r30
#endif

i2cStartCond:
	        ldi		r16, _BV(TWINT) | _BV(TWEN) | _BV(TWIE)
i2cWriteTWCR:
#if _SFR_IO_REG_P(TWCR)
	        out		_SFR_IO_ADDR(TWCR), r16
#else
          sts   _SFR_MEM_ADDR(TWCR), r16
#endif

#ifdef I2C_DEBUG_REGISTER
          lds		r30, memptr
          lds		r31, memptr + 1
          st		Z+, r16
          sts		memptr, r30
#endif

i2cdone:
	        sts		I2CMaster_mode, r18

          pop   r31
          pop   r30
	        pop		r18
          pop   r17
	        pop		r16
	        out		_SFR_IO_ADDR(SREG), r16
	        pop		r16
	        reti
.endfunc

#endif

/*#ifndef DEBUG_TWI_INT

.global TIMER0_OVF_vect
.func TIMER0_OVF_vect
TIMER0_OVF_vect:

	        push	r16
	        in		r16, _SFR_IO_ADDR(SREG)
	        push	r16

	        ldi		r16, 0
#if _SFR_IO_REG_P(TWCR)
	        out		_SFR_IO_ADDR(TWCR), r16
#else
          sts   _SFR_MEM_ADDR(TWCR), r16
#endif
          out   _SFR_IO_ADDR(TCCR0B), r16

          ldi   r16, _BV(I2C_ERROR) | _BV(I2C_TIMEOUT)
	        sts		I2CMaster_mode, r16
          
	        pop		r16
	        out		_SFR_IO_ADDR(SREG), r16
	        pop		r16
	        reti
.endfunc

#endif*/
