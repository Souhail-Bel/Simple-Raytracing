#ifndef BVH_H
#define BVH_H

#include "defs.h"

#include <algorithm>

class BVH_node : public IHittable {
	private:
		shared_ptr<IHittable> left;
		shared_ptr<IHittable> right;
		AABB bbox;
		
		static bool box_compare(
			const shared_ptr<IHittable> a,
			const shared_ptr<IHittable> b,
		int axis_index){
			auto a_axis_interval = a -> bounding_box().axis_interval(axis_index);
			auto b_axis_interval = b -> bounding_box().axis_interval(axis_index);
			return a_axis_interval.min < b_axis_interval.min;
		}
		
		static bool box_x_compare(
			const shared_ptr<IHittable> a,
			const shared_ptr<IHittable> b
		) { return box_compare(a, b, 0); }
		
		static bool box_y_compare(
			const shared_ptr<IHittable> a,
			const shared_ptr<IHittable> b
		) { return box_compare(a, b, 1); }
		
		static bool box_z_compare(
			const shared_ptr<IHittable> a,
			const shared_ptr<IHittable> b
		) { return box_compare(a, b, 2); }
	
	public:
		BVH_node(hittable_list list) : BVH_node(list.objects, 0, list.objects.size()) {
			
		}
		
		BVH_node(std::vector<shared_ptr<IHittable>>& objects, size_t start, size_t end) {
			bbox = AABB::empty;
			for(size_t obj_index = start; obj_index < end; obj_index++)
				bbox = AABB(bbox, objects[obj_index] -> bounding_box());
			
			int axis = bbox.longest_axis();
			
			auto comparator = (axis == 0) ? box_x_compare
					: (axis == 1) ? box_y_compare
					: box_z_compare;
			
			size_t object_span = end - start;
			
			if(object_span == 1) {
				left = right = objects[start];
			} else if(object_span == 2) {
				left = objects[start];
				right = objects[start+1];
			} else {
				std::sort(std::begin(objects) + start,
						std::begin(objects) + end,
						comparator);
				
				auto mid = start + object_span/2;
				left = make_shared<BVH_node>(objects, start, mid);
				right = make_shared<BVH_node>(objects, mid, end);
			}
			
		}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			if(!bbox.hit(r, ray_t))
				return false;
			
			bool hit_left  = left  -> hit(r, ray_t, rec);
			bool hit_right = right -> hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);
			
			return hit_left || hit_right;
		}
		
		AABB bounding_box() const override {return bbox;}
};

#endif