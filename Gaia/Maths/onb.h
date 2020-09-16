#pragma once

#ifndef ONB_H
#define ONB_H

#include "vec3.h"

class onb {
public:
	onb() {}
	inline vec3 operator[](int i) const { return axis[i]; }
	vec3 u()const { return axis[0]; }
	vec3 v()const { return axis[1]; }
	vec3 w()const { return axis[2]; }


	// Given a u/v/w coordinate, return the corresponding global x/y/z coordinate
	vec3 toGlobal(float a, float b, float c) const { return a * u() + b * v() + c * w(); }
	// Given a u/v/w coordinate, return the corresponding global x/y/z coordinate
	vec3 toGlobal(const vec3& a) const { return a.x() * u() + a.y() * v() + a.z() * w(); }

	// Given an x/y/z coordinate, return the corresponding local u/v/w coordinate
	vec3 toLocal(float a, float b, float c) const { return a * inverseAxis[0] + b * inverseAxis[1] + c * inverseAxis[2]; }
	// Given an x/y/z coordinate, return the corresponding local u/v/w coordinate
	vec3 toLocal(const vec3& a) const { return a.x() * inverseAxis[0] + a.y() * inverseAxis[1] + a.z() * inverseAxis[2]; }

	void build_from_w(const vec3& n);
	vec3 axis[3];
	vec3 inverseAxis[3];
};

#endif // !ONB_H
