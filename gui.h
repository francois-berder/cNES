#ifndef __NES_GUI__
#define __NES_GUI__

#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

const unsigned SCREEN_WIDTH;
const unsigned SCREEN_HEIGHT;

//extern uint8_t pixels[SCREEN_WIDTH][SCREEN_HEIGHT][3]; // pixels[256][240][3], [3] --> 0 = red, 1 = blue
//extern uint8_t pixels[256][240][3]; // pixels[256][240][3], [3] --> 0 = red, 1 = blue
//extern uint32_t *pixels; // pixels[256][240][3], [3] --> 0 = red, 1 = blue
uint32_t pixels[256 * 240]; // pixels[256][240][3], [3] --> 0 = red, 1 = blue

int SDL_init(void);
void draw_pixels(uint32_t *pixels); // Draws frame to screen

#endif /* __NES_GUI__ */
