#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"
#include <memory>
#include <omp.h>

class Camera {
	private:
		hittable_list world;
		
		int WIN_WIDTH;
		int WIN_HEIGHT;
		int WIN_SIZE;
		
		float VIEWPORT_HEIGHT = 2.;
		float VIEWPORT_WIDTH;
		
		vec3 u, v, w;
		
		// Distance between viewport and eye point
		// Set to 1.9248599884 for a very nice FOV
		// Currently set at FOV 120°
		float focal_length;
		
		vec3 pixel_delta_h;
		vec3 pixel_delta_v;
		
		point3 pixel_00;
		
		constexpr static uint32_t default_pixel = 0xFFU << 24;
		
		#ifdef SAMPLING_MODE	
			float pixel_samples_scale;
		#endif
		
		
		// get color func
		color ray_color(const ray& r, int bounces) const;
		
		ray get_ray(int x, int y) const;
		
		
	public:
		std::vector<uint32_t> display_buffer;
		int display_buffer_size;
		#ifdef SAMPLING_MODE
			int samples_per_pixel = 5;
		#endif
		int max_bounces = 10;
		
		point3 eye_point = point3(0, 0, 0);
		point3 foc_point = point3(0, 0, -1);
		vec3   camera_up = vec3(0, 1, 0);
		
		float FOV = 120; // In degrees
		
		
		Camera() {}
		
		Camera(const hittable_list& scene) : world(scene) {}
		
		
		// init function
		void init_CAMERA(const int WIDTH, const int HEIGHT) {
			
			WIN_WIDTH = WIDTH;
			WIN_HEIGHT = HEIGHT;
			WIN_SIZE = WIN_HEIGHT * WIN_WIDTH;
			
			float ASPECT_RATIO = (1. * WIN_WIDTH)/WIN_HEIGHT;
			VIEWPORT_WIDTH = ASPECT_RATIO * VIEWPORT_HEIGHT;
			
			#ifdef SAMPLING_MODE
				pixel_samples_scale = 1. / samples_per_pixel;
			#endif
			
			display_buffer_size = sizeof(uint32_t) * WIN_SIZE;
			display_buffer.resize(WIN_SIZE);
			
			focal_length = VIEWPORT_WIDTH/(2*std::tan(degrees_to_radians(FOV)/2));
			
			w = normalized(eye_point - foc_point);
			u = normalized(cross(camera_up, w));
			v = cross(w, u);
			
			// Viewport pixel grid set up
			// Note how the y-axis is inverted
			vec3 viewport_h = VIEWPORT_WIDTH * u;
			vec3 viewport_v = VIEWPORT_HEIGHT * -v;

			// Pixel-to-pixel delta vectors
			pixel_delta_h = viewport_h / WIN_WIDTH;
			pixel_delta_v = viewport_v / WIN_HEIGHT;

						
			// Find (0, 0) of the viewport (upper-left)
			vec3 offset_00 = (focal_length * w)
							+ viewport_h/2 + viewport_v/2;
			point3 viewport_00 = eye_point - offset_00;
			pixel_00 = viewport_00 + 0.5 * (pixel_delta_h + pixel_delta_v);
		}
		
		// compute frame func
		void compute_FRAME(void);
		
		void refocus(void) {
			w = normalized(eye_point - foc_point);
			u = normalized(cross(camera_up, w));
			v = cross(w, u);
			
			// Viewport pixel grid set up
			// Note how the y-axis is inverted
			vec3 viewport_h = VIEWPORT_WIDTH * u;
			vec3 viewport_v = VIEWPORT_HEIGHT * -v;

			// Pixel-to-pixel delta vectors
			pixel_delta_h = viewport_h / WIN_WIDTH;
			pixel_delta_v = viewport_v / WIN_HEIGHT;

						
			// Find (0, 0) of the viewport (upper-left)
			vec3 offset_00 = (focal_length * w)
							+ viewport_h/2 + viewport_v/2;
			point3 viewport_00 = eye_point - offset_00;
			pixel_00 = viewport_00 + 0.5 * (pixel_delta_h + pixel_delta_v);
		}
		
		void ascend(void) {
			static vec3 deltaU = vec3(0, 0.01, 0);
			static vec3 deltaV = vec3(1);
			
			camera_up += deltaV;
			eye_point += deltaU;
		}
};

ray Camera::get_ray(int x, int y) const {
	// Ray directed to pixel (x, y)
	#ifdef SAMPLING_MODE
		vec3 offset = vec3(get_rand_double()-.5, get_rand_double()-.5, 0);
		
		point3 pixel_center = pixel_00
							+ (x + offset.x()) * pixel_delta_h
							+ (y + offset.y()) * pixel_delta_v;
	#else
		point3 pixel_center = pixel_00
							+ x * pixel_delta_h
							+ y * pixel_delta_v;
	#endif
	
	return ray(eye_point,					// Origin
				pixel_center - eye_point,	// Direction
				get_rand_double());			// Time
}


color Camera::ray_color(const ray& r, int bounces_left) const {
	if (bounces_left <= 0) return color(0);
	
	hit_record rec;
	// Ray bounces off surfaces randomly
	if(world.hit(r, interval(0.001, inf), rec)){
		ray scattered;
		color attenuation;
		
		if(rec.mat -> scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, bounces_left-1);
		return color(0);
		// return .5 * ray_color(ray(rec.p, rand_dir), bounces_left-1);
	}
	
	// BG
	vec3 dir = normalized(r.direction());
	float a = 0.5*(dir.y() + 1.);
	color bg = (1-a) * color(1) + a * color(.4, .6, 1);
	#ifdef SAMPLING_MODE
		bg = .5*bg;
	#endif
	return bg;
}


void Camera::compute_FRAME(void) {
	
	#pragma omp parallel for
	for(int y = 0; y < WIN_HEIGHT; y++) {
		for(int x = 0; x < WIN_WIDTH; x++){
			#ifdef SAMPLING_MODE
				color pixel_color(0);
				for(int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(x, y);
					pixel_color += ray_color(r, max_bounces);
				}
				
				pixel_color *= pixel_samples_scale;
			#else
				ray r = get_ray(x, y);
				color pixel_color = ray_color(r, max_bounces);
			#endif
				
			display_buffer[y * WIN_WIDTH + x] = get_color(pixel_color);
		}
	}
}

#endif