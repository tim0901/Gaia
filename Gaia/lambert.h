#pragma once

#ifndef LAMBERT_H
#define LAMBERT_H

#include "material.h"
#include "random.h"

class lambertian :public material {
public:
	lambertian(const vec3& a) :albedo(a) {}
	//Describes how a ray is scattered
	virtual bool scatter(const ray &incident, const hit_record &rec, vec3 &brdf, ray &scattered) const {
		//New ray direction is uniformly sampled from a sphere
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		//Fires new ray
		scattered = ray(rec.p, target - rec.p);
		//Absorbs a little of the colour of the material
		brdf = albedo/M_PI;
		return true;
	}
	vec3 albedo;
};

#endif // !LAMBERT_H
