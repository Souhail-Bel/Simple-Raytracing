#include <cerrno>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

#include <vector>
#include <cstdint>

using namespace std;

#include "display.h"
#include "defs.h"
#include "bvh.h"
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
	
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "gpu");
	
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
	// cam.refocus();
	
	// Optimized approach
	// using Lock/Unlock texture on GPU
	
	void* texture_pixels;
	int pitch;
	
	SDL_LockTexture(g_texture, NULL, &texture_pixels, &pitch);
	
	// Per pixel (32-bit) manipulation
	// ARGB8888 format	
	memcpy(texture_pixels, cam.display_buffer.data(), cam.display_buffer_size);
	
	SDL_UnlockTexture(g_texture);
	
	SDL_RenderClear(g_renderer);
	
	SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
	SDL_RenderPresent(g_renderer);
}
#endif

void scene_origScene(void) {
	
	auto checker = make_shared<Checker_Texture>(.3, color(.1), color(.9));
	
	auto mat_center = make_shared<Metal>(color(.3), 0);
	auto mat_ground = make_shared<Lambertian>(checker);
	auto mat_sphsky = make_shared<Metal>(color(.9, .3, .3), .3);
	auto mat_spher1 = make_shared<Metal>(color(.7, .6, .2), .7);
	auto mat_spher2 = make_shared<Lambertian>(color(.2, .6, .7));
	auto mat_spher3 = make_shared<Dielectric>(1./1.33);
	auto mat_spher4 = make_shared<Dielectric>(2.5);
	
	
	scene.add(make_shared<Sphere>(point3(0,0,-.5),	.25, mat_center));
	scene.add(make_shared<Sphere>(point3(10,10,-20), 10, mat_sphsky));
	scene.add(make_shared<Sphere>(point3(0,-30.5,-1),30, mat_ground));
	scene.add(make_shared<Sphere>(point3(-10,5,-10),  3, mat_spher1));
	scene.add(make_shared<Sphere>(point3(-10,5,-2), point3(-10,2,-2), 5, mat_spher2));
	scene.add(make_shared<Sphere>(point3(-10,2,-10),  4, mat_spher3));
	scene.add(make_shared<Sphere>(point3(5,2,-10),    4, mat_spher4));
}

void scene_bookScene(void) {
	// Scene from Raytracing in One Weekend
	
	auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
    scene.add(make_shared<Sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -5; a < 5; a++) {
        for (int b = -5; b < 5; b++) {
            auto choose_mat = get_rand_double();
            point3 center(a + 0.9*get_rand_double(), 0.2, b + 0.9*get_rand_double());

            if ((center - point3(4, 0.2, 0)).len() > 0.9) {
                shared_ptr<IMaterial> sphere_material;

                if (choose_mat < 0.8) {
                    // Diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<Lambertian>(albedo);
					auto center2 = center + vec3(0,get_rand_double(0, 1),0);
                    scene.add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // Metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = get_rand_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // Glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    scene.add(make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
    scene.add(make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
    scene.add(make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));
	
	
}

void setup_SCENE(void){
	
	scene_origScene();
	
	scene = hittable_list(make_shared<BVH_node>(scene));
	
	cam = Camera(scene);
	
	cam.eye_point = point3(3,2,5);
	cam.foc_point = point3(0,0,0);
	cam.camera_up = vec3(0,1,0);
	
	cam.FOV = 100;
	
	cam.init_CAMERA(WIDTH, HEIGHT);
	#ifdef SAMPLING_MODE
		cam.samples_per_pixel = 10;
	#endif
	cam.max_bounces = 50;
	
	
	return;
}
