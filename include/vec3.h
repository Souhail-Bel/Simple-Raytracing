#ifndef VEC3_H
#define VEC3_H

#include <cmath>

class vec3 {
	public:
		// Base components
		double e[3];
		
		// Empty vector with initializer list
		vec3() : e(0, 0, 0) {}
		
		vec3(float x, double y, double z) : e(x, y, z) {}
		
		double x() const {return e[0];}
		double y() const {return e[1];}
		double z() const {return e[2];}
		
		
}

#endif