#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class IMaterial {
	public:
		virtual ~IMaterial() = default;
		
		virtual bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class Lambertian : public IMaterial {
	private:
		color albedo;
		
	public:
		Lambertian(const color& albedo) : albedo(albedo) {}
		
		bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
			vec3 scatter_dir = rec.normal + random_unit_hemisphere(rec.normal);
			
			// Handle degenerate scatter dirs
			if(scatter_dir.near_null())
				scatter_dir = rec.normal;
			
			scattered = ray(rec.p, scatter_dir);
			attenuation = albedo;
			
			return true;
		}
};

class Metal : public IMaterial {
	private:
		color albedo;
		double fuzz;
	
	public:
		Metal(const color& albedo, const double& fuzz) : albedo(albedo), fuzz((fuzz < 1) ? fuzz : 1) {}
		
		bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
			vec3 reflected = reflect(r_in.direction(), rec.normal);
			reflected = normalized(reflected) + (fuzz * random_unit_vector());
			
			scattered = ray(rec.p, reflected);
			attenuation = albedo;
			
			return (dot(scattered.direction(), rec.normal) > 0);
		}
};

#endif