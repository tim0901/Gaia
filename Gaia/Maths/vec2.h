#pragma once

#ifndef VEC2_H
#define VEC2_H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

class vec2 {
public:
	vec2() {}
	vec2(float e0, float e1) { e[0] = e0; e[1] = e1; }

	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float u() const { return e[0]; }
	inline float v() const { return e[1]; }

	//Defining vector operators
	inline const vec2& operator + () const { return *this; }
	inline vec2 operator - () const { return vec2(-e[0], -e[1]); }
	inline float operator [] (int i) const { return e[i]; }
	inline float& operator [] (int i) { return e[i]; };

	inline vec2& operator += (const vec2 &v2);
	inline vec2& operator -= (const vec2 &v2);
	inline vec2& operator *= (const vec2 &v2);
	inline vec2& operator /= (const vec2 &v2);
	inline vec2& operator *= (const float t);
	inline vec2& operator /= (const float t);

	inline float length() const {
		return sqrt(e[0] * e[0] + e[1] * e[1]);
	}

	inline float squared_length() const {
		return e[0] * e[0] + e[1] * e[1];
	}

	inline float norm() const {
		return e[0] * e[0] + e[1] * e[1];
	}

	inline void make_unit_vector();

	float e[2];
};

//Vector addition
inline vec2 operator + (const vec2 &v1, const vec2 &v2) {
	return vec2(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1]);
}

//Vector subtraction
inline vec2 operator - (const vec2 &v1, const vec2 &v2) {
	return vec2(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1]);
}

//Vector multiplication
inline vec2 operator * (const vec2 &v1, const vec2 &v2) {
	return vec2(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1]);
}

//Vector division
inline vec2 operator / (const vec2 &v1, const vec2 &v2) {
	return vec2(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1]);
}

//Scalar multiplication
inline vec2 operator * (float t, const vec2 &v) {
	return vec2(t*v.e[0], t*v.e[1]);
}
inline vec2 operator * (const vec2 &v, float t) {
	return vec2(t*v.e[0], t*v.e[1]);
}

//Scalar division
inline vec2 operator / (vec2 v, float t) {
	return vec2(v.e[0] / t, v.e[1] / t);
}

//Dot product
inline float dot(const vec2 &v1, const vec2 &v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1];
}

//Returns vector with abs applied to each component
inline vec2 abs(const vec2 &v) {
	return vec2(std::abs(v[0]), std::abs(v[1]));
}

inline float max_component(const vec2 &v) {
	return std::max(v[0], v[1]);
}

inline float min_component(const vec2 &v) {
	return std::min(v[0], v[1]);
}

inline vec2 unit_vector(vec2 v) {
	return v / v.length();
}

inline void vec2::make_unit_vector() {
	float k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1]);
	e[0] *= k; e[1] *= k;
}

inline vec2& vec2::operator +=(const vec2 &v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	return *this;
}

inline vec2& vec2::operator *=(const vec2 &v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	return *this;
}

inline vec2& vec2::operator /=(const vec2 &v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	return *this;
}

inline vec2& vec2::operator -=(const vec2 &v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	return *this;
}

inline vec2& vec2::operator *=(float t) {
	e[0] *= t;
	e[1] *= t;
	return *this;
}

inline vec2& vec2::operator /=(float t) {
	float k = 1.0 / t;

	e[0] *= k;
	e[1] *= k;
	return *this;
}

//For cout / cin
inline std::istream& operator >> (std::istream &is, vec2 &t) {
	is >> t.e[0] >> t.e[1] ;
	return is;
}

inline std::ostream& operator << (std::ostream &os, const vec2 &t) {
	os << t.e[0] << " " << t.e[1];
	return os;
}
#endif // !VEC2_H
