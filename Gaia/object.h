#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "hit_record.h"
#include "ray.h"
#include "aabb.h"

class object{
public:
	//Hit function. Takes ray, returns a hit_record of the object if it hits anything
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb &box) const = 0;
	virtual float pdf_value(const vec3 &o, const vec3 &v) const { return 0.0; }
	virtual vec3 random(const vec3& o) const { return vec3(1, 0, 0); }
	float object_id = -1;
	float primitive_id = -1;
};
#endif // !OBJECT_H