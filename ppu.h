#ifndef __NES_PPU__
#define __NES_PPU__

#include <stdbool.h>
#include <stdio.h>
#include "cpu.h"
#include "opcode_functions.h"

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
	/* Registers  - maybe make these pointers to the CPU mem space */
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
	uint8_t fineX; /* Fine X Scroll - only lower 4 bits are used */
	bool toggle_w; /* 1st/2nd Write toggle */
	uint8_t temp;

	/* Latches & Shift registers */
	uint8_t pt_lo_latch; /* Most recent fetch pt_lo fetch */
	uint8_t pt_hi_latch;
	uint16_t pt_lo_shift_reg; /* Stores a 16 pixels in the pipeline - 1st 8 pixels to be rendered are in lowest byte */
	uint16_t pt_hi_shift_reg;
	uint8_t at_byte_current; /* Current at byte (pixels 1- 8 in the pipeline) */
	uint8_t at_byte_next; /* Next byte for at byte (pixels 9 - 16 in the pipeline) */

	//int palette[4]; /* Stores the 4 colour palette */
	bool RESET_1;
	bool RESET_2;

	uint32_t scanline; /* Pre-render = 261, visible = 0 - 239, post-render 240 - 260 */
	uint32_t nmi_start; /* Scanline in which NMI starts - set value depending on NTSC or PAL */
	const uint32_t nmi_end; /* Scanline in which NMI end */
	uint32_t cycle; /* PPU Cycles, each PPU mem access takes 2 cycles */
	uint32_t old_cycle; /* */
} PPU_Struct;

/* Global defintions */
const uint8_t reverse_bits[256];
static const unsigned int palette[64];

/* Initialise Function */
PPU_Struct *PPU; /* Global NES PPU Pointer */
PPU_Struct *ppu_init();
void ppu_reset(int start, PPU_Struct *p); /* Emulates reset/warm-up of PPU */

/* Read & Write Functions */
//uint8_t read_PPU();
uint8_t read_PPU_Reg(uint16_t addr, PPU_Struct *p); /* For addresses exposed to CPU */
void write_PPU_Reg(uint16_t addr, uint8_t data, PPU_Struct *p); /* For addresses exposed to CPU */

uint8_t read_2002(PPU_Struct *p);
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
uint16_t ppu_base_nt_address(PPU_Struct *p);
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


void ppu_tick(PPU_Struct *p);
void ppu_step(PPU_Struct *p, CPU_6502* NESCPU);


#endif /* __NES_PPU__ */
