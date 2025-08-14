#ifndef UTILS_H
#define UTILS_H

#include <limits>
#include <cstdint>
#include <random>


// Get rand double in [0,1[
inline double get_rand_double(void) {
	static std::random_device rd;
	static std::mt19937 gen(rd()); 
	static std::uniform_real_distribution<> dis(0.0, 1.0); 
	return dis(gen);
}

// Get rand double in [min,max[
inline double get_rand_double(double min, double max) {
	return min + (max - min)*get_rand_double();
}


const double inf = std::numeric_limits<double>::infinity();

#include "utils/vec3.h"
#include "utils/color.h"
#include "utils/ray.h"
#include "utils/interval.h"

#endif