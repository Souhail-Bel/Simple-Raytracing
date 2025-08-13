#include "utils.h"



// ARGB8888
uint32_t get_color(const color& pixel_color){
	// Mapping 1.0 to 255
	uint8_t R = pixel_color.x()*255;
	uint8_t G = pixel_color.y()*255;
	uint8_t B = pixel_color.z()*255;
	return (R << 16) | (G << 8) | B;
}


const interval interval::empty = interval();
const interval interval::universe = interval(-inf, +inf);
const interval interval::positive = interval(0, +inf);