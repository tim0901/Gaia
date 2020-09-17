#pragma once

#ifndef ONB_H
#define ONB_H

#include "vec3.h"
#include "matrix3.h"

class onb {
public:
	onb() {}
	inline vec3 operator[](int i) const { return axes.row(i); }
	vec3 u()const { return axes.row(0); }
	vec3 v()const { return axes.row(1); }
	vec3 w()const { return axes.row(2); }


	// Given a u/v/w coordinate, return the corresponding global x/y/z coordinate
	vec3 toGlobal(float a, float b, float c) const { return a * u() + b * v() + c * w(); }
	// Given a u/v/w coordinate, return the corresponding global x/y/z coordinate
	vec3 toGlobal(const vec3& a) const { return a.x() * u() + a.y() * v() + a.z() * w(); }

	// Given an x/y/z coordinate, return the corresponding local u/v/w coordinate
	vec3 toLocal(float a, float b, float c) const { return a * inverseAxes.row(0) + b * inverseAxes.row(1) + c * inverseAxes.row(2); }
	// Given an x/y/z coordinate, return the corresponding local u/v/w coordinate
	vec3 toLocal(const vec3& a) const { return a.x() * inverseAxes.row(0) + a.y() * inverseAxes.row(1) + a.z() * inverseAxes.row(2); }

	void build_from_w(const vec3& n);
	void build_from_w_with_a(const vec3& n, const vec3& a);

	matrix3 axes;
	matrix3 inverseAxes;
};

#endif // !ONB_H
