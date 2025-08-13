#include <cerrno>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

#include <vector>
#include <cstdint>

using namespace std;

#include "utils.h"

#include "display.h"


// Viewport pixel grid set up
// Note how the y-axis is inverted
vec3 viewport_h = vec3(Camera.VIEWPORT_WIDTH, 0, 0);
vec3 viewport_v = vec3(0, -Camera.VIEWPORT_HEIGHT, 0);

// Pixel-to-pixel delta vectors
vec3 pixel_delta_h = viewport_h / WIDTH;
vec3 pixel_delta_v = viewport_v / HEIGHT;

// Find (0, 0) of the viewport (upper-left)
point3 viewport_00 = Camera.eye_point - vec3(0, 0, Camera.focal_length)
				- viewport_h/2 - viewport_v/2;
point3 pixel_00 = viewport_00 + 0.5 * (pixel_delta_h + pixel_delta_v);


// Scene parameters
hittable_list scene;



// SDL-specific
// #if is used for region
#if 1
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

void setup_SCENE(void){
	scene.add(make_shared<Sphere>(point3(0,0,-1), .5));
	scene.add(make_shared<Sphere>(point3(10,10,-20), 10));
	scene.add(make_shared<Sphere>(point3(10,-10,-20), 50));
	return;
}


color ray_color(const ray& r) {
	hit_record rec;
	if(scene.hit(r, interval::positive, rec))
		return .5 * (rec.normal + color(1));
	
	
	// BG
	vec3 dir = normalized(r.direction());
	float a = 0.5*(dir.y() + 1.);
	return (1-a) * color(1) + a * color(.4, .6, 1);
}


void compute_FRAME(void){
	
	memset(display_buffer, default_pixel, sizeof(display_buffer));
	
	for(int y = 0; y < HEIGHT; y++) {
		for(int x = 0; x < WIDTH; x++){
			point3 pixel_center = pixel_00
								+ x * pixel_delta_h
								+ y * pixel_delta_v;
			
			vec3 ray_dir = pixel_center - Camera.eye_point;
			
			ray r(Camera.eye_point, ray_dir);
			
			color pixel_color = ray_color(r);
			
			display_buffer[get_idx(x,y)] |= get_color(pixel_color);
		}
	}
}