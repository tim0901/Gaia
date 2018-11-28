#pragma once

#ifndef METAL_H
#define METAL_H

#include "material.h"
#include "random.h"

class metal :public material {
public:
	metal(const vec3& a, float f) :albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	//Describes how a ray is scattered
	virtual bool scatter(const ray &incident, const hit_record &rec, vec3 &alb, ray &scattered) const {
		//Ray is reflected perfectly
		vec3 reflected = reflect(unit_vector(incident.direction()), rec.normal);
		//Fires new ray
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
		//Absorbs a little of the colour of the material
		alb = albedo;
		return (dot(scattered.direction(),rec.normal)>0);
	}
	vec3 albedo;
	float fuzz;
};

#endif // !METAL_H
