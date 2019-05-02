#pragma once

#ifndef VEC4_H
#define VEC4_H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

//This class defines a vector3 structure and some basic vector operations

class vec4{
public:

	vec4() {}
	vec4(float e0, float e1, float e2, float e3) { e[0] = e0; e[1] = e1; e[2] = e2; e[3] = e3; }

	//Can use x, y, z or r, g, b to return stored values - vec3 can be used to store either.
	inline float w() const { return e[0]; }
	inline float x() const { return e[1]; }
	inline float y() const { return e[2]; }
	inline float z() const { return e[3]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }
	inline float a() const { return e[3]; }

	//Defining vector operators
	inline const vec4& operator + () const { return *this; }
	inline vec4 operator - () const { return vec4(-e[0], -e[1], -e[2], -e[3]); }
	inline float operator [] (int i) const { return e[i]; }
	inline float& operator [] (int i) { return e[i]; };

	inline vec4& operator += (const vec4 &v2);
	inline vec4& operator -= (const vec4 &v2);
	inline vec4& operator *= (const vec4 &v2);
	inline vec4& operator /= (const vec4 &v2);
	inline vec4& operator *= (const float t);
	inline vec4& operator /= (const float t);

	inline float length() const {
		return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3]);
	}

	inline float squared_length() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
	}

	inline float norm() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3]*e[3];
	}

	inline void make_unit_vector();

	float e[4];
};

//Vector addition
inline vec4 operator + (const vec4 &v1, const vec4 &v2) {
	return vec4(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2], v1.e[3] + v2.e[3]);
}

//Vector subtraction
inline vec4 operator - (const vec4 &v1, const vec4 &v2) {
	return vec4(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2], v1.e[3] - v2.e[3]);
}

//Vector multiplication
inline vec4 operator * (const vec4 &v1, const vec4 &v2) {
	return vec4(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2], v1.e[3] * v2.e[3]);
}

//Vector division
inline vec4 operator / (const vec4 &v1, const vec4 &v2) {
	return vec4(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2], v1.e[3] / v2.e[3]);
}

//Scalar multiplication
inline vec4 operator * (float t, const vec4 &v) {
	return vec4(t*v.e[0], t*v.e[1], t*v.e[2], t*v.e[3]);
}
inline vec4 operator * (const vec4 &v, float t) {
	return vec4(t*v.e[0], t*v.e[1], t*v.e[2], t*v.e[3]);
}

//Scalar division
inline vec4 operator / (vec4 v, float t) {
	return vec4(v.e[0] / t, v.e[1] / t, v.e[2] / t, v.e[3] / t);
}


//Dot product
inline float dot(const vec4 &v1, const vec4 &v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2] + v1.e[3] * v2.e[3];
}

//Cross Product



//Returns vector with abs applied to each component
inline vec4 abs(const vec4 &v) {
	return vec4(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]), std::abs(v[3]));
}

inline vec4 unit_vector(vec4 v) {
	return v / v.length();
}

inline void vec4::make_unit_vector() {
	float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3]);
	e[0] *= k; e[1] *= k; e[2] *= k; e[3] *= k;
}

//Permutes coordinates according to index values provided
inline vec4 permute(const vec4 &v, int w, int x, int y, int z) {
	return vec4(v[w], v[x], v[y], v[z]);
}

inline vec4& vec4::operator +=(const vec4 &v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	e[3] += v.e[3];
	return *this;
}

inline vec4& vec4::operator *=(const vec4 &v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	e[3] *= v.e[3];
	return *this;
}

inline vec4& vec4::operator /=(const vec4 &v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	e[3] /= v.e[3];
	return *this;
}

inline vec4& vec4::operator -=(const vec4 &v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	e[3] -= v.e[3];
	return *this;
}

inline vec4& vec4::operator *=(float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	e[3] *= t;
	return *this;
}

inline vec4& vec4::operator /=(float t) {
	float k = 1.0 / t;

	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
	e[3] *= k;
	return *this;
}

//For cout / cin
inline std::istream& operator >> (std::istream &is, vec4 &t) {
	is >> t.e[0] >> t.e[1] >> t.e[2] >> t.e[3];
	return is;
}

inline std::ostream& operator << (std::ostream &os, const vec4 &t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2] << " " << t.e[3];
	return os;
}

//Remove NaNs
inline vec4 de_nan(const vec4& c) {
	vec4 temp = c;
	if (!(temp[0] == temp[0])) temp[0] = 0;
	if (!(temp[1] == temp[1])) temp[1] = 0;
	if (!(temp[2] == temp[2])) temp[2] = 0;
	if (!(temp[3] == temp[3])) temp[3] = 0;
	return temp;
}



#endif // !VEC4_H
