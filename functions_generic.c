/*
 * Generic Functions 
 */

#include "functions_generic.h"



/***************************
 * FETCH OPCODE            *
 * *************************/

/* get_op_IMM : fetches operand based on IMM address modes
 */
size_t get_op_IMM(uint8_t *ptr_code)
{
	/* Immediate - XXX #Operand */
	//operand = (uint8_t) *(ptr_code+1); /* Keeps operand on Zero Page */
	operand = (uint8_t) read_addr(NES, NES->PC+1);
	NES->PC += 2; /* Update PC */
	return operand;
}


/* get_op_ZP_offest : fetches operand based on ZPX/ZPY address modes
 */
size_t get_op_ZP_offset(uint8_t *ptr_code, uint8_t offset)
{
	/* Zero Page X or Y - XXX operand, X/Y */
	operand = (uint8_t) (*(ptr_code+1) + offset); /* Keeps operand on Zero Page */
	sprintf(append_int, "%.2lX", operand - offset);
	strcpy(end, "$");
	strcat(end, append_int);
	//operand = (uint8_t) read_addr(NES, (NES->PC + 1 + offset)); //cpu overhaul pt2.
	NES->PC += 2; /* Update PC */
	return operand;
	//operand = read_addr(NES, (uint8_t) ((NES->PC + 1) + offset)); cpu overhaul pt2.
}


/* get_op_ABS_offest : fetches operand based on ABS/ABSX/ABSY address modes
 */
size_t get_op_ABS_offset(uint8_t *ptr_code, uint8_t offset)
{
	/* Absolute (modes) - XXX operand  or XXX operand, X/Y */
	operand = ((uint16_t) (*(ptr_code+2) << 8) | *(ptr_code+1));
	operand = (uint16_t) (operand + offset);
	NES->PC += 3; /* Update PC */
	tmp = operand - offset; // Page boundry calc
	/* Debugger */
	sprintf(append_int, "%.4zX", operand - offset);
	strcpy(end, "$");
	strcat(end, append_int);
	return operand;
	//operand = fetch_16(NES, (uint16_t) ((NES->PC + 1) + offset)));
	// causes strange behaviour
	// fetches right operand but using as is causes pc to point to illegal opcodes
	//printf("OPERAND = %d  %d (NEW)\t", operand, fetch_16(NES, (uint16_t) ((NES->PC + 1) + offset)));
}


/* get_op_IND : fetches operand based on IND address mode
 */
size_t get_op_IND(uint8_t *ptr_code, CPU_6502 *NESCPU)
{
	/* Indirect - JMP (operand) - 2 Byte address */
	operand = ((uint16_t) (*(ptr_code+2) << 8) | *(ptr_code+1));
	tmp = read_addr(NES, operand); /* PC low bute */
	if ((operand & 0x00FF) == 0x00FF) {
		/* JUMP BUG */
		operand = read_addr(NES, operand & 0xFF00); /* PC high byte */
	} else {
		operand = read_addr(NES, operand + 1); /* PC high byte */
	}
	operand = (uint16_t) (operand << 8) | tmp; /* get target address (little endian) */
	/* Debugger */
	sprintf(append_int, "%.zX", operand);
	strcpy(end, "($");
	strcat(end, append_int);
	strcat(end, ")");
	NES->PC += 3; /* Update PC */
	return operand;
}


/* get_op_INDX : fetches operand based on INDX address mode
 */
size_t get_op_INDX(uint8_t *ptr_code, CPU_6502 *NESCPU)
{
	/* Indirect X - XXX (operand, X ) - 2 Byte address (Zero-Page) */
	tmp = read_addr(NES, (uint8_t) (*(ptr_code+1) + NESCPU->X)); /* sum address (LSB) */
	operand = read_addr(NES, (uint8_t) (*(ptr_code+1) + NESCPU->X + 1)); /* Sum address + 1 (MSB) */
	operand = (uint16_t) (operand << 8) | tmp; /* get target address (little endian) */
	/* Debugger */
	sprintf(append_int, "%.2X", *(ptr_code+1));
	strcpy(end, "($");
	strcat(end, append_int);
	strcat(end, ",X)");
	NES->PC += 2; /* Update PC */
	return operand;
}


/* get_op_INDY : fetches operand based on INDY address mode
 */
size_t get_op_INDY(uint8_t *ptr_code, CPU_6502 *NESCPU)
{
	/* Indirect Y - XXX (operand), Y - 2 Byte address (Zero-Page) */
	tmp = read_addr(NES, (uint8_t) *(ptr_code+1)); /* sum address (LSB) */
	operand = read_addr(NES, (uint8_t) (*(ptr_code+1) + 1)); /* sum address + 1 (MSB) */
	operand = (uint16_t) (operand << 8) | tmp; /* get little endian */
	operand = (uint16_t) (NESCPU->Y + operand); /* get target address */
	/* Debugger */
	sprintf(append_int, "%.2X", *(ptr_code+1));
	strcpy(end, "($");
	strcat(end, append_int);
	strcat(end, "),Y");
	NES->PC += 2; /* Update PC */

	tmp = operand - NESCPU->Y; // Page boundy calc
	return operand;
}

/* using getch_16() for IND instructions and ABS causes some errors
 * most likely due to the code layout
 * will have to look into some more
 */

// Determines if a page cross has occured for a certain instruction
unsigned PAGE_CROSS(unsigned val1, unsigned val2)
{
	return ((val1 & 0xFF00) == (val2 & 0xFF00)) ? 1 : 0;
}

