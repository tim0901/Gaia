#pragma once

#ifndef AABB_H
#define AABB_H

#include "ray.h"
#include "object.h"

//ffmin and ffmax are faster than fmin and fmax, but don't worry about NaNs and other exceptions
inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

//Defines axis-aligned bounding boxes, for accelerating intersection tests in complex scenes.

class aabb {
public:
	aabb() {}
	aabb(const vec3 &a, const vec3 &b) { _min = a; _max = b; }

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

	vec3 _min;
	vec3 _max;
};

//Returns a bounding box that surrounds two bounding boxes given to it. 
aabb surrounding_box(aabb box0, aabb box1) {

	vec3 small(fmin(box0.min().x(), box1.min().x()), fmin(box0.min().y(), box1.min().y()), fmin(box0.min().z(), box1.min().z()));

	vec3 big(fmax(box0.max().x(), box1.max().x()), fmax(box0.max().y(), box1.max().y()), fmax(box0.max().z(), box1.max().z()));

	return aabb(small, big);
}
#endif // !AABB_H
