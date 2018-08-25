/*
 * Execute Functions (Opcode functions)
 *
 */

#include "functions.h" /* + cpu.h */
#include "functions_generic.h"
#include <stdio.h>

size_t operand;
uint8_t tmp;
int counter;


int bin_operand1[8];
int bin_operand2[8];
int bin_result[8];
unsigned int power2 = 1;
/***************************
 * STORAGE                 *
 * *************************/

/* execute_LDA: LDA command - Load A with memory
 */
void execute_LDA(enum MODES address_mode, size_t operand)
{
	if (address_mode == IMM) {
		//printf("LDA #%.4zX    ", operand);
		strcpy(instruction, "LDA #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		NES->A = operand;
	} else {
		strcpy(instruction, "LDA ");
		strcat(instruction, end);
		NES->A = read_addr(NES, operand);
	}
	update_FLAG_N(NES->A);
	update_FLAG_Z(NES->A);
}


/* execute_LDX: LDX command - Load X with memory
 */
void execute_LDX(enum MODES address_mode, size_t operand)
{
	if (address_mode == IMM) {
		NES->X = operand;
		strcpy(instruction, "LDX #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
	} else {
		//printf("LDX $%.4zX    ", operand);
		strcpy(instruction, "LDX ");
		strcat(instruction, end);
		NES->X = read_addr(NES, operand);
	}
	update_FLAG_N(NES->X);
	update_FLAG_Z(NES->X);
}


/* execute_LDY: LDY command - Load Y with memory
 */
void execute_LDY(enum MODES address_mode, size_t operand)
{
	if (address_mode == IMM) {
		strcpy(instruction, "LDY #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		NES->Y = operand;
	} else {
		strcpy(instruction, "LDY ");
		strcat(instruction, end);
		NES->Y = read_addr(NES, operand);
	}
	update_FLAG_N(NES->Y);
	update_FLAG_Z(NES->Y);
}


/* execute_STA: STA command - Store A in memory
 */
void execute_STA(size_t operand)
{
	strcpy(instruction, "STA ");
	strcat(instruction, end);
	//printf("STA $%.4zX    ", operand);
	write_addr(NES, operand, NES->A);
}


/* execute_STX: STX command - Store X in memory
 */
void execute_STX(size_t operand)
{
	strcpy(instruction, "STX ");
	strcat(instruction, end);
	write_addr(NES, operand, NES->X);
}


/* execute_STY: STY command - Store Y in memory
 */
void execute_STY(size_t operand)
{
	strcpy(instruction, "STY ");
	strcat(instruction, end);
	write_addr(NES, operand, NES->Y);
}


/* execute_TAX: TAX command - Transfer A to X
 */
void execute_TAX(void)
{
	strcpy(instruction, "TAX");
	NES->X = NES->A;
	update_FLAG_N(NES->X);
	update_FLAG_Z(NES->X);
}


/* execute_TAY: TAY command - Transfer A to Y
 */
void execute_TAY(void)
{
	strcpy(instruction, "TAY");
	NES->Y = NES->A;
	update_FLAG_N(NES->Y);
	update_FLAG_Z(NES->Y);
}


/* execute_TSX: TSX command - Transfer SP to X
 */
void execute_TSX(void)
{
	strcpy(instruction, "TSX");
	NES->X = NES->Stack;
	update_FLAG_N(NES->X);
	update_FLAG_Z(NES->X);
}


/* execute_TXA: TXA command - Transfer X to A
 */
void execute_TXA(void)
{
	strcpy(instruction, "TXA");
	NES->A = NES->X;
	update_FLAG_N(NES->A);
	update_FLAG_Z(NES->A);
}


/* execute_TXS: TXS command - Transfer X to SP
 */
void execute_TXS(void)
{
	strcpy(instruction, "TXS");
	NES->Stack = NES->X;
}


/* execute_TYA: TYA command - Transfer Y to A
 */
void execute_TYA(void)
{
	strcpy(instruction, "TYA");
	NES->A = NES->Y;
	update_FLAG_N(NES->A);
	update_FLAG_Z(NES->A);
}

/***************************
 * MATH                    *
 * *************************/

/* execute_ADC: ADC command - Add mem w/ A and C (A + M + C : then set flags)
 */
void execute_ADC(enum MODES address_mode, size_t operand)
{
	Base10toBase2(NES->A, bin_operand1);
	if (address_mode == IMM) {
		/* Immediate - ADC #Operand */
		//printf("ADC #%.4zX    ", operand);
		strcpy(instruction, "ADC #$");
		sprintf(append_int, "%zX", operand);
		strcat(instruction, append_int);
		Base10toBase2(operand, bin_operand2);
	} else {
		//printf("ADC $%.4zX    ", operand);
		strcpy(instruction, "ADC ");
		strcat(instruction, end);
		Base10toBase2(read_addr(NES, operand), bin_operand2);
	}
	full_adder(bin_operand1, bin_operand2, NES->P & FLAG_C, &tmp, bin_result);
	NES->A = Base2toBase10(bin_result, 0);
	update_FLAG_N(NES->A);
	update_FLAG_V(bin_operand1, bin_operand2, bin_result);
	update_FLAG_Z(NES->A);
	set_or_clear_CARRY(tmp);
}

/* execute_DEC: DEC command - Decrement Mem by one
 */
void execute_DEC(size_t operand)
{
	//printf("DEC $%.4zX    ", operand);
	strcpy(instruction, "DEC");
	strcat(instruction, end);
	write_addr(NES, operand, read_addr(NES, operand) - 1);
	update_FLAG_N(read_addr(NES, operand));
	update_FLAG_Z(read_addr(NES, operand));
}


/* execute_DEX: DEX command - Decrement X by one
 */
void execute_DEX(void)
{
	/* Implied Mode */
	strcpy(instruction, "DEX");
	--(NES->X);
	update_FLAG_N(NES->X);
	update_FLAG_Z(NES->X);
}


/* execute_DEY: DEY command - Decrement Y by one
 */
void execute_DEY(void)
{
	/* Implied Mode */
	strcpy(instruction, "DEY");
	--(NES->Y);
	update_FLAG_N(NES->Y);
	update_FLAG_Z(NES->Y);
}


/* execute_INC: INC command - Increment Mem by one
 */
void execute_INC(size_t operand)
{
	printf("INC $%.4zX    ", operand);
	write_addr(NES, operand, read_addr(NES, operand) + 1);
	update_FLAG_N(read_addr(NES, operand));
	update_FLAG_Z(read_addr(NES, operand));
}


/* execute_INX: INX command - Increment X by one
 */
void execute_INX(void)
{
	strcpy(instruction, "INX");
	/* Implied Mode */
	++(NES->X);
	update_FLAG_N(NES->X);
	update_FLAG_Z(NES->X);
}


/* execute_INY: DEY command - Increment Y by one
 */
void execute_INY(void)
{
	strcpy(instruction, "INY");
	/* Implied Mode */
	++(NES->Y);
	update_FLAG_N(NES->Y);
	update_FLAG_Z(NES->Y);
}


/* execute_SBC: SBC command - Subtract mem w/ A and C (A - M -C : then set flags)
 */
void execute_SBC(enum MODES address_mode, size_t operand)

{
	/* SBC = ADC w/ 2nd operand converted to 2's compliment */
	Base10toBase2(NES->A, bin_operand1);
	if (address_mode == IMM) {
		/* Immediate - SBC #Operand */
		strcpy(instruction, "SBC #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		Base10toBase2(operand ^ 0xFF, bin_operand2);
	} else {
		strcpy(instruction, "SBC ");
		strcat(instruction, end);
		Base10toBase2(read_addr(NES, operand) ^ 0xFF, bin_operand2);
	}
	full_adder(bin_operand1, bin_operand2, NES->P & FLAG_C, &tmp, bin_result);
	NES->A = Base2toBase10(bin_result, 0);
	update_FLAG_N(NES->A);
	update_FLAG_V(bin_operand1, bin_operand2, bin_result);
	update_FLAG_Z(NES->A);
	set_or_clear_CARRY(tmp);
}

/***************************
 * BITWISE                 *
 * *************************/

/* execute_AND: AND command - AND memory with Acc
 */
void execute_AND(enum MODES address_mode, size_t operand)
{
	if (address_mode == IMM) {
		strcpy(instruction, "AND #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		NES->A &= operand;
	} else {
		strcpy(instruction, "AND ");
		strcat(instruction, end);
		NES->A &= read_addr(NES, operand);
	}
	update_FLAG_N(NES->A);
	update_FLAG_Z(NES->A);
}


/* execute_ASL: ASL command - Arithmetic Shift Left one bit (Acc or mem)
 * ASL == LSL
 */
void execute_ASL(enum MODES address_mode, size_t operand)
{
	if (address_mode == ACC) {
		/* Accumulator - ASL #Operand */
		strcpy(instruction, "ASL A");
		tmp = NES->A & 0x80; /* Mask 7th bit */
		NES->A = NES->A << 1;
		update_FLAG_N(NES->A);
		update_FLAG_Z(NES->A);
	} else {
		/* Shift value @ address 1 bit to the left */
		strcpy(instruction, "ASL ");
		strcat(instruction, end);
		tmp = read_addr(NES, operand) & 0x80; /* Mask 7th bit */
		write_addr(NES, operand, read_addr(NES, operand) << 1);
		update_FLAG_N(read_addr(NES, operand));
		update_FLAG_Z(read_addr(NES, operand));
	}
	tmp = tmp >> 7; /* needed so that function below works */;
	/* Update Carry */
	set_or_clear_CARRY(tmp);
}


/* execute_BIT: BIT command - BIT test (AND) between mem and Acc
 */
void execute_BIT(size_t operand)
{
	//printf("BIT $%.4zX    ", operand);
	strcpy(instruction, "BIT ");
	strcat(instruction, end);
	tmp = NES->A & read_addr(NES, operand);
	/* Update Flags */
	/* N = Bit 7, V = Bit 6 (of fetched operand) & Z = 1 (if AND result = 0) */
	/* Setting 7th Bit */
	if ((read_addr(NES, operand) & FLAG_N) == FLAG_N) {
		NES->P |= FLAG_N; /* set */
	} else {
		NES->P &= ~(FLAG_N); /* clear flag */
	}
	/* Setting 6th Bit */
	if ((read_addr(NES, operand) & FLAG_V) == FLAG_V) {
		NES->P |= FLAG_V;
	} else {
		NES->P &= ~(FLAG_V);
	}
	/* Setting Zero FLAG */
	if (tmp == 0) {
		NES->P |= FLAG_Z; /* set */
	} else {
		NES->P &= ~(FLAG_Z); /* clear */
	}
	/* CLC/CLV could be called - but wanted to reduce function overhead */
}


/* execute_EOR: EOR command - Exclusive OR memory with Acc
 */
void execute_EOR(enum MODES address_mode, size_t operand)
{
	if (address_mode == IMM) {
		/* Immediate - AND #Operand */
		//printf("EOR #%.4zX    ", operand);
		strcpy(instruction, "EOR #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		NES->A ^= operand;
	} else {
		strcpy(instruction, "EOR ");
		strcat(instruction, end);
		NES->A ^= read_addr(NES, operand);
	}
	update_FLAG_N(NES->A);
	update_FLAG_Z(NES->A);
}


/* execute_LSR: LSR command - Logical Shift Right by one bit (Acc or mem)
 */
void execute_LSR(enum MODES address_mode, size_t operand)
{
	if (address_mode == ACC) {
		/* Accumulator - LSR #Operand */
		strcpy(instruction, "LSR A");
		tmp = NES->A & 0x01; /* Mask 0th bit */
		NES->A = NES->A >> 1;
		update_FLAG_N(NES->A); /* Should always clear N flag */
		update_FLAG_Z(NES->A);
	} else {
		strcpy(instruction, "LSR ");
		strcat(instruction, end);
		tmp = read_addr(NES, operand) & 0x01; /* Mask 0th bit */
		write_addr(NES, operand, read_addr(NES, operand) >> 1);
		update_FLAG_N(read_addr(NES, operand)); /* Should always clear N flag */
		update_FLAG_Z(read_addr(NES, operand));
	}
	/* Update Carry */
	set_or_clear_CARRY(tmp);
}
	

/* execute_ORA: ORA command - OR memory with Acc
 */
void execute_ORA(enum MODES address_mode, size_t operand)
{
	if (address_mode == IMM) {
		/* Immediate - AND #Operand */
		//printf("ORA #%.4zX    ", operand);
		strcpy(instruction, "ORA #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		NES->A |= operand;
	} else {
		strcpy(instruction, "ORA ");
		strcat(instruction, end);
		NES->A |= read_addr(NES, operand);
	}
	update_FLAG_N(NES->A);
	update_FLAG_Z(NES->A);
}


/* execute_ROL: ROL command - Rotate Shift Left one bit (Acc or mem)
 * ROL == LSL (execpt Carry Flag is copied into LSB & Carry = MSB after shift)
 */
void execute_ROL(enum MODES address_mode, size_t operand)
{
	if (address_mode == ACC) {
		/* Accumulator - ROL #Operand */
		strcpy(instruction, "ROL A");
		tmp = NES->A & 0x80; /* Mask 7th bit */
		NES->A = NES->A << 1;
		/* Testing if Status Reg has a 1 in Carry Flag */
		if ((NES->P & FLAG_C) == 0x01) {
			NES->A |= (NES->P & FLAG_C);
		} /* if carry = 0 then do nothing as that still leaves a zero in the 0th bit */
		update_FLAG_N(NES->A);
		update_FLAG_Z(NES->A);
	} else {
		strcpy(instruction, "ROL ");
		strcat(instruction, end);
		tmp = read_addr(NES, operand) & 0x80; /* Mask 7th bit */
		write_addr(NES, operand, read_addr(NES, operand) << 1);
		if ((NES->P & FLAG_C) == 0x01) {
			write_addr(NES, operand, read_addr(NES, operand) | 0x01);
		}
		update_FLAG_N(read_addr(NES, operand));
		update_FLAG_Z(read_addr(NES, operand));
	}
	/* Update Flag */
	tmp = tmp >> 7; /* needed so that function below works */;
	set_or_clear_CARRY(tmp);
}


/* execute_ROR: ROR command - Rotate Shift Right one bit (Acc or mem)
 * ROR == LSR (execpt MSB = carry & LSB copied into carry)
 */
void execute_ROR(enum MODES address_mode, size_t operand)
{
	if (address_mode == ACC) {
		/* Accumulator - ROR #Operand */
		strcpy(instruction, "ROR A");
		tmp = NES->A & 0x01; /* Mask 0th bit */
		NES->A = NES->A >> 1; /* Shift right */
		if ((NES->P & FLAG_C) == 0x01) {
			NES->A |= 0x80; /* Set 7th bit to 1 - if carry = 1 */
		} /* if carry = 0 then do nothing as that still leaves a zero in the 0th bit */
		update_FLAG_N(NES->A);
		update_FLAG_Z(NES->A);
	} else {
		strcpy(instruction, "ROR ");
		strcat(instruction, end);
		tmp = read_addr(NES, operand) & 0x01;
		write_addr(NES, operand, read_addr(NES, operand) >> 1);
		if ((NES->P & FLAG_C) == 0x01) {
			/* Set 7th bit to 1 - if carry = 1 */
			write_addr(NES, operand, read_addr(NES, operand) | 0x80);
		} /* if carry = 0 then do nothing as that still leaves a zero in the 0th bit */
		update_FLAG_N(read_addr(NES, operand));
		update_FLAG_Z(read_addr(NES, operand));
	}
	/* Update Carry */
	set_or_clear_CARRY(tmp);
}

/***************************
 * BRANCH                  *
 * *************************/
/* all are in RELATIVE address mode : -126 to +129 on pc (due to +2 @ end) */
/* No flag changes */

/* execute_BCC: BCC command - Branch on Carry Clear (C = 0)
 */
void execute_BCC(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BCC $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_C) == 0x00) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/* execute_BCS: BCS command - Branch on Carry Set (C = 1)
 */
void execute_BCS(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BCS $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_C) == 0x01) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/* execute_BEQ: BEQ command - Branch on Zero result (Z = 1)
 */
void execute_BEQ(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BEQ $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_Z) == FLAG_Z) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/* execute_BMI: BMI command - Branch on Minus result (N = 1)
 */
void execute_BMI(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BMI $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_N) == FLAG_N) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/* execute_BNE: BNE command - Branch on NOT Zero result (Z = 0)
 */
void execute_BNE(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BNE $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_Z) == 0x00) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/* execute_BPL: BPL command - Branch on Plus result (N = 0)
 */
void execute_BPL(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BPL $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_N) == 0x00) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/* execute_BVC: BVC command - Branch on Overflow Clear (V = 0)
 */
void execute_BVC(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BVC $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_V) == 0x00) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/* execute_BVS: BVS command - Branch on Overflow Set (V = 1)
 */