/***************************
 * STATUS                  *
 * *************************/

/* Return Status */
void RET_NES_CPU(void)
{
	printf("%-20s ", instruction);
	printf("A:%.2X ", NES->old_A);
	printf("X:%.2X ", NES->old_X);
	printf("Y:%.2X ", NES->old_Y);
	printf("P:%.2X ", NES->old_P);
	printf("SP:%.2X ", NES->old_Stack);
	printf("PC:%.4X ", NES->old_PC);
	printf("CPU:%.4d\n", NES->old_Cycle);
}

void transfer_cpu(void)
{
	NES->old_A = NES->A;
	NES->old_X = NES->X;
	NES->old_Y = NES->Y;
	NES->old_P = NES->P;
	NES->old_Stack = NES->Stack;
	NES->old_PC = NES->PC;
	NES->old_Cycle = NES->Cycle;
}

/***************************
 * ADD & SUB RELATED FUNCS *
 * *************************/

/* Base10toBase2 : converts an 8 bit number into it's binary equivalent
 */
void Base10toBase2(uint8_t quotient, int *bin_array)
{
	memset(bin_array, 0, 8*sizeof(int)); /* reset array to 0 */
	while ((quotient != 0) && (bin_array != NULL)) {
		*bin_array = quotient % 2;
		quotient /= 2;
		++bin_array;
	}
}

/* Base2toBase10 : converts an 8 bit array into it's decimal equivalent
 */
unsigned int Base2toBase10(int *bin_array, unsigned int dec_out)
{
	counter = 0;
	power2 = 1;
	while (counter == 0) {
		dec_out = *bin_array;
		++counter;
		++bin_array;
	}	
	for (counter = 1; counter < 8; counter++) {
		power2 *= 2;
		dec_out = dec_out + (power2 * (*bin_array));
		++bin_array;
	}
	return dec_out;
}

/* full_adder : full adder w/ carry in and out functionality
 *              returns cOUT --> mask into NES->P
 */
void full_adder(int *bin_sum1, int *bin_sum2, int cIN, uint8_t *cOUT, int *result)
{
	*cOUT = 0; /* Reset cOUT (tmp in this case) */
	counter = 0; /* for loop is cleaner - but breaks cOUT */
	while (counter < 8) {
		/* Fetch operand */
		result[counter] = (*bin_sum1 ^ *bin_sum2) ^ cIN;
		*cOUT = (cIN & (*bin_sum1 ^ *bin_sum2)) | (*bin_sum1 & *bin_sum2);
		++counter;
		++bin_sum1;
		++bin_sum2;
		cIN = *cOUT; /* update carry in */
	}
}

/***************************
 * STACK                   *
 * *************************/

/* Genric Push function */
void PUSH(uint8_t value)
{
	/* SP_START - 1 - as Stack = Empty Descending */
	/* FIX LIMIT CHECK */
	if (NES->Stack == 0x00) {
		/* Overflow */
		printf("Full stack - can't PUSH\n");
	} else {
		NES->RAM[SP_START + NES->Stack] = value;
		--NES->Stack;
	}
}


/* Genric Pop (Pull) function */
uint8_t PULL(void)
{
	/* FIX LIMIT CHECK */
	if (NES->Stack == SP_START) {
		/* Underflow */
		printf("Empty stack - can't PULL\n");
	} else {
		++NES->Stack;
		return (NES->RAM[SP_START + NES->Stack]);
	}
	return 0; /* fail-safe - if-else branch should return a value */
}

/***************************
 * FLAGS                   *
 * *************************/

/* Bits : 7 ----------> 0 */
/* Flags: N V - - D I Z C */

void update_FLAG_Z(uint8_t result)
{
	/* Zero Flag Test */
	if (result == 0) {
		NES->P |= FLAG_Z; /* Set Z */
	} else {
		NES->P &= ~(FLAG_Z); /* Clear Z */
	}
}


void update_FLAG_N(uint8_t result)
{
	/* Negative Flag Test */
	/* if (int8_t) result >= 0) - both execute in ~ same time */
	if (result >= 0x00 && result <= 0x7F) {
		NES->P &= ~(FLAG_N); /* Clear N */
	} else {
		NES->P |= FLAG_N; /* Set N */
	}
}


/* Parameters = 2 binary operands and then the result */
void update_FLAG_V(int *bin_array1, int *bin_array2, int *result)
{
	/* Overflow Flag Test */
	if (bin_array1[7] != bin_array2[7]) {
		/* Overflow is impossible if MSB (signs) are different */
		NES->P &= ~(FLAG_V); /* Clear V */
	} else if (bin_array1[7] != result[7]) {
		NES->P |= FLAG_V; /* Set V */
	} else {
		NES->P &= ~(FLAG_V); /* Clear V */
	}
}


void update_FLAG_C(uint8_t cOUT)
{
	/* Carry Flag Update */
	if (cOUT == 1) {
		NES->P |= cOUT; /* carry flag = 0th bit hence no shift */
	}

}
/* value is either 0x00 or 0x01 - catch all statement doesn't work */
void set_or_clear_CARRY(uint8_t value)
{
	/* Sets a FLAG if = 1, else if 0 then flag is cleared */ 
	if (value == 0) {
		NES->P &= (value | 0xFE); /* Need 0xFE as after AND need to preserve NES-> P values */ 
	} else if (value == 1) {
		NES->P |= value;
	}
}
