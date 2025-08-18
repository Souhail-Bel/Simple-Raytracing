#ifndef SPHERE_H
#define SPHERE_H

#include "utils.h"
#include "defs/hittable.h"

// Sphere to inherit from hittable interface

class Sphere : public IHittable {
	private:
		ray center;
		float radius;
		shared_ptr<IMaterial> mat;
		AABB bbox;
		
		static void get_uv(const point3& p, double& u, double& v) {
			// u and v go from 0 to 1
			// u angle around Y axis from -X
			// v angle from -Y to +Y
			
			auto theta = std::acos(-p.y());
			auto phi   = std::atan2(-p.z(), p.x()) + PI;
			
			u = phi / TWO_PI;
			v = theta / PI;
		}
	
	public:
		// Static sphere
		Sphere(const point3& c, float r, shared_ptr<IMaterial> mat) : center(c, vec3(0)), radius(std::fabsf(r)), mat(mat) {
			vec3 r_vec = vec3(radius);
			bbox = AABB(c - r_vec, c + r_vec);
		}
		
		// Moving sphere
		Sphere(const point3& c1, const point3& c2, float r, shared_ptr<IMaterial> mat) : center(c1, c2 - c1), radius(std::fabsf(r)), mat(mat) {
			vec3 r_vec = vec3(radius);
			AABB box_0(center.at(0) - r_vec, center.at(0) + r_vec);
			AABB box_1(center.at(1) - r_vec, center.at(1) + r_vec);
			bbox = AABB(box_0, box_1);
		}
		
		AABB bounding_box() const override {return bbox;}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			point3 curr_center = center.at(r.time());
			
			vec3 OC = curr_center - r.origin();
	
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
			vec3 out_normal = (rec.p - curr_center) / radius;
			rec.set_face_normal(r, out_normal);
			get_uv(out_normal, rec.u, rec.v);
			rec.mat = mat;
			
			return true;
		}
};

#endif