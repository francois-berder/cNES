#ifndef __NES_PPU__
#define __NES_PPU__

#include <stdbool.h>
#include "cpu.h"

#define PPUCTRL_ADDR 0x2000 /* ADD Define addr consts???? */
#define KiB (1024)


/* Struct */
typedef struct {
	uint8_t R;
	uint8_t G;
	uint8_t B;
} RGB;

extern unsigned int pixel_color;

typedef struct {
	/* Registers */
	uint8_t PPU_CTRL;   /* $2000 */
	uint8_t PPU_MASK;   /* $2001 */
	uint8_t PPU_STATUS; /* $2002 */
	uint8_t OAM_ADDR;   /* $2003 */
	uint8_t OAM_DATA;   /* $2004 */
	uint8_t PPU_SCROLL; /* $2005 */
	uint8_t PPU_ADDR;   /* $2006 */
	uint8_t PPU_DATA;   /* $2007 */

	uint8_t VRAM[16 * KiB]; /* PPU memory space (VRAM) */
	uint8_t OAM[256]; /* OAM Address Space */

	/* BACKROUND */
	uint16_t vram_addr; /* VRAM address - LoopyV (v) */
	uint16_t vram_tmp_addr; /* Temp VRAM address - LoopyT (t) */
	uint16_t bit_L; /* Bitmap low - Shift Reg */
	uint16_t bit_H; /* Bitmap low - Shift Reg */
	uint8_t pal_L; /* Pallette attribute low - Shift reg */
	uint8_t pal_H; /* Pallette attribute low - Shift reg */
	uint8_t fineX; /* Fine X Scroll - only lower 4 bits are used */
	bool toggle_w; /* 1st/2nd Write toggle */

	int palette[4]; /* Stores the 4 colour palette */

	uint32_t scanline;
} PPU_Struct;

/* Global defintions */
const uint8_t reverse_bits[256];
static const unsigned int palette[64];

/* Initialise Function */
PPU_Struct *PPU; /* Global NES PPU Pointer */
PPU_Struct *ppu_init();

/* Read & Write Functions */
//uint8_t read_PPU();
uint8_t read_PPU_Reg(uint16_t addr, PPU_Struct *p); /* For addresses exposed to CPU */
void write_PPU_Reg(uint16_t addr, uint8_t data, PPU_Struct *p); /* For addresses exposed to CPU */

uint8_t read_2002(uint16_t addr, PPU_Struct *p);
//uint8_t read_2004(uint16_t addr, PPU_Struct *p);
//uint8_t read_2007(uint16_t addr, PPU_Struct *p);

void write_2000(uint8_t data, PPU_Struct *p); /* PPU_CTRL */
void write_2004(uint8_t data, PPU_Struct *p); /* OAM_DATA */
void write_2005(uint8_t data, PPU_Struct *p); /* PPU_SCROLL */
void write_2006(uint8_t data, PPU_Struct *p); /* PPU_ADDR */
void write_2007(uint8_t data, PPU_Struct *p); /* PPU_DATA */


/* PPU_CTRL FUNCTIONS */
//uint16_t ppu_nametable_addr();
uint8_t ppu_vram_addr_inc(PPU_Struct *p);
//uint16_t ppu_sprite_pattern_table_addr();
//uint16_t ppu_bg_pattern_table_addr();
//uint8_t ppu_sprite_height();
//bool ppu_gen_nmi();

/* PPU_CTRL FUNCTIONS */
//bool ppu_show_greyscale();
//bool ppu_bg_show_left_8();
//bool ppu_sprite_show_left_8();
//bool ppu_show_bg();
//bool ppu_show_sprite();
//bool ppu_intense_reds();
//bool ppu_intense_greens();
//bool ppu_intense_blues();

/* PPU_CTRL FUNCTIONS */
//bool ppu_sprite_overflow();
//bool ppu_sprite_0_hit();
//bool ppu_vblank();


#endif /* __NES_PPU__ */
