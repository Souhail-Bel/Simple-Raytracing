#ifndef VEC3_H
#define VEC3_H

#include <cmath>

class vec3 {
	public:
		// Base components
		float e[3];
		
		// Empty vector with initializer list
		vec3() : e(0, 0, 0) {}
		
		vec3(float x, float y, float z) : e(x, y, z) {}
		
		float x() const {return e[0];}
		float y() const {return e[1];}
		float z() const {return e[2];}
		
		
}

#endif