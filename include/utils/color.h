#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

#include "utils/vec3.h"

using color = vec3;

// Defined in utils.cpp
uint32_t get_color(const color& pixel_color);

#endif