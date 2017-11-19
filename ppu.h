#ifndef __NES_PPU__
#define __NES_PPU__

#include <stdbool.h>

#define PPUCTRL_ADDR 0x2000 /* ADD Define addr consts???? */
#define KiB (1024)

/* Read & Write Functions */
uint8_t read_PPU();
uint8_t read_PPU_Reg(uint16_t addr); /* For addresses exposed to CPU */

uint8_t read_2002();
uint8_t read_2004();
uint8_t read_2007();

void write_2000(uint8_t data); /* PPU_CTRL */
void write_2004(uint8_t data); /* OAM_DATA */
void write_2005(uint8_t data); /* PPU_SCROLL */
void write_2006(uint8_t data); /* PPU_ADDR */
void write_2007(uint8_t data); /* PPU_DATA */


/* PPU_CTRL FUNCTIONS */
uint16_t ppu_nametable_addr();
uint8_t ppu_vram_addr_inc();
uint16_t ppu_sprite_pattern_table_addr();
uint16_t ppu_bg_pattern_table_addr();
uint8_t ppu_sprite_height();
bool ppu_gen_nmi();


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

	uint8_t MEM[16 * KiB]; /* PPU memory space */
	uint8_t OAM[256]; /* OAM Address Space */

	/* BACKROUND */
	uint16_t vram_addr; /* VRAM address */
	uint16_t vram_tmp_addr; /* Temp VRAM address */
	uint16_t bit_L; /* Bitmap low - Shift Reg */
	uint16_t bit_H; /* Bitmap low - Shift Reg */
	uint8_t pal_L; /* Pallette attribute low - Shift reg */
	uint8_t pal_H; /* Pallette attribute low - Shift reg */
	uint8_t fineX; /* Fine X Scroll */
	bool toggle_w; /* 1st/2nd Write toggle */

	int palette[4]; /* Stores the 4 colour palette */

	uint32_t scanline;
} PPU_Struct;

static const unsigned int palette[64];

/* Initialise Function */
void ppu_init();

/* More functions */
void ppu_INCX();
void ppu_INCY();


void sprite_render();
void bg_render();


#endif /* __NES_PPU__ */
