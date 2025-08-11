#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

#include "vec3.h"

using color = vec3;

// ARGB8888
inline uint32_t get_color(const color& pixel_color){
	uint8_t R = pixel_color.x();
	uint8_t G = pixel_color.y();
	uint8_t B = pixel_color.z();
	return (R << 16) | (G << 8) | B;
}

#endif