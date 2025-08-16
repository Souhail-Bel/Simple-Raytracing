#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

#include <vector>
#include <cstdint>

#include "utils.h"
#include "defs.h"

using namespace std;

// Window dimensions
const int WIDTH  		= 640;
const int HEIGHT 		= 480;
constexpr int WIN_SIZE 	= WIDTH*HEIGHT;

// Global SDL elements
extern SDL_Window* 		g_window;
extern SDL_Renderer* 	g_renderer;
extern SDL_Texture*		g_texture; // Frame buffer
extern SDL_Event 		g_event;


void init_SDL(void);
void close_SDL(void);
void handle_INPUT(void);
void update_RENDER(void);

void setup_SCENE(void);

#endif