#pragma once

#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

//This class defines a vector3 structure and some basic vector operations

class vec3 {
public:

	vec3() {}
	vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

	//Can use x, y, z or r, g, b to return stored values - vec3 can be used to store either.
	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }

	//Defining vector operators
	inline const vec3& operator + () const { return *this; }
	inline vec3 operator - () const { return vec3(-e[0], -e[1], -e[2]); }
	inline float operator [] (int i) const { return e[i]; }
	inline float& operator [] (int i) { return e[i]; };

	inline vec3& operator += (const vec3 &v2);
	inline vec3& operator -= (const vec3 &v2);
	inline vec3& operator *= (const vec3 &v2);
	inline vec3& operator /= (const vec3 &v2);
	inline vec3& operator *= (const float t);
	inline vec3& operator /= (const float t);

	inline float length() const {
		return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}

	inline float squared_length() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline float norm() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline void make_unit_vector();

	float e[3];
};

//Vector addition
inline vec3 operator + (const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

//Vector subtraction
inline vec3 operator - (const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

//Vector multiplication
inline vec3 operator * (const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

//Vector division
inline vec3 operator / (const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

//Scalar multiplication
inline vec3 operator * (float t, const vec3 &v) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}
inline vec3 operator * (const vec3 &v, float t) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

//Scalar division
inline vec3 operator / (vec3 v, float t) {
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

//Dot product
inline float dot(const vec3 &v1, const vec3 &v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

//Cross product
inline vec3 cross(const vec3 &v1, const vec3 &v2) {
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
		(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

//Returns vector with abs applied to each component
inline vec3 abs(const vec3 &v) {
	return vec3(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]));
}

//Returns the index of the component with the largest value
inline int max_dimension(const vec3 &v) {
	return (v[0] > v[1]) ? ((v[0] > v[2]) ? 0 : 2) : ((v[1] > v[2]) ? 1 : 2);
}

inline int min_dimension(const vec3 &v) {
	return (v[0] < v[1]) ? ((v[0] < v[2]) ? 0 : 2) : ((v[1] < v[2]) ? 1 : 2);
}

inline float max_component(const vec3 &v) {
	return std::max(v[0], std::max(v[1], v[2]));
}

inline float min_component(const vec3 &v) {
	return std::min(v[0], std::min(v[1], v[2]));
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

inline void vec3::make_unit_vector() {
	float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	e[0] *= k; e[1] *= k; e[2] *= k;
}

//Permutes coordinates according to index values provided
inline vec3 permute(const vec3 &v, int x, int y, int z) {
	return vec3(v[x], v[y], v[z]);
}

//Creates local coordinate system from a single normalized 3D vector
inline void coordinate_system(const vec3& v1, vec3* v2, vec3* v3) {
	if (std::abs(v1[0]) > std::abs(v1[1])) {
		*v2 = vec3(-v1[2], 0, v1[0]) / sqrt(v1[0] * v1[0] + v1[2] * v1[2]);
	}
	else {
		*v2 = vec3(0, v1[2], -v1[1]) / sqrt(v1[1] * v1[1] + v1[2] * v1[2]);
	}
	*v3 = cross(v1, *v2);
}

inline vec3& vec3::operator +=(const vec3 &v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}

inline vec3& vec3::operator *=(const vec3 &v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}

inline vec3& vec3::operator /=(const vec3 &v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}

inline vec3& vec3::operator -=(const vec3 &v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}

inline vec3& vec3::operator *=(float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

inline vec3& vec3::operator /=(float t) {
	float k = 1.0 / t;

	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
	return *this;
}

//For cout / cin
inline std::istream& operator >> (std::istream &is, vec3 &t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}

inline std::ostream& operator << (std::ostream &os, const vec3 &t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}

//Remove NaNs
inline vec3 de_nan(const vec3& c) {
	vec3 temp = c;
	if (!(temp[0] == temp[0])) temp[0] = 0;
	if (!(temp[1] == temp[1])) temp[1] = 0;
	if (!(temp[2] == temp[2])) temp[1] = 0;
	return temp;
}

#endif //VEC3_H
