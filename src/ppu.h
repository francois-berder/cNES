#ifndef __NES_PPU__
#define __NES_PPU__

#include <stdbool.h>
#include <stdio.h>
#include "cpu.h"
#include "gui.h"

#define KiB (1024)

/* Struct */
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

	uint8_t return_value;

	uint8_t buffer_2007; /* Read buffer for register 2007 */

	/* Memory */
	uint8_t VRAM[16 * KiB]; /* PPU memory space (VRAM) */
	uint8_t OAM[256]; /* OAM Address Space (Sprite RAM) */

	/* Sprites */
	uint8_t scanline_OAM[32]; // Secondary OAM, change to scanline
	uint8_t OAM_read_buffer;
	unsigned sprites_found; // Number of sprites found in next scanlie: MAX 8
	unsigned sprite_index; // Max 63 (0 indexed)
	bool stop_early;
	bool sprite_zero_hit;
	unsigned sprite_zero_scanline; // Scanlines of sprite 0 if any
	unsigned sprite_zero_scanline_tmp; // Scanlines of sprite 0 if any
	unsigned hit_scanline; // Needed because Y is calculated 1 scanline ahead
	unsigned hit_cycle; // Needed because Y is calculated 1 scanline ahead
	uint16_t sprite_addr;
	uint8_t sprite_at_latches[8]; // Holds attribute byte data for 8 sprites
	uint8_t sprite_pt_lo_shift_reg[8];
	uint8_t sprite_pt_hi_shift_reg[8];
	uint8_t sprite_x_counter[8]; /* X pos of sprite (decremented every 8 cycles */

	/* BACKROUND */
	uint16_t vram_addr; /* VRAM address - LoopyV (v) */
	uint16_t vram_tmp_addr; /* Temp VRAM address - LoopyT (t) */
	uint8_t fineX; /* Fine X Scroll - only lower 4 bits are used */
	bool write_toggle; /* 1st/2nd Write toggle */

	/* Latches & Shift registers */
	uint8_t pt_lo_latch; /* Most recent fetch pt_lo fetch */
	uint8_t pt_hi_latch;
	uint16_t pt_lo_shift_reg; /* Stores a 16 pixels in the pipeline - 1st 8 pixels to be rendered are in lowest byte */
	uint16_t pt_hi_shift_reg;
	uint8_t nt_byte; /* NT byte that is rendered 16 PPU cycles later */
	uint8_t at_latch; /* AT byte that is rendered 16 PPU cycles later */
	uint8_t at_current; /* AT byte for pixels 1 - 8 in pipeline */
	uint8_t at_next; /* AT byte for pixels 9 - 16 in pipeline */

	uint16_t nt_addr_tmp; /* Address used to generate nt byte */
	uint16_t nt_addr_next; /* Next tile address for pixels 9 -16 in pipeline */
	uint16_t nt_addr_current; /* Current tile address for pixels 1 - 8 in pipeline */

	//int palette[4]; /* Stores the 4 colour palette */
	bool reset_1;
	bool reset_2;

	unsigned mirroring; // 0 = Horz, 1 = vert, 4 = 4 screen

	uint32_t scanline; /* Pre-render = 261, visible = 0 - 239, post-render 240 - 260 */
	uint32_t nmi_start; /* Scanline in which NMI starts - set value depending on NTSC or PAL */
	const uint32_t nmi_end; /* Scanline in which NMI end */
	uint16_t cycle; /* PPU Cycles, each PPU mem access takes 2 cycles */
	uint16_t old_cycle; /* Runs much quicker when 16 wide instead of 32 and fixes a weird overflow bug */
	// HASN'T FIXED WEIRD OVERFLOW
} PPU_Struct;

enum Memory {
	PRIMARY_OAM,
	SECONDARY_OAM,
	PATTERN_TABLE_1
} ppu_mem;

/* Global defintions */
static const uint8_t reverse_bits[256];
static const unsigned int palette[64];

/* Initialise Function */
PPU_Struct *PPU; /* Global NES PPU Pointer */
PPU_Struct *ppu_init();
void debug_entry(PPU_Struct *p);
void debug_exit(PPU_Struct *p);
void ppu_reset(int start, PPU_Struct *p, Cpu6502* CPU); /* Emulates reset/warm-up of PPU */

/* Debug Functions */
void append_ppu_info(void);
void debug_ppu_regs(Cpu6502* CPU);
void PPU_MEM_DEBUG(void); // rename to VRAM viewer
void OAM_viewer(enum Memory ppu_mem); // rename to VRAM viewer

/* Read & Write Functions */
//uint8_t read_PPU();
uint8_t read_PPU_Reg(uint16_t addr, PPU_Struct *p); /* For addresses exposed to CPU */
void write_PPU_Reg(uint16_t addr, uint8_t data, PPU_Struct *p, Cpu6502* CPU); /* For addresses exposed to CPU */

void write_vram(uint8_t data, PPU_Struct *p);

void read_2002(PPU_Struct *p);
//uint8_t read_2004(uint16_t addr, PPU_Struct *p);
void read_2007(PPU_Struct *p);

void write_2000(uint8_t data, PPU_Struct *p); /* PPU_CTRL */
void write_2004(uint8_t data, PPU_Struct *p); /* OAM_DATA */
void write_2005(uint8_t data, PPU_Struct *p); /* PPU_SCROLL */
void write_2006(uint8_t data, PPU_Struct *p); /* PPU_ADDR */
void write_2007(uint8_t data, PPU_Struct *p); /* PPU_DATA */
void write_4014(uint8_t data, PPU_Struct *p, Cpu6502* NESCPU); /* DMA_DATA */


/* PPU_CTRL FUNCTIONS */
//uint16_t ppu_nametable_addr();
uint8_t ppu_vram_addr_inc(PPU_Struct *p);
uint16_t ppu_base_nt_address(PPU_Struct *p);
uint16_t ppu_base_pt_address(PPU_Struct *p);
uint16_t ppu_sprite_pattern_table_addr(PPU_Struct *p);
//uint16_t ppu_bg_pattern_table_addr();
uint8_t ppu_sprite_height(PPU_Struct *p);
//bool ppu_gen_nmi();

/* PPU_MASK FUNCTIONS */
//bool ppu_show_greyscale();
//bool ppu_bg_show_left_8();
//bool ppu_sprite_show_left_8();
bool ppu_show_bg(PPU_Struct *p);
bool ppu_show_sprite(PPU_Struct *p);
//bool ppu_intense_reds();
//bool ppu_intense_greens();
//bool ppu_intense_blues();

/* PPU_CTRL FUNCTIONS */
//bool ppu_sprite_overflow();
//bool ppu_sprite_0_hit();
//bool ppu_vblank();

void inc_vert_scroll(PPU_Struct *p);
void inc_horz_scroll(PPU_Struct *p);

void fetch_nt_byte(PPU_Struct *p);
void fetch_at_byte(PPU_Struct *p);
void fetch_pt_lo(PPU_Struct *p);
void fetch_pt_hi(PPU_Struct *p);

void render_pixel(PPU_Struct *p);

void ppu_transfer_oam(PPU_Struct* p, unsigned index);
void reset_secondary_oam(PPU_Struct* p);
void sprite_evaluation(PPU_Struct* p);

void ppu_tick(PPU_Struct *p);
void ppu_step(PPU_Struct *p, Cpu6502* NESCPU);


#endif /* __NES_PPU__ */
