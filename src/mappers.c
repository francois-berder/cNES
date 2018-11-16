#include "mappers.h"
#include "cart.h"
#include <stdio.h>

/* Mapper Registers & Banks */
uint8_t *PRG_ROM_BANK_1; // till BFFF
uint8_t *PRG_ROM_BANK_2; // till FFFF
uint8_t *CHR_ROM; // till 1FFF

/* NROM mapper */
void mapper_000(Cartridge* cart, CPU_6502* CPU, PPU_Struct* p)
{
	uint8_t *prg_rom_bank_1 = &CPU->RAM[0x8000]; // till BFFF
	uint8_t *prg_rom_bank_2 = &CPU->RAM[0xC000]; // till FFFF
	if (cart->prg_rom.size == 16 * KiB) {
		memcpy(prg_rom_bank_1, cart->prg_rom.data, 16 * KiB); // First 16KiB
		memcpy(prg_rom_bank_2, prg_rom_bank_1, 16 * KiB); // Last 16KiB
	} else {
		memcpy(prg_rom_bank_1, cart->prg_rom.data, 32 * KiB);
	}
	free(cart->prg_rom.data);
	uint8_t *chr_rom = &p->VRAM[0x0000]; // till 1FFF
	if (cart->chr_rom.size) {
		memcpy(chr_rom, cart->chr_rom.data, 8 * KiB);
	}
	free(cart->chr_rom.data);
}

/* MMC1 mapper */
void mapper_001(Cartridge* cart)
{
	// NOT FINISHED
	uint8_t *PRG_ROM_BANK_1 = &NES->RAM[0x8000]; /* till BFFF */
	uint8_t *PRG_ROM_BANK_2 = &NES->RAM[0xC000]; /* till FFFF */
	if (cart->prg_rom.size == 16 * KiB) {
		memcpy(PRG_ROM_BANK_1, cart->prg_rom.data, 16 * KiB); /* First 16KiB */
		memcpy(PRG_ROM_BANK_2, PRG_ROM_BANK_1, 16 * KiB); /* Last 16KiB */
	} else {
		memcpy(PRG_ROM_BANK_1, cart->prg_rom.data, 32 * KiB);
	}
	free(cart->prg_rom.data);
}


/* CNROM mapper */
//void mapper_003(Cartridge* cart)
//{
	// NOT FINISHED
	//uint8_t *PRG_ROM_BANK_1 = &NES->RAM[0x8000]; /* till BFFF */
	//uint8_t *PRG_ROM_BANK_2 = &NES->RAM[0xC000]; /* till FFFF */
	//if (cart->prg_rom.size == 16 * KiB) {
		//memcpy(PRG_ROM_BANK_1, cart->prg_rom.data, 16 * KiB); /* First 16KiB */
		//memcpy(PRG_ROM_BANK_2, PRG_ROM_BANK_1, 16 * KiB); /* Last 16KiB */
	//} else {
		//memcpy(PRG_ROM_BANK_1, cart->prg_rom.data, 32 * KiB);
	//}
	// Writing to PPU->RAM[0 to 1FFF]
	//uint8_t *CHR_ROM_BANK = &PPU->RAM[0x0000]; /* till 1FFF */
	//memcpy(CHR_ROM_BANK, cart->chr_rom.data, 8 * KiB);
	//free(cart->prg_rom.data);
//}
