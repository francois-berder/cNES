/* Reads NES ROM and executes instructions we encounter */

#include <stdio.h>
#include "helper_functions.h"
#include "opcode_functions.h"
#include "opcode_table.h"
#include "cart.h"
#include "ppu.h"

//const char *filename = "nestest.nes";
//const char *filename = "super_mario_bros.nes";
const char *filename = "donkey_kong.nes";
//const char *filename = "nmi.nes";

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

void ppu_cpu_ratio(void)
{
	// 3 : 1 PPU to CPU ratio
	PPU->old_cycle = PPU->cycle;
	ppu_step(PPU, NES);
	ppu_step(PPU, NES);
	ppu_step(PPU, NES);
	CPU_6502_STEP(NES->PC);
	for (int i = 1; i < (NES->Cycle - NES->old_Cycle); i++) {
		ppu_step(PPU, NES);
		ppu_step(PPU, NES);
		ppu_step(PPU, NES);
	}
	RET_NES_CPU();
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
	// Can combine above code into a NES init function

#ifdef __LOG__
	stdout = freopen("log-dk.txt", "w", stdout);
#endif /*__LOG__ */

	int i = 0;
	//5005 - nestest, 104615 - SMB1, 42360 Donkey Kong @ 5 frames
	// 32598 SMB1 not sure why tho
	//NES->PC = 0xC000; //nes test
	while (i < 42360) {
		ppu_cpu_ratio();
		//Debug_6502(NES->PC);
		printf(" PPU_CYC: %-3d", PPU->old_cycle);
		printf(" SL: %d\n", (PPU->scanline));
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
	PPU_PT_DEBUG();
	return 0;
}
