#include "utils.h"

inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return sqrt(linear_component);

    return 0;
}


// ARGB8888
uint32_t get_color(const color& pixel_color){
	static const interval intensity(0., 1.);
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();
	
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);
	
	// Mapping 1.0 to 255
	uint8_t R_byte = intensity.clamp(r) * 255;
	uint8_t G_byte = intensity.clamp(g) * 255;
	uint8_t B_byte = intensity.clamp(b) * 255;
	return (R_byte << 16) | (G_byte << 8) | B_byte;
}


const interval interval::empty = interval();
const interval interval::universe = interval(-inf, +inf);
const interval interval::positive = interval(0.001, +inf);
