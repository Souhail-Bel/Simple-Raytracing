#ifndef HITTABLE_H
#define HITTABLE_H

#include "utils/ray.h"

class hit_record {
	public:
		point3 p;
		vec3 normal;
		double t;
		bool is_front;
		
		// 'ext_normal' is assumed normalized
		void set_face_normal(const ray& r, const vec3& ext_normal) {
			is_front = dot(r.direction(), ext_normal) < 0;
			normal = is_front ? ext_normal : -ext_normal;
		}
};

class IHittable {
	public:
		virtual ~IHittable() = default;
		
		virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};
#endif