void execute_BVS(uint8_t *ptr_code)
{
	/* Debugger */
	sprintf(append_int, "%.4X", NES->PC + 2 + (int8_t) *(ptr_code+1));
	strcpy(instruction, "BVS $");
	strcat(instruction, append_int);

	if ((NES->P & FLAG_V) == FLAG_V) {
		tmp = NES->PC + 2;
		NES->PC += (int8_t) *(ptr_code+1);
		NES->Cycle += 1 + PAGE_CROSS(tmp, NES->PC);
	}
	NES->PC += 2;
}


/***************************
 * JUMP                    *
 * *************************/

/* execute_JMP: JMP command - JuMP to another location
 */
void execute_JMP(size_t operand)
{
	strcpy(instruction, "JMP ");
	strcat(instruction, end);
	NES->PC = operand;
}


/* execute_JSR: JSR command - Jump to SubRoutine
 */
void execute_JSR(size_t operand)
{
	strcpy(instruction, "JSR $");
	sprintf(append_int, "%.4zX", operand);
	strcat(instruction, append_int);
	/* Absolute - JSR operand */
	/* PC + 2 is pushed onto stack - always PUSH high byte first */
	PUSH((uint8_t) ((NES->PC - 1) >> 8)); /* Push PCH (PC High byte onto stack) */
	PUSH((uint8_t) (NES->PC - 1)); /* Push PCL (PC Low byte onto stack) */

	NES->PC = operand;
	/* NB: get_op_ABS_offset sets PC to += 3 therfore to push PC + 2
	 * all we need to do is just push PC - 1
	 */
}



