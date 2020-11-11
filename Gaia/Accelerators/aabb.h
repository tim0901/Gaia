#pragma once

#ifndef AABB_H
#define AABB_H

#include "ray.h"

//ffmin and ffmax are faster than fmin and fmax, but don't worry about NaNs and other exceptions
inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

//Defines axis-aligned bounding boxes, for accelerating intersection tests in complex scenes.

class aabb {
public:
	aabb() {
		// By setting _max to be smaller than _min, we create an invalid configuration that
		// violates eg _max.x > _min.x and therefore any operations involving an empty box
		// will yield the correct result.
		_min = vec3(FLT_MAX, FLT_MAX, FLT_MAX); _max = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	}
	aabb(const vec3 &a, const vec3 &b) { _min = a; _max = b; }
    ~aabb(){}

	vec3 min() const { return _min; }
	vec3 max() const { return _max; }

	bool hit(const ray &r, float t_min, float t_max) const {
		for (int a = 0; a < 3; a++) {
			float invD = 1.0f / r.direction()[a];
			float t0 = (min()[a] - r.origin()[a]) * invD;
			float t1 = (max()[a] - r.origin()[a]) * invD;
			if (invD < 0) {
				std::swap(t0, t1);
			}
			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;
			if (t_max <= t_min) {
				return false;
			}
		}
		return true;
	}

	// Return the relative position of a point within an aabb
	// At _min this would be (0,0,0), at _max it would be (1,1,1)
	vec3 positionInside(const vec3& p) const {
		vec3 o = p - _min; // Shift to local

		if (_max.x() > _min.x())
			o[0] /= (_max.x() - _min.x());

		if (_max.y() > _min.y())
			o[1] /= (_max.y() - _min.y());

		if (_max.z() > _min.z())
			o[2] /= (_max.z() - _min.z());
		return o;
	}
	
	// Returns the index of which axis is longest
	int longestAxis() const {
		vec3 diagonal = _max - _min;
		if (diagonal.x() > diagonal.y() && diagonal.x() > diagonal.z()) {
			return 0;
		}
		else if (diagonal.y() > diagonal.z()) {
			return 1;
		}
		else {
			return 2;
		}
	}

	// Returns the surface area of the aabb
	double surfaceArea() const {
		vec3 d = _max - _min;
		return 2.0 * ((double)(d.x() * d.y()) + (double)(d.y() * d.z()) + (double)(d.z() * d.z()));
	}

	// Returns the volume of the aabb
	float volume() const {
		vec3 d = _max - _min;
		return d.x() * d.y() * d.z();
	}

	vec3 _min;
	vec3 _max;
};

//For cout
inline std::ostream& operator << (std::ostream& os, const aabb& box) {
	os << box.min() << " " << box.max();
	return os;
}



aabb surrounding_box(const aabb &box0, const aabb &box1);
aabb surrounding_box(const aabb &box0, const vec3 &point);
#endif // !AABB_H
