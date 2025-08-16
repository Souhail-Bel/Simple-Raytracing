#ifndef SPHERE_H
#define SPHERE_H

#include "utils.h"
#include "defs/hittable.h"

// Sphere to inherit from hittable interface

class Sphere : public IHittable {
	private:
		point3 center;
		float radius;
		shared_ptr<IMaterial> mat;
	
	public:
		Sphere(const point3& c, float r, shared_ptr<IMaterial> mat) : center(c), radius(std::fabsf(r)), mat(mat) {}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			vec3 OC = center - r.origin();
	
			// Solves quadratic equation for sphere
			// b' is used because b is even
			float a = r.direction().len_sqr();
			float b_pr = dot(r.direction(), OC);
			float c = OC.len_sqr() - radius * radius;
			
			float del = (b_pr*b_pr - a*c);
			
			if(del < 0)
				return false;
			
			float sqrt_del = sqrt(del);
			
			// Root within range
			float t = (b_pr - sqrt_del) / a;
			if(!ray_t.has_open(t)) {
				t = (b_pr + sqrt_del) / a;
				
				if(!ray_t.has_open(t))
					return false;
			}
			
			rec.t = t;
			rec.p = r.at(t);
			rec.set_face_normal(r, (rec.p - center) / radius);
			rec.mat = mat;
			
			return true;
		}
};

#endif