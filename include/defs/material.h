#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class IMaterial {
	public:
		virtual ~IMaterial() = default;
		
		virtual color emitted(double u, double v, const point3& p) const {return color(0);}
		
		virtual bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {return 0;};
};

class Lambertian : public IMaterial {
	private:
		shared_ptr<ITexture> tex;
		
	public:
		Lambertian(const color& albedo) : tex(make_shared<Uniform_Color>(albedo)) {}
		
		Lambertian(shared_ptr<ITexture> tex) : tex(tex) {}
		
		bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
			vec3 scatter_dir = rec.normal + random_unit_hemisphere(rec.normal);
			
			// Handle degenerate scatter dirs
			if(scatter_dir.near_null())
				scatter_dir = rec.normal;
			
			scattered = ray(rec.p, scatter_dir, r_in.time());
			attenuation = tex->value(rec.u, rec.v, rec.p);
			
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
			
			scattered = ray(rec.p, reflected, r_in.time());
			attenuation = albedo;
			
			return (dot(scattered.direction(), rec.normal) > 0);
		}
};

class Dielectric : public IMaterial {
	private:
		double refraction_index;
		
		// Schlik's approx.
		static double reflectance(double cosine, double ri) {
			auto r0 = (1-ri)/(1+ri);
			r0 = r0*r0;
			return r0 + (1-r0)*std::pow((1-cosine),5);
		}
		
	public:
		Dielectric(double refraction_index) : refraction_index(refraction_index) {}
		
		bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
			attenuation = color(1);
			double rri = rec.is_front ? (1./refraction_index) : refraction_index;
			
			vec3 unit_dir = normalized(r_in.direction());
			
			double cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.);
			double sin_theta = std::sqrt(1. - cos_theta*cos_theta);
			
			
			bool beyond_critical = (rri * sin_theta) > 1.;
			beyond_critical |= reflectance(cos_theta, rri) > get_rand_double();
			
			vec3 dir = beyond_critical ? reflect(unit_dir, rec.normal) : refract(unit_dir, rec.normal, rri);
			
			scattered = ray(rec.p, dir, r_in.time());
			return true;
		}
};

class Emitter : public IMaterial {
	private:
		shared_ptr<ITexture> tex;
		
	public:
		Emitter(shared_ptr<ITexture> tex) : tex(tex) {}
		
		Emitter(const color& emit) : tex(make_shared<Uniform_Color>(emit)) {}
		
		color emitted(double u, double v, const point3& p) const override {
			return tex -> value(u, v, p);
		}
};

#endif