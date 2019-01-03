#pragma once

#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "vec3.h"
#include "material.h"

class diffuse_light :public material {
public:
	diffuse_light(vec3 *a) : emit(a) {}
	virtual bool scatter(const ray &incident, const hit_record &rec, vec3 &alb, ray& scattered, float &pdf) const {
		return false;
	}
	virtual vec3 emitted(float u, float v, const vec3& p) const { return *emit; }

	vec3 *emit;

	std::string type = "diffuse_light";

};

#endif // !DIFFUSE_LIGHT_H
