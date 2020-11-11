#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "material.h"
#include "vec2.h"
#include "ray.h"
#include "aabb.h"

class aabb;

class object {
public:
	virtual ~object() {}
	//Hit function. Takes ray, returns a hit_record of the object if it hits anything
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
	virtual float pdf_value(const vec3& o, const vec3& v) const { return 0.0; }
	virtual vec3 random(const vec3& o) const { return vec3(1, 0, 0); }

	virtual vec3 normal() const { return vec3(0, 0, 0); }

	virtual vec2 positionToUV(const vec3& p) const { return vec2(0, 0); }
	virtual vec3 UVToPosition(const vec2& uv) const { return vec3(0, 0, 0); }

	virtual vec3 centroid() const {return vec3(0, 0, 0);}

	float object_id = -1;
	float primitive_id = -1;
    std::string type = "undefined";
};
#endif // !OBJECT_H