/* execute_RTI: RTI command - ReTurn from Interrupt
 */
void execute_RTI(void)
{
	/* Implied */
	strcpy(instruction, "RTI");
	/* PULL SR */
	tmp = PULL();
	NES->P = tmp | 0x20; /* Bit 5 is always set */
	/* PULL PC */
	tmp = PULL(); /* PULL PCL */
	operand = PULL(); /* PULL PCH */
	NES->PC = tmp | (operand << 8);
}


/* execute_RTS: RTS command - ReTurn from Sub-routine
 */
void execute_RTS(void)
{
	/* Implied */
	strcpy(instruction, "RTS");
	/* opposite of JSR - PULL PCL first */
	tmp = PULL(); /* Pull PCL */
	operand = PULL(); /* Pull PCH */
	NES->PC = (operand << 8) | tmp ;
	++NES->PC;

}

/***************************
 * Registers               *
 * *************************/

/* execute_CLC: CLC command - Clear Carry flag
 */
void execute_CLC(void)
{
	/* CLC */
	strcpy(instruction, "CLC");
	NES->P &= ~(FLAG_C); /* set Flag C to: 11111110 then AND to P */
}


/* execute_CLD: CLD command - Clear Decimal Mode (Decimal mode not supported in NES) 
 */
void execute_CLD(void)
{
	/* CLD */
	strcpy(instruction, "CLD");
	NES->P &= ~(FLAG_D);

}


