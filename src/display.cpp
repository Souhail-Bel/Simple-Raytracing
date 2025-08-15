#include <cerrno>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

#include <vector>
#include <cstdint>

using namespace std;

#include "display.h"

#include "camera.h"

// Scene parameters
hittable_list scene;
Camera cam;


// SDL-specific
// #if is used for region
#if 1
SDL_Window* 	g_window;
SDL_Renderer* 	g_renderer;
SDL_Texture*	g_texture;
SDL_Event 		g_event;

bool is_running;



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
	
	cam.compute_FRAME();
	// cam.ascend();
	
	// Optimized approach
	// using Lock/Unlock texture on GPU
	
	void* texture_pixels;
	int pitch;
	
	SDL_LockTexture(g_texture, NULL, &texture_pixels, &pitch);
	
	// Per pixel (32-bit) manipulation
	// ARGB8888 format	
	memcpy(texture_pixels, cam.display_buffer, cam.display_buffer_size);
	
	SDL_UnlockTexture(g_texture);
	
	SDL_RenderClear(g_renderer);
	
	SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
	SDL_RenderPresent(g_renderer);
}
#endif

void setup_SCENE(void){
	
	auto mat_ground = make_shared<Lambertian>(color(.5, .2, .9));
	auto mat_center = make_shared<Metal>(color(.3));
	auto mat_sphsky = make_shared<Metal>(color(.8));
	auto mat_spher1 = make_shared<Metal>(color(.7, .6, .2));
	auto mat_spher2 = make_shared<Lambertian>(color(.2, .6, .7));
	auto mat_spher3 = make_shared<Lambertian>(color(.1, .5, .1));
	
	
	scene.add(make_shared<Sphere>(point3(0,0,-1),	 .5, mat_center));
	scene.add(make_shared<Sphere>(point3(10,10,-20), 10, mat_sphsky));
	scene.add(make_shared<Sphere>(point3(0,-30.5,-1),30, mat_ground));
	scene.add(make_shared<Sphere>(point3(-10,5,-10),  3, mat_spher1));
	scene.add(make_shared<Sphere>(point3(-10,5,-2),   5, mat_spher2));
	scene.add(make_shared<Sphere>(point3(-10,1,-10),   4, mat_spher3));
	
	// scene.add(make_shared<Sphere>(point3(10,-10,-20), 50));
	
	cam = Camera(scene);
	cam.init_CAMERA(WIDTH, HEIGHT);
	#ifdef SAMPLING_MODE
		cam.samples_per_pixel = 10;
	#endif
	cam.max_bounces = 50;
	
	return;
}
