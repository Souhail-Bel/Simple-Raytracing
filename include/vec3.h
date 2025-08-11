#ifndef VEC3_H
#define VEC3_H

#include <cmath>

class vec3 {
	public:		
		// Base components
		double e[3];
		
		// Empty vector with initializer list
		vec3() : e{0, 0, 0} {}
		
		vec3(float u) : e {u, u, u} {}
		
		vec3(float x, double y, double z) : e{x, y, z} {}
		
		double x() const {return e[0];}
		double y() const {return e[1];}
		double z() const {return e[2];}
		
		// Indiv elements
		double operator[](int i) const {return e[i];}
		double& operator[](int i){return e[i];}
		
		vec3 operator-() const {
			return vec3(-e[0], -e[1], -e[2]);
		}
		
		vec3& operator+=(const vec3& v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			return *this;
		}
		
		vec3& operator*=(double a) {
			e[0] *= a;
			e[1] *= a;
			e[2] *= a;
			return *this;
		}
		
		vec3& operator/=(double a){
			return *this *= (1/a);
		}
		
		double len_sqr() const {
			return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
		}
		
		double len() const {
			return std::sqrt(len_sqr());
		}
};

using point3 = vec3;

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double a, const vec3& v) {
	return vec3(a * v.e[0], a * v.e[1], a * v.e[2]);
}

inline vec3 operator*(const vec3& v, double a) {
	return a * v;
}

inline vec3 operator/(const vec3& v, double a) {
	return (1/a) * v;
}

inline double dot(const vec3& u, const vec3& v){
	return (u.e[0]*v.e[0]) + (u.e[1]*v.e[1]) + (u.e[2]*v.e[2]);
}

inline vec3 cross(const vec3& u, const vec3& v){
	return vec3(u.e[1]*v.e[2] - u.e[2]*v.e[1],
				u.e[2]*v.e[0] - u.e[0]*v.e[2],
				u.e[0]*v.e[1] - u.e[1]*v.e[0]);
}

inline vec3 normalized(const vec3& v) {
	return v / v.len();
}

#endif