/* execute_CLI: CLI command - Clear Interrupt disable bit
 */
void execute_CLI(void)
{
	/* CLI */
	strcpy(instruction, "CLI");
	NES->P &= ~(FLAG_I);
}


/* execute_CLV: CLV command - Clear Overflow flag
 */
void execute_CLV(void)
{
	/* CLV */
	strcpy(instruction, "CLV");
	NES->P &= ~(FLAG_V);
}


/* execute_CMP: CMP command - Compare mem w/ A (A - M then set flags)
 */
void execute_CMP(enum MODES address_mode, size_t operand)
{
	/* CMP - same as SBC except result isn't stored and V flag isn't changed */
	Base10toBase2(NES->A, bin_operand1);
	if (address_mode == IMM) {
		/* Immediate - SBC #Operand */
		strcpy(instruction, "CMP #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		Base10toBase2(operand ^ 0xFF, bin_operand2);
	} else {
		strcpy(instruction, "CMP ");
		strcat(instruction, end);
		Base10toBase2(read_addr(NES, operand) ^ 0xFF, bin_operand2);
	}
	full_adder(bin_operand1, bin_operand2, 1, &tmp, bin_result);
	set_or_clear_CARRY(tmp);
	operand = Base2toBase10(bin_result, 0);
	update_FLAG_N(operand);
	update_FLAG_Z(operand);
}


/* execute_CPX: CPX command - Compare mem w/ X (X - M then set flags)
 */
void execute_CPX(enum MODES address_mode, size_t operand)
{
	Base10toBase2(NES->X, bin_operand1);
	if (address_mode == IMM) {
		/* Immediate - SBC #Operand */
		strcpy(instruction, "CPX #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		Base10toBase2(operand ^ 0xFF, bin_operand2);
	} else {
		strcpy(instruction, "CPX ");
		strcat(instruction, end);
		Base10toBase2(read_addr(NES, operand) ^ 0xFF, bin_operand2);
	}
	full_adder(bin_operand1, bin_operand2, 1, &tmp, bin_result);
	set_or_clear_CARRY(tmp);
	operand = Base2toBase10(bin_result, 0);
	update_FLAG_N(operand);
	update_FLAG_Z(operand);
}


/* execute_CPY: CPY command - Compare mem w/ Y (Y - M then set flags)
 */
void execute_CPY(enum MODES address_mode, size_t operand)
{
	Base10toBase2(NES->Y, bin_operand1);
	if (address_mode == IMM) {
		/* Immediate - SBC #Operand */
		strcpy(instruction, "CPY #$");
		sprintf(append_int, "%.2zX", operand);
		strcat(instruction, append_int);
		Base10toBase2(operand ^ 0xFF, bin_operand2);
	} else {
		//printf("CPY $%.4zX    ", operand);
		strcpy(instruction, "CPY ");
		strcat(instruction, end);
		Base10toBase2(read_addr(NES, operand) ^ 0xFF, bin_operand2);
	}
	full_adder(bin_operand1, bin_operand2, 1, &tmp, bin_result);
	set_or_clear_CARRY(tmp);
	operand = Base2toBase10(bin_result, 0);
	update_FLAG_N(operand);
	update_FLAG_Z(operand);
}


/* execute_SEC: SEC command - Set Carry flag (C = 1)
 */
void execute_SEC(CPU_6502 *NESCPU)
{
	/* SEC */
	strcpy(instruction, "SEC");
	NESCPU->P |= FLAG_C;
}


/* execute_SED: SED command - Set Decimal Mode (Decimal mode not supported in NES) 
 */
void execute_SED(CPU_6502 *NESCPU)
{
	/* SED */
	//printf("SED          ");
	strcpy(instruction, "SED");
	NESCPU->P |= FLAG_D;
}


/* execute_SEI: SEI command - Set Interrupt disable bit (I = 1)
 */
void execute_SEI(CPU_6502 *NESCPU)
{
	/* SEI */
	//printf("SEI          ");
	strcpy(instruction, "SEI");
	NESCPU->P |= FLAG_I;
}


/***************************
 * SYSTEM                  *
 * *************************/

/* execute_BRK: BRK command - Fore Break - Store PC & P (along w/ X, Y & A) 
 */
void execute_BRK(void)
{
	/* 2 Pushes onto stack
	 * A) PC + 2 (2 seperate pushes as stack = 8 bits vs 16 bit PC)
	 * B) Status Register (w/ bits 4 & 4 set)
	 *
	 * - also set I in Status reg
	 * RTI does the inverese POPs A & B
	 */

	/* PC + 2 is pushed onto stack - always PUSH high byte first */
	strcpy(instruction, "BRK");
	PUSH((uint8_t) ((NES->PC + 2) >> 8)); /* Push PCH (PC High byte onto stack) */
	PUSH((uint8_t) (NES->PC + 2)); /* Push PCL (PC Low byte onto stack) */
	PUSH(NES->P | 0x30);           /* PUSH Staus Reg - w/ bits 4 & 5 set */
	NES->P |= FLAG_I;              /* Flag I is set */
	NES->PC = (read_addr(NES, 0xFFFF) << 8) | read_addr(NES, 0xFFFE);
}


/* execute_NOP: NOP command - Does nothing (No OPeration)
 */
void execute_NOP(void)
{
	strcpy(instruction, "NOP");
}

/* Non opcode interrupts */
void execute_IRQ(void)
{
	strcpy(instruction, "IRQ");
	/* PC is pushed onto stack, high byte first */
	PUSH((uint8_t) ((NES->PC + 2) >> 8)); /* Push PCH (PC High byte onto stack) */
	PUSH((uint8_t) (NES->PC + 2)); /* Push PCL (PC Low byte onto stack) */
	/* PUSH Staus Reg - w/ bits 4 & 5 clear */
	PUSH(NES->P & ~(0x30));
	/* Flag I is set */
	NES->P |= FLAG_I;
	NES->PC = (read_addr(NES, 0xFFFF) << 8) | read_addr(NES, 0xFFFE);
}


void execute_NMI(void)
{
	strcpy(instruction, "NMI");
	/* PC is pushed onto stack, high byte first */
	PUSH((uint8_t) (NES->PC >> 8)); /* Push PCH (PC High byte onto stack) */
	PUSH((uint8_t) NES->PC); /* Push PCL (PC Low byte onto stack) */
	/* PUSH Staus Reg - w/ bits 4 & 5 clear */
	PUSH(NES->P & ~(0x30));
	/* Flag I is set */
	NES->P |= FLAG_I;
	NES->PC = (read_addr(NES, 0xFFFB) << 8) | read_addr(NES, 0xFFFA);
}
