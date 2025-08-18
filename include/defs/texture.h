#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"

class ITexture {
	public:
		virtual ~ITexture() = default;
		
		virtual color value(const double u, const double v, const point3& p) const = 0;
};

class Uniform_Color : public ITexture {
	private:
		color albedo;
	
	public:
		Uniform_Color(const color& albedo) : albedo(albedo) {}
		
		Uniform_Color(const double r, const double g, const double b) : albedo(color(r, g, b)) {}
		
		color value(const double u, const double v, const point3& p) const override {return albedo;}
};

#endif