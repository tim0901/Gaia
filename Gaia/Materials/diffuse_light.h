#pragma once

#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "vec3.h"
#include "material.h"

class diffuse_light :public material {
public:
	diffuse_light(vec3 *a) : emit(a) {}
    ~diffuse_light(){}
	virtual bool scatter(const ray &incident, const hit_record &rec, scattering_record &scatter) const {
		return false;
	}
	virtual vec3 emitted(const ray& incident, const hit_record &rec, float u, float v, const vec3& p) const { 

		if (dot(rec.normal, incident.direction()) < 0.0) {
			return *emit;
		}
		else {
			return vec3(0, 0, 0);
		}
	}

	virtual std::string type(int i = 0) const {
		return "diffuse_light";
	}
	vec3 *emit;


};

#endif // !DIFFUSE_LIGHT_H
