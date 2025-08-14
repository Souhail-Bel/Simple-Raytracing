#ifndef MATERIAL_H
#define MATERIAL_H

#inlucde "hittable.h"

class IMaterial {
	public:
		virtual ~IMaterial() = default;
		
		virtual bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {return false;}
}

class Lambertian : public IMaterial {
	private:
		color albedo;
		
	public:
		Lambertian(const color& albedo) : albedo(albedo) {}
		
		bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered){
			vec3 scatter_dir = rec.normal + random_unit_hemisphere(rec.normal);
			
			// Handle degenerate scatter dirs
			if(scatter_dir.near_null())
				scatter_dir = rec.normal;
			
			scattered = ray(rec.p, scatter_dir);
			attenuation = albedo;
			
			return true;
		}
}

#endif