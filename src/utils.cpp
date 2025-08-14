#include "utils.h"

inline double get_rand_double(void) {
	static std::random_device rd;
	static std::mt19937 gen(rd()); 
	static std::uniform_real_distribution<> dis(0.0, 1.0); 
	return dis(gen);
}

inline double get_rand_double(double min, double max) {
	return min + (max - min)*get_rand_double();
}


// ARGB8888
uint32_t get_color(const color& pixel_color){
	static const interval intensity(0., 1.);
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();
	
	// Mapping 1.0 to 255
	uint8_t R_byte = intensity.clamp(r) * 255;
	uint8_t G_byte = intensity.clamp(g) * 255;
	uint8_t B_byte = intensity.clamp(b) * 255;
	return (R_byte << 16) | (G_byte << 8) | B_byte;
}


const interval interval::empty = interval();
const interval interval::universe = interval(-inf, +inf);
const interval interval::positive = interval(0, +inf);
