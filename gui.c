#include "gui.h"

const unsigned SCREEN_WIDTH = 256;
const unsigned SCREEN_HEIGHT = 240;


static SDL_Window* window;
//static SDL_Surface* surface;
static SDL_Renderer* renderer;
static SDL_Texture* framebuffer;

int SDL_init()
{
	/*
	uint32_t (*pixels)[256 *240] = malloc(SCREEN_WIDTH * SCREEN_HEIGHT* sizeof(uint32_t));
	if (pixels == NULL) {
		fprintf(stderr, "malloc failed\n");
		return -1;
	}
	*/
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("cNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	return 0;
}

/*
void draw_pixels()
{
	for (int i = 0; i < 240; i++) { // Vertical pixels
		for (int j = 0; j < 256; j++) { // Horizontial pixels
			unsigned red = pixels[j][i][0];
			unsigned blue = pixels[j][i][1];
			unsigned green = pixels[j][i][2];
			SDL_SetRenderDrawColor(renderer, red, blue, green, 0xFF); // Alpha @ 255
			SDL_RenderDrawPoint(renderer, j, i);
		}
	}
	SDL_RenderPresent(renderer);
}
*/

void draw_pixels(uint32_t *pixels)
{
	SDL_UpdateTexture(framebuffer, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
	SDL_RenderPresent(renderer);
}
