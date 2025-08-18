#ifndef HITTABLE_H
#define HITTABLE_H

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

#include "utils.h"
#include "aabb.h"

class IMaterial;

class hit_record {
	public:
		point3 p;
		vec3 normal;
		double t;
		bool is_front;
		
		// UV surface coords
		double u;
		double v;
		
		shared_ptr<IMaterial> mat;
		
		// 'ext_normal' is assumed normalized
		void set_face_normal(const ray& r, const vec3& ext_normal) {
			is_front = dot(r.direction(), ext_normal) < 0;
			normal = is_front ? ext_normal : -ext_normal;
		}
};

class IHittable {
	public:
		virtual ~IHittable() = default;
		
		virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
		
		virtual AABB bounding_box() const = 0;
};


// Shared pointers allow multiple geometries to share a common instance (i.e. multiple spheres, same material)

class hittable_list : public IHittable {
	private:
		AABB bbox;
	public:
		std::vector<shared_ptr<IHittable>> objects;
		
		hittable_list() {}
		hittable_list(shared_ptr<IHittable> obj) {add(obj);}
		
		void clear() {objects.clear();}
		
		void add(shared_ptr<IHittable> obj){
			objects.push_back(obj);
			bbox = AABB(bbox, obj -> bounding_box());
		}
		
		AABB bounding_box() const override {return bbox;}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			
			hit_record temp_rec;
			bool got_hit = false;
			double closest = ray_t.max;
			
			for(const auto& obj : objects){
				if(obj -> hit(r, interval(ray_t.min, closest), temp_rec)) {
					got_hit = true;
					closest = temp_rec.t;
					rec = temp_rec;
				}
			}
			
			return got_hit;
		}
};

#endif