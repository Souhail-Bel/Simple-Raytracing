#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"

class ITexture {
	public:
		virtual ~ITexture() = default;
		
		virtual color value(const double u, const double v, const point3& p) const = 0;
};

#endif