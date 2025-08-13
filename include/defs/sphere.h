#ifndef SPHERE_H
#define SPHERE_H

#include "utils.h"
#include "defs/hittable.h"

// Sphere to inherit from hittable interface

class Sphere : public IHittable {
	private:
		point3 center;
		float radius;
	
	public:
		Sphere(const point3& c, float r) : center(c), radius(std::fabsf(r)) {}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			vec3 OC = center - r.origin();
	
			// Solves quadratic equation for sphere
			// b' is used because b is even
			double a = r.direction().len_sqr();
			double b_pr = dot(r.direction(), OC);
			double c = OC.len_sqr() - radius * radius;
			
			double del = (b_pr*b_pr - a*c);
			
			if(del < 0)
				return false;
			
			double sqrt_del = sqrt(del);
			
			// Root within range
			double t = (b_pr - sqrt_del) / a;
			if(!ray_t.has_open(t)) {
				t = (b_pr + sqrt_del) / a;
				
				if(!ray_t.has_open(t))
					return false;
			}
			
			rec.t = t;
			rec.p = r.at(t);
			rec.set_face_normal(r, (rec.p - center) / radius);
			
			return true;
		}
};

#endif