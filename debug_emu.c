/* Reads NES ROM and executes instructions we encounter */

#include <stdio.h>
#include "functions_generic.h"
#include "functions.h"
#include "opcode_debug.h"
#include "cart.h"
#include "ppu.h"

const char *filename = "super_mario_bros.nes";
//const char *filename = "donkey_kong.nes";

#define __LOG__

/* comment out above and uncomment below to disable logging to a file
#undef __LOG__
*/

void debug_ppu()
{
	printf("2000: %.2X\n", read_addr(NES, 0x2000));
	printf("2001: %.2X\n", read_addr(NES, 0x2001));
	printf("2002: %.2X\n", read_addr(NES, 0x2002));
	printf("2003: %.2X\n", read_addr(NES, 0x2003));
	printf("2004: %.2X\n", read_addr(NES, 0x2004));
	printf("2005: %.2X\n", read_addr(NES, 0x2005));
	printf("2006: %.2X\n", read_addr(NES, 0x2006));
	printf("2007: %.2X\n", read_addr(NES, 0x2007));
	printf("3F00: %.2X\n", read_addr(NES, 0x3F00));
	printf("3F01: %.2X\n\n", read_addr(NES, 0x3F01));
}

void PPU_PT_DEBUG(void) {
	printf("      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	unsigned int addr = 0;
	unsigned int mem = 0;
	while (addr < 600) {
		printf("%.4X: ", addr << 4);
		for (int x = 0; x < 16; x++) {
			printf("%.2X ", PPU->VRAM[mem]);
			++mem;
		}
		printf("\n");
		++addr;
	}
}

int main(int argc, char **argv)
{
	NES = NES_CPU(0xC000);
#define __RESET__
	PPU = ppu_init();

	Cartridge* cart = malloc(sizeof(Cartridge));
	load_cart(cart, filename);
	free(cart);
	NES_PC(NES); /* Set PC to reset vector */
	// Can combine aove code into a NES init function

#ifdef __LOG__
	stdout = freopen("log-smb1.txt", "w", stdout);
#endif /*__LOG__ */

	// CURRENTLY TESTING PPU READ/WRITE + INTERACTIONS W/CPU ($2000 - $2007)
	int i = 0;
	
	while (i < 104615) {
		Debug_6502(NES->PC);
		Debug_6502(NES->PC);
		Debug_6502(NES->PC);
		ppu_step(PPU);
		RET_NES_CPU();
		//debug_ppu();
		++i;
	}
	

	// Testing read/write of PPU
	/*
	write_2006(0x20, PPU);
	write_2006(0x00, PPU);
	write_2007(0x24, PPU);
	*/
	
	// TEST 2: same as bove
	/*
	write_PPU_Reg(0x2006, 0x20, PPU);
	write_PPU_Reg(0x2006, 0x00, PPU);
	write_PPU_Reg(0x2007, 0x24, PPU);
	*/
	// Pattern table viewer
	//PPU_PT_DEBUG();
	return 0;
}
