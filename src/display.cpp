#include <cerrno>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

#include <vector>
#include <cstdint>

using namespace std;

#include "display.h"
#include "color.h"
#include "ray.h"


// SDL-specific
// #if is used for region
#if
SDL_Window* 	g_window;
SDL_Renderer* 	g_renderer;
SDL_Texture*	g_texture;
SDL_Event 		g_event;

bool is_running;


uint32_t display_buffer[WIN_SIZE] = {default_pixel};
// vector<uint8_t> CANAL_R(WIN_SIZE, 0);
// vector<uint8_t> CANAL_G(WIN_SIZE, 0);
// vector<uint8_t> CANAL_B(WIN_SIZE, 0);


constexpr int get_idx(int x, int y) {
	return y * WIDTH + x;
}


void init_SDL(void){
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
		perror(SDL_GetError());
		exit(-1);
	}
	
	
	g_window = SDL_CreateWindow(
		"Raytracing",
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT,
		SDL_WINDOW_SHOWN
	);	
	
	g_renderer = SDL_CreateRenderer(
		g_window,
		-1,
		SDL_RENDERER_ACCELERATED
	);
	
	g_texture = SDL_CreateTexture(
		g_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		// SDL_PIXELFORMAT_RGB24,
		SDL_TEXTUREACCESS_STREAMING,
		WIDTH,
		HEIGHT
	);
	
	
	if(!g_window || !g_renderer || !g_texture){
		perror(SDL_GetError());
		SDL_Quit();
		exit(-1);
	}
}

void close_SDL(void){
	SDL_DestroyTexture(g_texture);
	SDL_DestroyRenderer(g_renderer);
	SDL_DestroyWindow(g_window);
	
	SDL_Quit();
}


void handle_INPUT(void){
	while(SDL_PollEvent(&g_event)){
		if(g_event.type == SDL_QUIT) is_running = false;
		
		// if(g_event.type == SDL_KEYDOWN){
			// cout << "Key down:" << SDL_GetKeyName(g_event.key.keysym.sym) << endl;
			// cout.flush();
		// }
	}
}


void update_RENDER(void){
	// Optimized approach
	// using Lock/Unlock texture on GPU
	
	void* texture_pixels;
	int pitch;
	
	SDL_LockTexture(g_texture, NULL, &texture_pixels, &pitch);
	
	// Per pixel (32-bit) manipulation
	// ARGB8888 format	
	memcpy(texture_pixels, display_buffer, sizeof(display_buffer));
	
	SDL_UnlockTexture(g_texture);
	
	SDL_RenderClear(g_renderer);
	
	SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
	SDL_RenderPresent(g_renderer);
}
#endif

int t = 0;
void compute_FRAME(void){
	
	memset(display_buffer, default_pixel, sizeof(display_buffer));
	
	for(int y = 0; y < HEIGHT; y++) {
		for(int x = 0; x < WIDTH; x++){
			auto pixel_color = color((256*x)/WIDTH,
									(256*y)/HEIGHT,
									(256*t)/320);
			
			display_buffer[get_idx(x,y)] |= get_color(pixel_color);
		}
	}
	
	t++;
	t %= 320;
}