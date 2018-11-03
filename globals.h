/* GLOBAL VARIABLES GO HERE */

#ifndef __GLOBALS__
#define __GLOBALS__


extern uint8_t tmp; /* tmp variable - stores result of functions - use to make code cleaner */

/* ADC & SBC array operands */
extern int bin_operand1[8]; /* NES->A converted to binary */
extern int bin_operand2[8]; /* other operand in ADC or SBC also in binary */
extern int bin_result[8]; /* result of ADC or SBC is stored here */
extern unsigned int power2; /* used in Base2 --> Base10 conversion */

#endif /* __GLOBALS__ */
