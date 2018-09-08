#include "ppu.h"

// Later add _PPU_IO_BUS (PPUGenLatch in FCEUX)


/* Reverse bits lookup table for an 8 bit number */
const uint8_t reverse_bits[256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

/* RGB values of NES colour palette -format: 0xRRGGBB */
/*
static const uint32_t palette[64] = {
	0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
	0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
	0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
	0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
	0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
	0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
	0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
	0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000
};
*/

// Mesen Palette
static const uint32_t palette[0x40] = { 0xFF666666, 0xFF002A88, 0xFF1412A7, 0xFF3B00A4, 0xFF5C007E, 0xFF6E0040, 0xFF6C0600, 0xFF561D00, 0xFF333500, 0xFF0B4800, 0xFF005200, 0xFF004F08, 0xFF00404D, 0xFF000000, 0xFF000000, 0xFF000000, 0xFFADADAD, 0xFF155FD9, 0xFF4240FF, 0xFF7527FE, 0xFFA01ACC, 0xFFB71E7B, 0xFFB53120, 0xFF994E00, 0xFF6B6D00, 0xFF388700, 0xFF0C9300, 0xFF008F32, 0xFF007C8D, 0xFF000000, 0xFF000000, 0xFF000000, 0xFFFFFEFF, 0xFF64B0FF, 0xFF9290FF, 0xFFC676FF, 0xFFF36AFF, 0xFFFE6ECC, 0xFFFE8170, 0xFFEA9E22, 0xFFBCBE00, 0xFF88D800, 0xFF5CE430, 0xFF45E082, 0xFF48CDDE, 0xFF4F4F4F, 0xFF000000, 0xFF000000, 0xFFFFFEFF, 0xFFC0DFFF, 0xFFD3D2FF, 0xFFE8C8FF, 0xFFFBC2FF, 0xFFFEC4EA, 0xFFFECCC5, 0xFFF7D8A5, 0xFFE4E594, 0xFFCFEF96, 0xFFBDF4AB, 0xFFB3F3CC, 0xFFB5EBF2, 0xFFB8B8B8, 0xFF000000, 0xFF000000 };

/*
static const RGB ppu_color_pallete[64][3] = {
	{0x7c, 0x7c, 0x7c},{0x00, 0x00, 0xfc},{0x00, 0x00, 0xbc},{0x44, 0x28, 0xbc},
	{0x94, 0x00, 0x84},{0xa8, 0x00, 0x20},{0xa8, 0x10, 0x00},{0x88, 0x14, 0x00},
	{0x50, 0x30, 0x00},{0x00, 0x78, 0x00},{0x00, 0x68, 0x00},{0x00, 0x58, 0x00},
	{0x00, 0x40, 0x58},{0x00, 0x00, 0x00},{0x00, 0x00, 0x00},{0x00, 0x00, 0x00},
	{0xbc, 0xbc, 0xbc},{0x00, 0x78, 0xf8},{0x00, 0x58, 0xf8},{0x68, 0x44, 0xfc},
	{0xd8, 0x00, 0xcc},{0xe4, 0x00, 0x58},{0xf8, 0x38, 0x00},{0xe4, 0x5c, 0x10},
	{0xac, 0x7c, 0x00},{0x00, 0xb8, 0x00},{0x00, 0xa8, 0x00},{0x00, 0xa8, 0x44},
	{0x00, 0x88, 0x88},{0x00, 0x00, 0x00},{0x00, 0x00, 0x00},{0x00, 0x00, 0x00},
	{0xf8, 0xf8, 0xf8},{0x3c, 0xbc, 0xfc},{0x68, 0x88, 0xfc},{0x98, 0x78, 0xf8},
	{0xf8, 0x78, 0xf8},{0xf8, 0x58, 0x98},{0xf8, 0x78, 0x58},{0xfc, 0xa0, 0x44},
	{0xf8, 0xb8, 0x00},{0xb8, 0xf8, 0x18},{0x58, 0xd8, 0x54},{0x58, 0xf8, 0x98},
	{0x00, 0xe8, 0xd8},{0x78, 0x78, 0x78},{0x00, 0x00, 0x00},{0x00, 0x00, 0x00},
	{0xfc, 0xfc, 0xfc},{0xa4, 0xe4, 0xfc},{0xb8, 0xb8, 0xf8},{0xd8, 0xb8, 0xf8},
	{0xf8, 0xb8, 0xf8},{0xf8, 0xa4, 0xc0},{0xf0, 0xd0, 0xb0},{0xfc, 0xe0, 0xa8},
	{0xf8, 0xd8, 0x78},{0xd8, 0xf8, 0x78},{0xb8, 0xf8, 0xb8},{0xb8, 0xf8, 0xd8},
	{0x00, 0xfc, 0xfc},{0xf8, 0xd8, 0xf8},{0x00, 0x00, 0x00},{0x00, 0x00, 0x00}
};
*/

PPU_Struct *ppu_init()
{
	PPU_Struct* ppu = malloc(sizeof(PPU_Struct));
	ppu->PPU_CTRL = 0x00;
	ppu->PPU_MASK = 0x00;
	ppu->PPU_MASK = 0x00;
	ppu->OAM_ADDR = 0x00;;
	ppu->PPU_STATUS = 0x00; // Had it on A0 before

	ppu->RESET_1 = false;
	ppu->RESET_2 = false;
	ppu->cycle = 0;
	ppu->cycle = 30; // Used to match Mesen traces
	ppu->scanline = 0; // Mesen starts @ 0, previously mine = 240

	/* Set PPU Latches and shift reg to 0 */
	ppu->pt_lo_shift_reg = 0;
	ppu->pt_hi_shift_reg = 0;

	/* NTSC */
	ppu->nmi_start = 241;
	return ppu;
}

// Reset/Warm-up function, clears and sets VBL flag at certain CPU cycles
void ppu_reset(int start, PPU_Struct *p)
{
	if (start && !p->RESET_1 && !p->RESET_2) {
		p->PPU_STATUS &= ~(0x80);  // clear VBL flag if set
		p->RESET_1 = true;
	} else if (p->RESET_1 && (NES->Cycle >= 27383)) {
		p->PPU_STATUS |= 0x80;
		p->RESET_1 = false;
		p->RESET_2 = true;
	} else if (p->RESET_2 && (NES->Cycle >= 57164)) {
		p->PPU_STATUS |= 0x80;
		p->RESET_2 = true;
	}
}


uint8_t read_PPU_Reg(uint16_t addr, PPU_Struct *p)
{
	switch (addr) {
	case (0x2002):
		/* PPU STATUS */
		read_2002(p);
		break;
	case (0x2004):
		/* OAM Data (read & write) */
		return p->OAM_DATA;
		break;
	case (0x2007):
		/* PPU DATA */
		return p->PPU_DATA;
		break;
	}
}

/* CPU uses this function */
void write_PPU_Reg(uint16_t addr, uint8_t data, PPU_Struct *p)
{
	switch (addr) {
	case (0x2000):
		/* PPU CTRL */
		p->PPU_CTRL = data;
		write_2000(data, p);
		break;
	case (0x2001):
		/* PPU MASK */
		p->PPU_MASK = data;
		break;
	case (0x2003):
		/* OAM ADDR */
		p->OAM_ADDR = data;
		break;
	case (0x2004):
		/* OAM Data (read & write) */
		p->OAM_DATA = data;
		break;
	case (0x2005):
		/* PPU SCROLL (write * 2) */
		write_2005(data, p);
		break;
	case (0x2006):
		/* PPU ADDR (write * 2) */
		write_2006(data, p);
		break;
	case (0x2007):
		/* PPU DATA */
		p->PPU_DATA = data;
		write_2007(data, p);
		break;
	}
}

void write_vram(uint8_t data, PPU_Struct *p)
{
	if (p->mirroring == 0) {
		// Horiz mirroring
		if (p->vram_addr >= 0x2000 && p->vram_addr < 0x2400) {
			p->VRAM[p->vram_addr] = data;
			p->VRAM[p->vram_addr + 0x0400] = data;
		} else if (p->vram_addr >= 0x2400 && p->vram_addr < 0x2800) {
			p->VRAM[p->vram_addr] = data;
			p->VRAM[p->vram_addr - 0x0400] = data;
		} else if (p->vram_addr >= 0x2800 && p->vram_addr < 0x2C00) {
			p->VRAM[p->vram_addr] = data;
			p->VRAM[p->vram_addr + 0x0400] = data;
		} else if (p->vram_addr >= 0x2C00 && p->cycle < 0x3000) {
			p->VRAM[p->vram_addr] = data;
			p->VRAM[p->vram_addr - 0x0400] = data;
		}
	} else if (p->mirroring == 1) {
		// Vertical mirroring
		if (p->vram_addr >= 0x2000 && p->vram_addr < 0x2800) {
			p->VRAM[p->vram_addr] = data;
			p->VRAM[p->vram_addr + 0x0800] = data;
		} else if (p->vram_addr >= 0x2800 && p->vram_addr < 0x2C00) {
			p->VRAM[p->vram_addr] = data;
			p->VRAM[p->vram_addr - 0x0800] = data;
		}
	} else if (p->mirroring == 4) {
		// 4 Screen
		p->VRAM[p->vram_addr] = data; // Do nothing
	}

	/* Write to palettes */
	if (p->vram_addr >= 0x3F00) {
		p->VRAM[p->vram_addr] = data;
	}
}

/* Read Functions */

uint8_t read_2002(PPU_Struct *p)
{
	p->temp = p->PPU_STATUS;
	p->PPU_STATUS &= ~(0x80);
	p->toggle_w = false; // Clear latch used by PPUSCROLL & PPUADDR
	return p->temp;
}

/* Write Functions */
void write_2000(uint8_t data, PPU_Struct *p)
{
	p->vram_tmp_addr &= ~(0x0C00); /* Clear bits to be set */
	p->vram_tmp_addr |= (data & 0x03) << 10;
}

inline void write_2003(uint8_t data, PPU_Struct *p)
{
	p->OAM_ADDR = data;
}

void write_2004(uint8_t data, PPU_Struct *p)
{
	// write_OAM
	p->OAM[p->OAM_ADDR] = data;
	++p->OAM_ADDR;
}

void write_2005(uint8_t data, PPU_Struct *p)
{
	// Valid address = 0x0000 to 0x3FFF
	if (!p->toggle_w) {
		// First Write
		p->vram_tmp_addr &= ~0x001F; /* Clear bits that are to be set */
		p->vram_tmp_addr |= (data >> 3);
		p->fineX = data & 0x07; /* same as data % 8 */
	} else {
		// Second Write
		p->vram_tmp_addr &= ~0x73E0; /* Clear bits that are to be set */
		p->vram_tmp_addr |= ((data & 248) << 5) | ((data & 7) << 12);
		p->PPU_SCROLL = p->vram_tmp_addr;
	}
	p->toggle_w = !p->toggle_w;
}


void write_2006(uint8_t data, PPU_Struct *p)
{
	// Valid address = 0x0000 to 0x3FFF
	if (!p->toggle_w) {
		p->vram_tmp_addr &= ~(0x7F00); /* Clear Higher Byte */
		p->vram_tmp_addr |= (uint16_t) ((data & 0x3F) << 8); /* 14th bit should be clear */
	} else {
		p->vram_tmp_addr &= ~(0x00FF); /* Clear Lower Byte */
		p->vram_tmp_addr |= data; /* Lower byte */
		p->vram_addr = p->vram_tmp_addr;
		p->PPU_ADDR = p->vram_tmp_addr;
	}
	p->toggle_w = !p->toggle_w;
}


void write_2007(uint8_t data, PPU_Struct *p)
{
	//p->VRAM[p->vram_addr] = data; // Think that is correct
	write_vram(data, p);
	p->vram_addr += ppu_vram_addr_inc(p);
}


/* FUNCTION FOR PPU MEM 
uint8_t read_PPU(uint16_t addr, PPU_Struct *p)
{
	if (addr < 0x1000) {
		return p->PPU->MEM[addr];
	}  / else if ... /
	else {
		return p->PPU->MEM[addr];
	}
}
*/

/**
 * PPU_CTRL
 */
uint8_t ppu_vram_addr_inc(PPU_Struct *p)
{
	if ((p->PPU_CTRL & 0x04) == 0) {
		return 1;
	} else {
		return 32;
	}
}

uint16_t ppu_base_nt_address(PPU_Struct *p)
{
	switch(p->PPU_CTRL & 0x03) {
	case 0:
		return 0x2000;
	case 1:
		return 0x2400;
	case 2:
		return 0x2800;
	case 3:
		return 0x2C00;
	default:
		return 0x2000;
	}
}


uint16_t ppu_base_pt_address(PPU_Struct *p)
{
	switch((p->PPU_CTRL >> 4) & 0x01) {
	case 0:
		return 0x0000;
	case 1:
		return 0x1000;
	}
}

/* 
 * Helper Functions
 */

void fetch_nt_byte(PPU_Struct *p)
{
	uint16_t nt_offset; // Calculates which pixel we are rendering and +1 to every 8 pixels
	if (p->cycle >= 321) {
		nt_offset = ((p->cycle + 16 - 314) /8); // Calculates which pixel we are rendering and +1 to every 8 pixels
	} else {
		nt_offset = (p->scanline /8)*32 + ((p->cycle + 16) /8); // Calculates which pixel we are rendering and +1 to every 8 pixels
	}
	p->nt_byte = p->VRAM[ppu_base_nt_address(p) + nt_offset];
	p->nt_addr_tmp = ppu_base_nt_address(p) + nt_offset;
	//p->nt_addr_tmp = 0x2000 | (p->vram_addr & 0x0FFF);
}

/* Determines colour palette */
void fetch_at_byte(PPU_Struct *p)
{
	uint16_t at_addr = 0x23C0;
	uint16_t at_offset; // + 8 for every 32 scanlines and + 1 for every 32 horiz pixel (cycle)
	if (p->cycle >= 321) {
		at_offset = (p->cycle - 314)/32;
	} else {
		at_offset = (p->cycle/32) + ((p->scanline/32)*8); // + 8 for every 32 scanlines and + 1 for every 32 horiz pixel (cycle)
	}
	p->at_latch = p->VRAM[at_addr + at_offset];
	//printf("   AT ADDR %X  ", at_addr + at_offset);
	//p->at_latch = p->VRAM[0x23C0 | (p->vram_addr & 0x0C00) | ((p->vram_addr >> 4) & 0x38) | ((p->vram_addr >> 2) & 0x07)];
}

/* Lo & Hi determine which index of the colour palette we use (0 to 3) */
void fetch_pt_lo(PPU_Struct *p)
{
	uint16_t pt_offset = (p->nt_byte << 4) + (p->scanline & 7);
	uint8_t latch = p->VRAM[ppu_base_pt_address(p) + pt_offset];
	p->pt_lo_latch = reverse_bits[latch]; // 8th bit = 1st pixel to render
	//printf("PT ADDR %X", ppu_base_pt_address(p) + pt_offset);
}


void fetch_pt_hi(PPU_Struct *p)
{
	uint16_t pt_offset = (p->nt_byte << 4) + (p->scanline & 7) + 8;
	//p->pt_hi_latch = p->VRAM[ppu_base_pt_address(p) + pt_offset];
	uint8_t latch = p->VRAM[ppu_base_pt_address(p) + pt_offset];
	p->pt_hi_latch = reverse_bits[latch]; // 8th bit = 1st pixel to render
}

/*
void shift_registers(PPU_Struct *p)
{
	p->pt_hi_shift_reg >>= 1;
	p->pt_lo_shift_reg >>= 1;
}
*/

void render_pixel(PPU_Struct *p)
{

	unsigned palette_addr;
	/* - Defines the Colour palette */
	printf("    NT ADDRESS: %.4X  ", p->nt_addr_current);
	printf("    AT ADDR %X  ", 0x23C0 + ((p->cycle >= 321) ? (p->cycle/32) : ((p->cycle/32) + ((p->scanline/32)*8))));
	if (p->nt_addr_current & 0x01) {
		// Right
		if (p->nt_addr_current & 0x20) {
			// Bottom 
			palette_addr = p->at_current >> 6;
		} else {
			// Top
			palette_addr = p->at_current >> 2;
		}
	} else { // Left quad
		if (p->nt_addr_current & 0x20) { // Bottom
			palette_addr = p->at_current >> 4;
		} else {
			// Top
			palette_addr = p->at_current & 0x0003;
		}
	}
	palette_addr &= 0x03;
	palette_addr <<= 2;

	palette_addr += 0x3F00; // Palette mem starts here
	// Palette address is stuck at 3F00
	// SEE CYC:114 on scanline 249, pal address should equal 3F04 not 3f00
	// LINE: 161108
	// AT BYTE (CURRENT IS 0 at the wrong times from NAMETABLE 2220 onwards AtByte = 55 or AA)
	printf("PAL_ADDR: %X", palette_addr);

	/* 0 - 4 offset for Palette memory - defines the colour */
	// @ CYC 25 palette offset should equal 1 but instead equals 3
	// Concat formula is correct thus error must be in loading the shift registers
	unsigned palette_offset = ((p->pt_hi_shift_reg & 0x01) << 1) | (p->pt_lo_shift_reg & 0x01);
	printf("   PAL: %X", palette_offset + palette_addr);
	printf("   AT BYTE %X", p->at_latch);
	printf("   HIGH: %d, LOW: %d", ((p->pt_hi_shift_reg & 0x01) << 1), (p->pt_lo_shift_reg & 0x01));

	unsigned RGB = p->VRAM[palette_addr + palette_offset]; // Get values

	/* Reverse Bits */
	//printf("A: %d\n", (p->cycle + (256 * p->scanline) - 1)); // Place in palette array, alpha set to 0xFF
	pixels[(p->cycle + (256 * p->scanline) - 1)] = 0xFF000000 | palette[RGB]; // Place in palette array, alpha set to 0xFF
	printf("   PALETTE = %X", palette[RGB]);
	printf("   NT BYTE %X", p->nt_byte);
	printf("   CYC = %d", p->cycle);
	printf("   SL = %d\n", p->scanline);

	/* Shift each cycle */
	p->pt_hi_shift_reg >>= 1;
	p->pt_lo_shift_reg >>= 1;
}

/*************************
 * RENDERING             *
 *************************/

void ppu_tick(PPU_Struct *p)
{
	p->cycle++;
	if (p->cycle > 340) {
		p->cycle = 0; /* Reset cycle count to 0, max val = 340 */

		p->scanline++;
		if (p->scanline > 261) {
			p->scanline = 0; /* Reset scanline to 0, max val == 261 */
		}
	}
}

void ppu_step(PPU_Struct *p, CPU_6502* NESCPU)
{
//#ifdef __RESET__
	ppu_reset(1, p);
//#endif

	ppu_tick(p); // Idle cycle thus can run tick to increment cycle from 0 to 1 initially

	/* NMI Calc */
	if (p->scanline == p->nmi_start) {
		if ((p->PPU_CTRL & 0x80) == 0x80) { /* if PPU CTRL has execute NMI on VBlank */
			p->PPU_STATUS |= 0x80; /* In VBlank */
			if ((p->cycle - 3) == 7) { // 7 --> delay by One PPU Cycle
				NESCPU->NMI_PENDING = 1;
			}
		}
		return;
	}  else if (p->scanline == 261) { /* Pre-render scanline */
		p->PPU_STATUS &= ~(0x80);
	}

	/* Process scanlines */
	if (p->scanline <= 239) { /* Visible scanlines */
		if (p->cycle <= 256 && (p->cycle != 0)) { // 0 is an idle cycle
			// write to pixel buffer each cycle
			render_pixel(p); // Render pixel every cycle
			switch ((p->cycle - 1) & 0x07) {
			case 0:
				fetch_nt_byte(p);
				break;
			case 2:
				fetch_at_byte(p);
				break;
			case 4:
				fetch_pt_lo(p);
				break;
			case 6:
				fetch_pt_hi(p);
				break;
			case 7: /* 8th Cycle */
				// 8 Shifts should have occured by now, load new data
				p->at_current = p->at_next;
				p->at_next = p->at_latch;
				/* Load latched values into upper byte of shift regs */
				p->pt_lo_shift_reg |= (uint16_t) (p->pt_lo_latch << 8);
				p->pt_hi_shift_reg |= (uint16_t) (p->pt_hi_latch << 8);
				/* Used for palette calculations */
				p->nt_addr_current = p->nt_addr_next;
				p->nt_addr_next = p->nt_addr_tmp;
				break;
			}
		} else if (p->cycle >= 321 && p->cycle <= 336) { // 1st 16 pixels of next scanline
			switch ((p->cycle - 1) & 0x07) {
			case 0:
				fetch_nt_byte(p);
				break;
			case 2:
				fetch_at_byte(p);
				break;
			case 4:
				fetch_pt_lo(p);
				break;
			case 6:
				fetch_pt_hi(p);
				/* Load latched values into upper byte of shift regs */
				p->pt_hi_shift_reg >>= 8; // Think i need this to set up the shift reg properly
				p->pt_lo_shift_reg >>= 8; // It fills the first 16 pixels properly
				p->pt_hi_shift_reg |= (uint16_t) (p->pt_hi_latch << 8);
				p->pt_lo_shift_reg |= (uint16_t) (p->pt_lo_latch << 8);
				p->at_current = p->at_next; // Current is 1st loaded w/ garbage
				p->at_next = p->at_latch;
				p->nt_addr_current = p->nt_addr_next;
				p->nt_addr_next = p->nt_addr_tmp;
				break;
			}
		}
	} else if (p->scanline == 240) {
		draw_pixels(pixels, nes_screen); // Render frame
	}
}
