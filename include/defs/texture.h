#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include "utils/img.h"

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

class Checker_Texture : public ITexture {
	private:
		double inv_scale;
		shared_ptr<ITexture> even;
		shared_ptr<ITexture> odd;
	
	public:
		Checker_Texture(double scale, shared_ptr<ITexture> even, shared_ptr<ITexture> odd) : inv_scale(1./scale), even(even), odd(odd) {}
		
		Checker_Texture(double scale, const color& c0, const color& c1) : inv_scale(1./scale), even(make_shared<Uniform_Color>(c0)), odd(make_shared<Uniform_Color>(c1)) {}
		
		// Checker the entire space, for now
		color value(const double u, const double v, const point3& p) const override {
			auto xInt = int(std::floor(inv_scale * p.x()));
			auto yInt = int(std::floor(inv_scale * p.y()));
			auto zInt = int(std::floor(inv_scale * p.z()));
			
			return ((xInt + yInt + zInt) % 2) ? odd->value(u, v, p) : even->value(u, v, p);
		}
};

class Lollipop_Texture : public ITexture {
	private:
		double scale;
		shared_ptr<ITexture> even;
		shared_ptr<ITexture> odd;
	
	public:
		Lollipop_Texture(double scale, shared_ptr<ITexture> even, shared_ptr<ITexture> odd) : scale(scale), even(even), odd(odd) {}
		
		Lollipop_Texture(double scale, const color& c0, const color& c1) : scale(scale), even(make_shared<Uniform_Color>(c0)), odd(make_shared<Uniform_Color>(c1)) {}
		
		// Checker the entire space, for now
		color value(const double u, const double v, const point3& p) const override {
			return (int(scale*(u+v)) % 2) ? odd->value(u, v, p) : even->value(u, v, p);
		}
};

class IMG_Texture : public ITexture {
	private:
		IMG image;
	
	public:
		IMG_Texture(const char* filename) : image(filename) {}
		
		color value(double u, double v, const point3& p) const override {
			if(image.height() <= 0) return color(1, 0, 1);
			
			u = interval::unit.clamp(u);
			v = 1. - interval::unit.clamp(v);
			
			auto i = int(u*image.width());
			auto j = int(v*image.height());
			auto pixel = image.pixel_data(i, j);
			
			auto color_scale = 1. / 255;
			
			return color_scale * color(pixel[0], pixel[1], pixel[2]);
			
		}
};

#endif