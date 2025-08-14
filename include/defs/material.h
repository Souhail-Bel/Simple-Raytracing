#ifndef MATERIAL_H
#define MATERIAL_H

#inlucde "hittable.h"

class IMaterial {
	public:
		virtual ~IMaterial() = default;
		
		virtual bool scatter (const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {return false;}
}

#endif