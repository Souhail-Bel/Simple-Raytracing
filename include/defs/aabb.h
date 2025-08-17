#ifndef AABB_H
#define AABB_H

#include "utils.h"

class AABB {
	public:
		interval x_i, y_i, z_i;
		
		AABB() {}
		
		AABB(const interval& x_i, const interval& y_i, const interval& z_i) : x_i(x_i), y_i(y_i), z_i(z_i) {}
		
		AABB(const point3& A, const point3& B) {
			x_i = (A[0] <= B[0]) ?
				interval(A[0], B[0]):
				interval(B[0], A[0]);
			y_i = (A[1] <= B[1]) ?
				interval(A[1], B[1]):
				interval(B[1], A[1]);
			z_i = (A[2] <= B[2]) ?
				interval(A[2], B[2]):
				interval(B[2], A[2]);
		}
		
		AABB(const AABB& box_0, const AABB& box_1) {
			x_i = interval(box_0.x_i, box_1.x_i);
			y_i = interval(box_0.y_i, box_1.y_i);
			z_i = interval(box_0.z_i, box_1.z_i);
		}
		
		const interval& axis_interval(int n) const {
			if(n == 0) return x_i;
			if(n == 1) return y_i;
			return z_i;
		}
		
		bool hit(const ray& r, interval ray_t) const {
			const point3& ray_orig = r.origin();
			const vec3&	  ray_dir  = r.direction();
			
			for(int axis = 0; axis < 3; axis++) {
				const interval& ax = axis_interval(axis);
				const double adinv = 1.0 / ray_dir[axis];

				auto t0 = (ax.min - ray_orig[axis]) * adinv;
				auto t1 = (ax.max - ray_orig[axis]) * adinv;

				if (t0 < t1) {
					if (t0 > ray_t.min) ray_t.min = t0;
					if (t1 < ray_t.max) ray_t.max = t1;
				} else {
					if (t1 > ray_t.min) ray_t.min = t1;
					if (t0 < ray_t.max) ray_t.max = t0;
				}

				if (ray_t.max <= ray_t.min)
					return false;
			}
			
			return true;
		}
};

#endif