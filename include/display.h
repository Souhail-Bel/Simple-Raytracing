#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

#include <vector>
#include <cstdint>

using namespace std;

// Window dimensions
const int WIDTH  		= 640;
const int HEIGHT 		= 480;
constexpr int WIN_SIZE 	= WIDTH*HEIGHT;

const uint32_t default_pixel = 0xFFU << 24;

// Global SDL elements
extern SDL_Window* 		g_window;
extern SDL_Renderer* 	g_renderer;
extern SDL_Texture*		g_texture; // Frame buffer
extern SDL_Event 		g_event;

extern bool is_running;

// Rendering elements
extern uint32_t display_buffer[HEIGHT*WIDTH];
// extern vector<uint8_t> CANAL_R;
// extern vector<uint8_t> CANAL_G;
// extern vector<uint8_t> CANAL_B;

constexpr int get_idx(int x, int y);
void init_SDL(void);
void close_SDL(void);
void handle_INPUT(void);
void update_RENDER(void);
void compute_FRAME(void);

#endif