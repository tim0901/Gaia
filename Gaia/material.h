#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "ray.h"

struct hit_record;

//Defines a material
class material {
public:
	//Scatter function
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& albedo, ray& scattered, float& pdf) const = 0;
	virtual vec3 emitted(float u, float v, const vec3& p) const { return vec3(0, 0, 0); }
	virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const { return false; }

	std::string type = "undefined";

};

//Describes a perfect reflection as used by metals and glasses
vec3 reflect(const vec3 &v, const vec3 &n) {
	return v - 2 * dot(v, n)*n;
}

//Determines whether a ray is refracted or not
bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);
	if (discriminant > 0) {
		//Refracted
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else {
		return false;
	}

}

#endif // !MATERIAL_H
