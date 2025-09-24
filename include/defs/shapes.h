#ifndef SHAPES_H
#define SHAPES_H

#include "utils.h"
#include "defs/hittable.h"

class Quad : public IHittable {
	private:
		point3 Q;
		vec3 u, v;
		vec3 w;
		
		shared_ptr<IMaterial> mat;
		AABB bbox;
		
		vec3 normal;
		double d;
	
	public:
		Quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<IMaterial> mat) : Q(Q), u(u), v(v), mat(mat) {
			vec3 n = cross(u, v);
			normal = normalized(n);
			d = dot(normal, Q);
			w = n / dot(n, n);
			
			bbox = AABB(
				AABB(Q, Q+u+v),
				AABB(Q+u, Q+v)
			);
		}
		
		AABB bounding_box() const override {return bbox;}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			auto denom = dot(normal, r.direction());
			
			if(std::fabs(denom) < 1e-8) return false;
		
			auto t = (d - dot(normal, r.origin())) / denom;
			
			if(!ray_t.has_closed(t)) return false;
			
			point3 intersection = r.at(t);
			vec3 p = intersection - Q;
			auto alpha = dot(w, cross(p, v));
			auto beta  = dot(w, cross(u, p));
			
			if(!interval::unit.has_closed(alpha)
				|| !interval::unit.has_closed(beta))
				return false;
			
			rec.u = alpha;
			rec.v = beta;
			
			
			rec.t = t;
			rec.p = intersection;
			rec.mat = mat;
			rec.set_face_normal(r, normal);
			
			return true;
		}
};


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

class Constant_Medium : public IHittable {
	private:
		shared_ptr<IHittable> boundary;
		double neg_inv_density;
		shared_ptr<IMaterial> phase_function;
	
	public:
		Constant_Medium(shared_ptr<IHittable> boundary, double density, shared_ptr<ITexture> tex)
		: boundary(boundary), neg_inv_density(-1./density),  phase_function(make_shared<Isotropic>(tex)) {}
		
		Constant_Medium(shared_ptr<IHittable> boundary, double density, const color& albedo)
		: boundary(boundary), neg_inv_density(-1./density),  phase_function(make_shared<Isotropic>(albedo)) {}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			hit_record rec1, rec2;
			
			if(!boundary -> hit(r, interval::universe, rec1)) return false;
			
			if(!boundary -> hit(r, interval(rec1.t+0.0001, inf), rec2)) return false;
			
			if (rec1.t < ray_t.min) rec1.t = ray_t.min;
			if (rec2.t > ray_t.max) rec2.t = ray_t.max;
			
			if (rec1.t >= rec2.t)
				return false;
			
			if(rec1.t < 0) rec1.t = 0;
			
			auto ray_length = r.direction().len();
			auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
			auto hit_distance = neg_inv_density * std::log(get_rand_double());
			
			if(hit_distance > distance_inside_boundary)
				return false;
			
			rec.t = rec1.t + hit_distance/ray_length;
			rec.p = r.at(rec.t);
			
			rec.normal = vec3(0,1,0);
			rec.is_front = true;
			rec.mat = phase_function;
			
			return true;
		}
		
		AABB bounding_box() const override {
			return boundary -> bounding_box();
		}
		
};

#endif