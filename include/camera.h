#ifndef CAMERA_H
#define CAMERA_H

#include "defs.h"

class Camera {
	private:
		hittable_list world;
		
		int WIN_WIDTH;
		int WIN_HEIGHT;
		int WIN_SIZE;
		
		float VIEWPORT_HEIGHT = 2.;
		float VIEWPORT_WIDTH;
		
		point3 eye_point = point3(0, 0, 0);
		
		// Distance between viewport and eye point
		// Set to 1.9248599884 for a very nice FOV
		// Currently set at FOV 120°
		float focal_length = 0.7698003589195;
		
		vec3 pixel_delta_h;
		vec3 pixel_delta_v;
		
		point3 pixel_00;
		
		uint32_t default_pixel = 0xFFU << 24;
		
	public:
		uint32_t* display_buffer = NULL;
		int display_buffer_size;
		
		Camera() {}
		
		Camera(const hittable_list& scene) : world(scene) {}
		
		~Camera(){
			if(display_buffer)
				free(display_buffer);
		}
		
		// init function
		void init_CAMERA(const int WIDTH, const int HEIGHT) {
			
			WIN_WIDTH = WIDTH;
			WIN_HEIGHT = HEIGHT;
			WIN_SIZE = WIN_HEIGHT * WIN_WIDTH;
			
			float ASPECT_RATIO = (1. * WIN_WIDTH)/WIN_HEIGHT;
			VIEWPORT_WIDTH = ASPECT_RATIO * VIEWPORT_HEIGHT;
			
			display_buffer_size = sizeof(uint32_t) * WIN_SIZE;
			display_buffer = (uint32_t*) malloc(display_buffer_size);
			
			
			// Viewport pixel grid set up
			// Note how the y-axis is inverted
			vec3 viewport_h = vec3(VIEWPORT_WIDTH, 0, 0);
			vec3 viewport_v = vec3(0, -VIEWPORT_HEIGHT, 0);

			// Pixel-to-pixel delta vectors
			pixel_delta_h = viewport_h / WIN_WIDTH;
			pixel_delta_v = viewport_v / WIN_HEIGHT;

			// Find (0, 0) of the viewport (upper-left)
			point3 viewport_00 = eye_point - vec3(0, 0, focal_length)
							- viewport_h/2 - viewport_v/2;
			pixel_00 = viewport_00 + 0.5 * (pixel_delta_h + pixel_delta_v);
		}
		
		// get color func
		color ray_color(const ray& r) const;
		
		// compute frame func
		void compute_FRAME(void) const;
};


color Camera::ray_color(const ray& r) const {
	hit_record rec;
	if(world.hit(r, interval::positive, rec))
		return .5 * (rec.normal + color(1));
	
	
	// BG
	vec3 dir = normalized(r.direction());
	float a = 0.5*(dir.y() + 1.);
	return (1-a) * color(1) + a * color(.4, .6, 1);
}


void Camera::compute_FRAME(void) const {
	
	memset(display_buffer, default_pixel, display_buffer_size);
	
	for(int y = 0; y < WIN_HEIGHT; y++) {
		for(int x = 0; x < WIN_WIDTH; x++){
			point3 pixel_center = pixel_00
								+ x * pixel_delta_h
								+ y * pixel_delta_v;
			
			vec3 ray_dir = pixel_center - eye_point;
			
			ray r(eye_point, ray_dir);
			
			color pixel_color = ray_color(r);
			
			display_buffer[y * WIN_WIDTH + x] |= get_color(pixel_color);
		}
	}
}

#endif