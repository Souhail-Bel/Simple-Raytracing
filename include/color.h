#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

#include "vec3.h"

using color = vec3;

// ARGB8888
inline uint32_t get_color(const color& pixel_color){
	// Mapping 1.0 to 255
	uint8_t R = pixel_color.x()*255;
	uint8_t G = pixel_color.y()*255;
	uint8_t B = pixel_color.z()*255;
	return (R << 16) | (G << 8) | B;
}

#endif