#ifndef UTILS_H
#define UTILS_H

#include <limits>
#include <cstdint>
#include <random>

	
// Get random double in [0,1[
inline double get_rand_double(void) {
	thread_local static std::mt19937 gen = []{
        std::random_device rd;
        return std::mt19937(rd());
    }();

	thread_local static std::uniform_real_distribution<> dis(0.0, 1.0); 
	return dis(gen);
}

// Get random double in [min,max[
inline double get_rand_double(double min, double max) {
	return min + (max - min)*get_rand_double();
}

// Get random int in [min, max];
inline int get_rand_int(int min, int max) {
	return int(get_rand_double(min, max+1));
}


constexpr double inf = std::numeric_limits<double>::infinity();
constexpr float PI = 3.1415926535897;
constexpr float TWO_PI = 2*PI;

inline double degrees_to_radians(double deg) {
	return deg * (PI/180);
}

#include "utils/vec3.h"
#include "utils/color.h"
#include "utils/ray.h"
#include "utils/interval.h"

#endif