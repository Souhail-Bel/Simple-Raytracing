#ifndef LBVH_H
#define LBVH_H

#include "defs.h"

#include <algorithm>
#include <omp.h>

struct BVH_node {
	AABB 	 bbox;
	uint32_t left, right;
	uint8_t  axis;
	bool 	 leaf;
};

class LBVH : public IHittable {
	private:
		static constexpr int leafThreshold = 4;
		std::vector<BVH_node> nodes;
		std::vector<std::shared_ptr<IHittable>> primitives_register;
		
		struct ObjectDef {
			std::shared_ptr<IHittable> obj;
			AABB bbox;
			point3 centroid;
		};

	public:
		LBVH(const hittable_list& list) : LBVH(list.objects) {}
		
		LBVH(const std::vector<std::shared_ptr<IHittable>>& objects) {
			if(objects.empty()) return;
			
			const size_t object_count = objects.size();
			
			std::vector<ObjectDef> entries(object_count);
			
			//#pragma omp parallel for
			// For some reason, this deletes a random object from the scene if not indexed
			// for(auto& obj : objects){
				// AABB box = obj -> bounding_box();
				// entries.push_back({obj, box, aabb_centroid(box)});
			// }
			
			for(size_t i = 0; i < object_count; i++){
				AABB box = objects[i] -> bounding_box();
				entries[i] = {objects[i], box, aabb_centroid(box)};
			}
			
			nodes.reserve(object_count * 2);
			primitives_register.reserve(object_count);
			
			// Parallel recursion
			#pragma omp parallel
			{
				// Call executed once per thread
				#pragma omp single
				{
					construct(entries, 0, entries.size());
				}
			}
		}
		
		AABB bounding_box() const override {
			if (nodes.empty()) return AABB::empty;
			return nodes[0].bbox; // root node’s bbox covers the whole BVH
		}

		
		// brain crumbles beyond this point.
		uint32_t construct(
			std::vector<ObjectDef>& entries,
			size_t start, size_t end
		) {
			const uint32_t idx = nodes.size();
			nodes.emplace_back();
			
			
			AABB bbox = AABB::empty;
			for(size_t obj_index = start; obj_index < end; obj_index++)
				bbox = AABB(bbox, entries[obj_index].bbox);
			
			const size_t n = end - start;
			BVH_node& node = nodes[idx];
			node.bbox = bbox;
			
			if(n <= leafThreshold) {
				node.left = primitives_register.size();
				node.right = n;
				node.leaf = true;
				node.axis = 0;
				for(size_t i = start; i < end; i++)
					primitives_register.emplace_back(entries[i].obj);
				return idx;
			}
			
			AABB centroid_box = AABB::empty;
			for(size_t i = start; i < end; i++)
				centroid_box = AABB(centroid_box, AABB(entries[i].centroid, entries[i].centroid));
			int axis = centroid_box.longest_axis();
			
			if(centroid_box.axis_interval(axis).size() == 0.) {
				node.left = primitives_register.size();
				node.right = n;
				node.leaf = true;
				node.axis = axis;
				for(size_t i = start; i < end; i++)
					primitives_register.emplace_back(entries[i].obj);
				return idx;
			}
			
			
			// Median split
			// to be replace with SAH
			// once my neurons fire
			size_t mid = start + (n/2);
			std::nth_element(entries.begin() + start, entries.begin() + mid, entries.begin() + end,
			[axis](const ObjectDef& a, const ObjectDef& b) {
				return a.centroid[axis] < b.centroid[axis];
			});
			
			uint32_t left_child  = construct(entries, start, mid);
			uint32_t right_child = construct(entries, mid  , end);
			
			node.left  = left_child;
			node.right = right_child;
			node.leaf  = false;
			node.axis  = axis;
			return idx;
		}
		
		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			if(nodes.empty()) return false;
			
			bool got_hit = false;
			uint32_t stack[64];
			int sp = 0;
			stack[sp++] = 0;
			
			const vec3 ray_dir = r.direction();
			
			while(sp > 0) {
				uint32_t idx = stack[--sp];
				const BVH_node& node = nodes[idx];
				
				if(!node.bbox.hit(r, ray_t)) continue;
				
				if(node.leaf) {
					uint32_t offset = node.left;
					uint32_t count  = node.right;
					
					for(uint32_t i = 0; i < count; i++) {
						if(primitives_register[offset+i]->hit(r, ray_t, rec)) {
							got_hit = true;
							ray_t.max = rec.t;
						}
					}
				} else {
					if(ray_dir[node.axis] < 0) {
						stack[sp++] = node.left;
						stack[sp++] = node.right;
					} else {
						stack[sp++] = node.right;
						stack[sp++] = node.left;
					}
				}
			}
			
			return got_hit;
		}
};

#endif