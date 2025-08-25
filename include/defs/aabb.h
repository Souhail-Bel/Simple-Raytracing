#ifndef AABB_H
#define AABB_H

#include "utils.h"

class AABB {
	private:
		void pad_to_min() {
			double delta = .0001;
			
			if(x_i.size() < delta) x_i = x_i.expand(delta);
			if(y_i.size() < delta) y_i = y_i.expand(delta);
			if(z_i.size() < delta) z_i = z_i.expand(delta);
		}
	
	public:
		interval x_i, y_i, z_i;
		
		AABB() {}
		
		AABB(const interval& inter) : x_i(inter), y_i(inter), z_i(inter) {
			pad_to_min();
		}
		
		AABB(const interval& x_i, const interval& y_i, const interval& z_i) : x_i(x_i), y_i(y_i), z_i(z_i) {
			pad_to_min();
		}
		
		AABB(const point3& A, const point3& B) {
			x_i = interval(std::min(A[0], B[0]), std::max(A[0], B[0]));
			y_i = interval(std::min(A[1], B[1]), std::max(A[1], B[1]));
			z_i = interval(std::min(A[2], B[2]), std::max(A[2], B[2]));
			
			pad_to_min();
		}
		
		AABB(const AABB& box_0, const AABB& box_1) {
			x_i = interval(box_0.x_i, box_1.x_i);
			y_i = interval(box_0.y_i, box_1.y_i);
			z_i = interval(box_0.z_i, box_1.z_i);
		}
		
		const interval& axis_interval(int n) const {
			switch(n) {
				case 0:  return x_i;
				case 1:  return y_i;
				default: return z_i;
			}
		}
		
		bool hit(const ray& r, interval ray_t) const {
			const point3& ray_orig = r.origin();
			const vec3&	  ray_dir  = r.direction();
			
			for(int axis = 0; axis < 3; axis++) {
				const interval& ax = axis_interval(axis);
				const double adinv = 1.0 / ray_dir[axis];

				auto t0 = (ax.min - ray_orig[axis]) * adinv;
				auto t1 = (ax.max - ray_orig[axis]) * adinv;
				
				if(adinv < 0)
					std::swap(t0, t1);
				
				ray_t.min = std::max(t0, ray_t.min);
				ray_t.max = std::min(t1, ray_t.max);

				if (ray_t.max <= ray_t.min)
					return false;
			}
			
			return true;
		}
		
		int longest_axis() const {
			if(x_i.size() > y_i.size())
				return x_i.size() > z_i.size() ? 0 : 2;
			return y_i.size() > z_i.size() ? 1 : 2;
		}
		
		static const AABB empty, universe;
};

const AABB AABB::empty    = AABB(interval::empty);
const AABB AABB::universe = AABB(interval::universe);

#endif