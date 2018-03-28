/* Reads NES ROM and executes instructions we encounter */

#include <stdio.h>
#include "functions_generic.h"
#include "functions.h"
#include "opcode_execute.h"
#include "cart.h"
#include "ppu.h" // cpu.h is implicitly added

const char *filename = "pong.nes";
/* for nestest.mes set NES_CPU(0xC000) */

void PPU_PT_DEBUG(void) {
	printf("      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	unsigned int addr = 0;
	unsigned int mem = 0;
	while (addr < 100) {
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
	PPU = ppu_init();

	Cartridge* cart = malloc(sizeof(Cartridge));
	load_cart(cart, filename);
	free(cart);

	int i = 0;
	while (i < 123) {
		Execute_6502(NES->PC);
		RET_NES_CPU();
		++i;
	}
	

	/* Basic read/write test to ppu reg */
	/*
	write_addr(NES, 0x2000, 10);
	printf("val @ 0x2000 = %.2X\n", read_addr(NES, 0x2000));
	printf("PPU_CTRL = %.2X\n", PPU->PPU_CTRL);

	// Pattern table viewer
	PPU_PT_DEBUG();
	*/
	return 0;
}
