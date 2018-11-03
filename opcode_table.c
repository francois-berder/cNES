/* Reads NES ROM and execute's what instructions we encounter.
 * Also modify PC and CPU cycles for each opcode. Page boundry
 * calculations occur here except for branch instructions where
 * they are computed in "opcode_functions.c".
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "opcode_functions.h" /* + cpu.h +*/
#include "helper_functions.h"
#include "opcode_table.h"

void CPU_6502_STEP(uint16_t PC)
{
	transfer_cpu();
	uint8_t *opcode = &NES->RAM[PC];


	/* Process NMI */
	if (!NES->NMI_PENDING) {
		switch (opcode[0]) {
		case 0x00:
			/* BRK */
			execute_BRK();
			NES->PC += 2;
			NES->Cycle += 7;
			break;
		case 0x01:
			/* ORA - Indirect X mode*/
			operand = get_op_INDX(opcode, NES);
			execute_ORA(INDX, operand);
			NES->Cycle += 6;
			break;
		case 0x05:
			/* ORA - Zero page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_ORA(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0x06:
			/* ASL - Zero page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_ASL(ZP, operand);
			NES->Cycle += 5;
			break;
		case 0x08:
			/* PHP */
			// 3 + 10 spaces 
			strcpy(instruction, "PHP");
			PUSH(NES->P | 0x30); /* Set Bits 4 & 5 to 1 for PHP */
			++NES->PC;
			NES->Cycle += 3;
			break;
		case 0x09:
			/* ORA - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_ORA(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0x0A:
			/* ASL - Accumulator mode */
			execute_ASL(ACC, operand); /* Doesn't use operand in this case */
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x0D:
			/* ORA - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_ORA(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0x0E:
			/* ASL - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_ASL(ABS, operand);
			NES->Cycle += 6;
			break;
		case 0x10:
			/* BPL */
			execute_BPL(opcode);
			NES->Cycle += 2; // Branch not taken, +1 if taken (in execute function)
			break;
		case 0x11:
			/* ORA - Indirect Y mode */
			operand = get_op_INDY(opcode, NES);
			execute_ORA(INDY, operand);
			NES->Cycle += 5; // Branch not taken, +1 if taken (in execute function)
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0x15:
			/* ORA - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ORA(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0x16:
			/* ASL - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ASL(ZPX, operand);
			NES->Cycle += 6;
			break;
		case 0x18:
			/* CLC */
			execute_CLC();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x19:
			/* ORA - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_ORA(ABSY, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0x1D:
			/* ORA - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ORA(ABSX, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			break;
		case 0x1E:
			/* ASL - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ASL(ABSX, operand);
			NES->Cycle += 7;
			break;
		case 0x20:
			/* JSR - Absolute mode*/
			operand = get_op_ABS_offset(opcode, 0);
			execute_JSR();
			NES->Cycle += 6;
			break;
		case 0x21:
			/* AND - Indirect X mode*/
			operand = get_op_INDX(opcode, NES);
			execute_AND(INDX, operand);
			NES->Cycle += 6;
			break;
		case 0x24:
			/* BIT - Zero Page mode*/
			operand = get_op_ZP_offset(opcode, 0);
			execute_BIT(operand);
			NES->Cycle += 3;
			break;
		case 0x25:
			/* AND - Zero Page mode*/
			operand = get_op_ZP_offset(opcode, 0);
			execute_AND(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0x26:
			/* ROL - Zero Page mode*/
			operand = get_op_ZP_offset(opcode, 0);
			execute_ROL(ZP, operand);
			NES->Cycle += 5;
			break;
		case 0x28:
			/* PLP */
			strcpy(instruction, "PLP");
			NES->P = PULL() & ~(0x10); /* B flag may exist on stack not on P */
			NES->P |= 0x20; /* bit 5 always set */
			++NES->PC;
			NES->Cycle += 4;
			break;
		case 0x29:
			/* AND - Immediate mode*/
			operand = get_op_IMM(opcode);
			execute_AND(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0x2A:
			/* ROL - Accumulator mode*/
			execute_ROL(ACC, operand); /* operand isn't used in this case */
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x2C:
			/* BIT - Absolute mode*/
			operand = get_op_ABS_offset(opcode, 0);
			execute_BIT(operand);
			NES->Cycle += 4;
			break;
		case 0x2D:
			/* AND - Absolute mode*/
			operand = get_op_ABS_offset(opcode, 0);
			execute_AND(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0x2E:
			/* ROL - Absolute mode*/
			operand = get_op_ABS_offset(opcode, 0);
			execute_ROL(ABS, operand);
			NES->Cycle += 6;
			break;
		case 0x30:
			/* BMI */
			execute_BMI(opcode);
			NES->Cycle += 2; // Branch not taken, additional cycles added in execution function
			break;
		case 0x31:
			/* AND - Indirect Y mode*/
			operand = get_op_INDY(opcode, NES);
			execute_AND(INDY, operand);
			NES->Cycle += 5;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0x35:
			/* AND - Zero Page X mode*/
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_AND(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0x36:
			/* ROL - Zero Page X mode*/
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ROL(ZPX, operand);
			NES->Cycle += 6;
			break;
		case 0x38:
			/* SEC */
			execute_SEC(NES);
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x39:
			/* AND - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_AND(ABSY, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0x3D:
			/* AND - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_AND(ABSX, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			break;
		case 0x3E:
			/* ROL - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ROL(ABSX, operand);
			NES->Cycle += 7;
			break;
		case 0x40:
			/* RTI */
			execute_RTI(); /* PC is pulled from stack */
			NES->Cycle += 6;
			break;
		case 0x41:
			/* EOR - Indirect X mode */
			operand = get_op_INDX(opcode, NES);
			execute_EOR(INDX, operand);
			NES->Cycle += 6;
			break;
		case 0x45:
			/* EOR - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_EOR(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0x46:
			/* LSR - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_LSR(ZP, operand);
			NES->Cycle += 5;
			break;
		case 0x48:
			/* PHA */
			strcpy(instruction, "PHA");
			PUSH(NES->A);
			++NES->PC;
			NES->Cycle += 3;
			break;
		case 0x49:
			/* EOR - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_EOR(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0x4A:
			/* LSR - Accumulator */
			execute_LSR(ACC, operand); /* operand not used */
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x4C:
			/* JMP - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_JMP(operand);
			NES->Cycle += 3;
			break;
		case 0x4D:
			/* EOR - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_EOR(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0x4E:
			/* LSR - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_LSR(ABS, operand);
			NES->Cycle += 6;
			break;
		case 0x50:
			/* BVC */
			execute_BVC(opcode);
			NES->Cycle += 2; // Branch no taken, penalties calc in exec function
			break;
		case 0x51:
			/* EOR - Indirect Y mode */
			operand = get_op_INDY(opcode, NES);
			execute_EOR(INDY, operand);
			NES->Cycle += 5;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0x55:
			/* EOR - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_EOR(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0x56:
			/* LSR - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_LSR(ZPX, operand);
			NES->Cycle += 6;
			break;
		case 0x58:
			/* CLI */
			execute_CLI();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x59:
			/* EOR - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_EOR(ABSY, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0x5D:
			/* EOR - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_EOR(ABSX, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			break;
		case 0x5E:
			/* LSR - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_LSR(ABSX, operand);
			NES->Cycle += 7;
			break;
		case 0x60:
			/* RTS */
			execute_RTS(); /* PC is pulled from stack */
			NES->Cycle += 6;
			break;
		case 0x61:
			/* ADC - Indirect X mode */
			operand = get_op_INDX(opcode, NES);
			execute_ADC(INDX, operand);
			NES->Cycle += 6;
			break;
		case 0x65:
			/* ADC - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_ADC(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0x66:
			/* ROR - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_ROR(ZP, operand);
			NES->Cycle += 5;
			break;
		case 0x68:
			/* PLA */
			strcpy(instruction, "PLA");
			NES->A = PULL();
			update_FLAG_Z(NES->A);
			update_FLAG_N(NES->A);
			++NES->PC;
			NES->Cycle += 4;
			break;
		case 0x69:
			/* ADC - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_ADC(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0x6A:
			/* ROR - Accumulator mode */
			execute_ROR(ACC, operand); /* operand isn't used */
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x6C:
			/* JMP - Indirect */
			operand = get_op_IND(opcode, NES);
			execute_JMP(operand);
			NES->Cycle += 5;
			break;
		case 0x6D:
			/* ADC - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_ADC(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0x6E:
			/* ROR - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_ROR(ABS, operand);
			NES->Cycle += 6;
			break;
		case 0x70:
			/* BVS */
			execute_BVS(opcode);
			NES->Cycle += 2; // Branch not taken, penalties in exec function
			break;
		case 0x71:
			/* ADC - Indirect Y mode */
			operand = get_op_INDY(opcode, NES);
			execute_ADC(INDY, operand);
			NES->Cycle += 5;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0x75:
			/* ADC - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ADC(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0x76:
			/* ROR - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ROR(ZPX, operand);
			NES->Cycle += 6;
			break;
		case 0x78:
			/* SEI */
			execute_SEI(NES);
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x79:
			/* ADC - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			execute_ADC(ABSY, operand);
			NES->Cycle += 4;
			break;
		case 0x7D:
			/* ADC - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			execute_ADC(ABSX, operand);
			NES->Cycle += 4;
			break;
		case 0x7E:
			/* ROR - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_ROR(ABSX, operand);
			NES->Cycle += 7;
			break;
		case 0x81:
			/* STA - Indirect X mode */
			operand = get_op_INDX(opcode, NES);
			execute_STA(operand);
			NES->Cycle += 6;
			break;
		case 0x84:
			/* STY - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_STY(operand);
			NES->Cycle += 3;
			break;
		case 0x85:
			/* STA - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_STA(operand);
			NES->Cycle += 3;
			break;
		case 0x86:
			/* STX - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_STX(operand);
			NES->Cycle += 3;
			break;
		case 0x88:
			/* DEY */
			execute_DEY();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x8A:
			/* TXA */
			execute_TXA();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x8C:
			/* STY - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_STY(operand);
			NES->Cycle += 4;
			break;
		case 0x8D:
			/* STA - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_STA(operand);
			NES->Cycle += 4;
			break;
		case 0x8E:
			/* STX - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_STX(operand);
			NES->Cycle += 4;
			break;
		case 0x90:
			/* BCC */
			execute_BCC(opcode);
			NES->Cycle += 2; // Branch not taken, penalties in exec function
			break;
		case 0x91:
			/* STA - Indirect Y mode */
			operand = get_op_INDY(opcode, NES);
			execute_STA(operand);
			NES->Cycle += 6;
			break;
		case 0x94:
			/* STY - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_STY(operand);
			NES->Cycle += 4;
			break;
		case 0x95:
			/* STA - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_STA(operand);
			NES->Cycle += 4;
			break;
		case 0x96:
			/* STX - Zero Page Y mode */
			operand = get_op_ZP_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_STX(operand);
			NES->Cycle += 4;
			break;
		case 0x98:
			/* TYA */
			execute_TYA();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x99:
			/* STA - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_STA(operand);
			NES->Cycle += 5;
			break;
		case 0x9A:
			/* TXS */
			execute_TXS();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0x9D:
			/* STA - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_STA(operand);
			NES->Cycle += 5;
			break;
		case 0xA0:
			/* LDY - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_LDY(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0xA1:
			/* LDA - Indirect X mode */
			operand = get_op_INDX(opcode, NES);
			execute_LDA(INDX, operand);
			NES->Cycle += 6;
			break;
		case 0xA2:
			/* LDX - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_LDX(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0xA4:
			/* LDY - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_LDY(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0xA5:
			/* LDA - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_LDA(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0xA6:
			/* LDX - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_LDX(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0xA8:
			/* TAY */
			execute_TAY();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xA9:
			/* LDA - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_LDA(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0xAA:
			/* TAX */
			execute_TAX();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xAC:
			/* LDY - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_LDY(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0xAD:
			/* LDA - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_LDA(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0xAE:
			/* LDX - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_LDX(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0xB0:
			/* BCS */
			execute_BCS(opcode);
			NES->Cycle += 2; // Branch not taken, penalties added in exec function
			break;
		case 0xB1:
			/* LDA - Indirect Y mode */
			operand = get_op_INDY(opcode, NES);
			execute_LDA(INDY, operand);
			NES->Cycle += 5;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0xB4:
			/* LDY - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_LDY(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0xB5:
			/* LDA - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_LDA(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0xB6:
			/* LDX - Zero Page Y mode */
			operand = get_op_ZP_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_LDX(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0xB8:
			/* CLV */
			execute_CLV();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xB9:
			/* LDA - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_LDA(ABSY, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0xBA:
			/* TSX */
			execute_TSX();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xBC:
			/* LDY - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_LDY(ABSX, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			break;
		case 0xBD:
			/* LDA - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_LDA(ABSX, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			break;
		case 0xBE:
			/* LDX - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_LDX(ABSY, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0xC0:
			/* CPY - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_CPY(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0xC1:
			/* CMP - Indirect X mode */
			operand = get_op_INDX(opcode, NES);
			execute_CMP(INDX, operand);
			NES->Cycle += 6;
			break;
		case 0xC4:
			/* CPY - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_CPY(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0xC5:
			/* CMP - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_CMP(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0xC6:
			/* DEC - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_DEC(operand);
			NES->Cycle += 5;
			break;
		case 0xC8:
			/* INY */
			execute_INY();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xC9:
			/* CMP - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_CMP(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0xCA:
			/* DEX */
			execute_DEX();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xCC:
			/* CPY - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_CPY(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0xCD:
			/* CMP - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_CMP(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0xCE:
			/* DEC - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_DEC(operand);
			NES->Cycle += 6;
			break;
		case 0xD0:
			/* BNE */
			execute_BNE(opcode);
			NES->Cycle += 2;
			break;
		case 0xD1:
			/* CMP - Indirect Y  mode */
			operand = get_op_INDY(opcode, NES);
			execute_CMP(INDY, operand);
			NES->Cycle += 5;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0xD5:
			/* CMP - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_CMP(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0xD6:
			/* DEC - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_DEC(operand);
			NES->Cycle += 6;
			break;
		case 0xD8:
			/* CLD */
			execute_CLD();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xD9:
			/* CMP - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_CMP(ABSY, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0xDD:
			/* CMP - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_CMP(ABSX, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			break;
		case 0xDE:
			/* DEC - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_DEC(operand);
			NES->Cycle += 7;
			break;
		case 0xE0:
			/* CPX - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_CPX(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0xE1:
			/* SBC - Indirect X mode */
			operand = get_op_INDX(opcode, NES);
			execute_SBC(INDX, operand);
			NES->Cycle += 6;
			break;
		case 0xE4:
			/* CPX - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_CPX(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0xE5:
			/* SBC - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_SBC(ZP, operand);
			NES->Cycle += 3;
			break;
		case 0xE6:
			/* INC - Zero Page mode */
			operand = get_op_ZP_offset(opcode, 0);
			execute_INC(operand);
			NES->Cycle += 5;
			break;
		case 0xE8:
			/* INX */
			execute_INX();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xE9:
			/* SBC - Immediate mode */
			operand = get_op_IMM(opcode);
			execute_SBC(IMM, operand);
			NES->Cycle += 2;
			break;
		case 0xEA:
			/* NOP */
			execute_NOP();
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xEC:
			/* CPX - Absolute mode*/
			operand = get_op_ABS_offset(opcode, 0);
			execute_CPX(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0xED:
			/* SBC - Absolute mode */
			operand = get_op_ABS_offset(opcode, 0);
			execute_SBC(ABS, operand);
			NES->Cycle += 4;
			break;
		case 0xEE:
			/* INC - Absolute mode*/
			operand = get_op_ABS_offset(opcode, 0);
			execute_INC(operand);
			NES->Cycle += 6;
			break;
		case 0xF0:
			/* BEQ */
			execute_BEQ(opcode);
			NES->Cycle += 2; // Branch not taken, +1 if taken (in execute_BEQ)
			break;
		case 0xF1:
			/* SBC - Indirect Y mode */
			operand = get_op_INDY(opcode, NES);
			execute_SBC(INDY, operand);
			NES->Cycle += 5;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0xF5:
			/* SBC - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_SBC(ZPX, operand);
			NES->Cycle += 4;
			break;
		case 0xF6:
			/* INC - Zero Page X mode */
			operand = get_op_ZP_offset(opcode, NES->X);
			strcat(end, ",X");
			execute_INC(operand);
			NES->Cycle += 6;
			break;
		case 0xF8:
			/* SED */
			execute_SED(NES);
			++NES->PC;
			NES->Cycle += 2;
			break;
		case 0xF9:
			/* SBC - Absolute Y mode */
			operand = get_op_ABS_offset(opcode, NES->Y);
			strcat(end, ",Y");
			execute_SBC(ABSY, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->Y);
			break;
		case 0xFD:
			/* SBC - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ", X");
			execute_SBC(ABSX, operand);
			NES->Cycle += 4;
			NES->Cycle += PAGE_CROSS(NES->target_addr, NES->target_addr - NES->X);
			break;
		case 0xFE:
			/* INC - Absolute X mode */
			operand = get_op_ABS_offset(opcode, NES->X);
			strcat(end, ", X");
			execute_INC(operand);
			NES->Cycle += 7;
			break;
		default:
			/* Invalid command */
			printf("Undocumented opcode: 0x%.2x\t", *opcode);
			++NES->PC; /* This causes a problem that undocumented opcodes advance PC by 1 */
			break;
		}
	} else {
		execute_NMI();
		NES->Cycle += 7;
	}

	if (NES->DMA_PENDING) {
		execute_DMA();
		if ((NES->Cycle - 1) & 0x01) {
			NES->Cycle += 1;
		}
		NES->Cycle += 513;
		NES->DMA_PENDING = 0;
	}
}
